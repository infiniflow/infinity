//! Recursive-descent SQL parser (subset).

use infinity_common::{Error, LogicalType, Result};

use crate::ast::{
    AggFunc, BinOp, BoolOp, CmpOp, ColumnDef, ExprAst, JoinClause, Literal, MatchClause, OrderKey,
    Predicate, SearchClause, SelectAgg, SelectItem, Statement,
};
use crate::lexer::{tokenize, Token};

/// Parses zero or more `;`-separated statements.
pub fn parse(sql: &str) -> Result<Vec<Statement>> {
    let mut p = Parser {
        toks: tokenize(sql)?,
        pos: 0,
    };
    let mut out = Vec::new();
    loop {
        while p.peek() == &Token::Semicolon {
            p.advance();
        }
        if p.peek() == &Token::Eof {
            break;
        }
        out.push(p.statement()?);
        if p.peek() == &Token::Semicolon {
            p.advance();
        }
    }
    Ok(out)
}

struct Parser {
    toks: Vec<Token>,
    pos: usize,
}

impl Parser {
    fn peek(&self) -> &Token {
        &self.toks[self.pos]
    }
    fn advance(&mut self) -> Token {
        let t = self.toks[self.pos].clone();
        if self.pos + 1 < self.toks.len() {
            self.pos += 1;
        }
        t
    }
    fn err<T>(&self, msg: &str) -> Result<T> {
        Err(Error::Invalid(format!(
            "parse error near {:?}: {msg}",
            self.peek()
        )))
    }
    fn eat_kw(&mut self, kw: &str) -> bool {
        if let Token::Ident(s) = self.peek() {
            if s.eq_ignore_ascii_case(kw) {
                self.advance();
                return true;
            }
        }
        false
    }
    fn expect_kw(&mut self, kw: &str) -> Result<()> {
        if self.eat_kw(kw) {
            Ok(())
        } else {
            self.err(&format!("expected keyword {kw}"))
        }
    }
    fn expect_ident(&mut self) -> Result<String> {
        match self.advance() {
            Token::Ident(mut s) => {
                // tolerate `db.table` / `db.schema.table`: keep the base name
                while self.peek() == &Token::Op(".".into()) {
                    self.advance();
                    if let Token::Ident(next) = self.advance() {
                        s = next;
                    } else {
                        break;
                    }
                }
                Ok(s)
            }
            _ => self.err("expected identifier"),
        }
    }

    /// Reads a (possibly `db.table`-qualified) table name, keeping the dotted
    /// form so the executor can resolve it against the current database.
    fn expect_table_name(&mut self) -> Result<String> {
        match self.advance() {
            Token::Ident(mut s) => {
                while self.peek() == &Token::Op(".".into()) {
                    self.advance();
                    if let Token::Ident(next) = self.advance() {
                        s.push('.');
                        s.push_str(&next);
                    } else {
                        break;
                    }
                }
                Ok(s)
            }
            _ => self.err("expected table name"),
        }
    }
    fn expect(&mut self, t: Token) -> Result<()> {
        if self.peek() == &t {
            self.advance();
            Ok(())
        } else {
            self.err(&format!("expected {t:?}"))
        }
    }

    /// Consumes `[IF] [NOT] EXISTS` if present.
    fn eat_if_exists(&mut self) -> bool {
        if self.eat_kw("IF") {
            let _ = self.eat_kw("NOT");
            let _ = self.eat_kw("EXISTS");
            true
        } else {
            false
        }
    }

    fn statement(&mut self) -> Result<Statement> {
        let kw = match self.peek() {
            Token::Ident(s) => s.to_ascii_uppercase(),
            _ => return self.err("expected statement"),
        };
        match kw.as_str() {
            "CREATE" => {
                self.advance();
                if self.eat_kw("SNAPSHOT") {
                    let name = self.expect_ident()?;
                    self.expect_kw("ON")?;
                    self.expect_kw("TABLE")?;
                    let table = self.expect_table_name()?;
                    return Ok(Statement::CreateSnapshot { name, table });
                }
                if self.eat_kw("DATABASE") {
                    // CREATE DATABASE [IF NOT EXISTS] name
                    let ine = self.eat_kw("IF");
                    let _ = self.eat_kw("NOT");
                    let _ = self.eat_kw("EXISTS");
                    let name = self.expect_ident()?;
                    Ok(Statement::CreateDatabase {
                        name,
                        if_not_exists: ine,
                    })
                } else if self.eat_kw("INDEX") {
                    let _ = self.eat_kw("IF");
                    let _ = self.eat_kw("NOT");
                    let _ = self.eat_kw("EXISTS");
                    let name = self.expect_ident()?;
                    self.expect_kw("ON")?;
                    let table = self.expect_table_name()?;
                    self.expect(Token::LParen)?;
                    // a plain column, or a functional-index expression
                    let mut column = String::new();
                    let mut depth = 0i32;
                    while !matches!(self.peek(), Token::Eof) {
                        match self.peek().clone() {
                            Token::RParen if depth == 0 => break,
                            Token::LParen => {
                                depth += 1;
                                self.advance();
                            }
                            Token::RParen => {
                                depth -= 1;
                                self.advance();
                            }
                            Token::Ident(n)
                                if column.is_empty()
                                    && !matches!(
                                        self.toks.get(self.pos + 1),
                                        Some(Token::LParen)
                                    ) =>
                            {
                                column = n;
                                self.advance();
                            }
                            _ => {
                                if depth == 0 && self.peek() == &Token::Comma {
                                    break;
                                }
                                self.advance();
                            }
                        }
                    }
                    // tolerate additional index columns
                    while !matches!(self.peek(), Token::RParen | Token::Eof) {
                        self.advance();
                    }
                    self.expect(Token::RParen)?;
                    // tolerate trailing `USING …` / `WITH (…)`
                    while !matches!(self.peek(), Token::Semicolon | Token::Eof) {
                        self.advance();
                    }
                    Ok(Statement::CreateIndex {
                        name,
                        table,
                        column,
                    })
                } else {
                    let _ = self.eat_kw("IF");
                    let _ = self.eat_kw("NOT");
                    let _ = self.eat_kw("EXISTS");
                    self.expect_kw("TABLE")?;
                    let name = self.expect_table_name()?;
                    self.expect(Token::LParen)?;
                    let columns = self.column_defs()?;
                    self.expect(Token::RParen)?;
                    // tolerate a trailing `PROPERTIES (…)` / `WITH (…)` clause
                    while !matches!(self.peek(), Token::Semicolon | Token::Eof) {
                        if self.peek() == &Token::LParen {
                            let mut depth = 0i32;
                            loop {
                                match self.advance() {
                                    Token::LParen => depth += 1,
                                    Token::RParen => {
                                        depth -= 1;
                                        if depth == 0 {
                                            break;
                                        }
                                    }
                                    Token::Eof | Token::Semicolon => break,
                                    _ => {}
                                }
                            }
                        } else {
                            self.advance();
                        }
                    }
                    Ok(Statement::CreateTable { name, columns })
                }
            }
            "DROP" => {
                self.advance();
                if self.eat_kw("DATABASE") {
                    let if_exists = self.eat_if_exists();
                    let name = self.expect_ident()?;
                    Ok(Statement::DropDatabase { name, if_exists })
                } else if self.eat_kw("SNAPSHOT") {
                    self.eat_if_exists();
                    let name = self.expect_ident()?;
                    Ok(Statement::DropSnapshot { name })
                } else if self.eat_kw("INDEX") {
                    self.eat_if_exists();
                    let name = self.expect_ident()?;
                    // tolerate `ON <table>`
                    if self.eat_kw("ON") {
                        let _ = self.expect_ident();
                    }
                    Ok(Statement::DropIndex { name })
                } else {
                    self.expect_kw("TABLE")?;
                    self.eat_if_exists();
                    let name = self.expect_table_name()?;
                    Ok(Statement::DropTable { name })
                }
            }
            "SET" if matches!(self.toks.get(self.pos + 1), Some(Token::Ident(k)) if k.eq_ignore_ascii_case("CONFIG")) =>
            {
                self.advance(); // SET
                self.advance(); // CONFIG
                let key = self.expect_ident()?;
                let value = match self.advance() {
                    Token::Ident(v) => v,
                    Token::Int(i) => i.to_string(),
                    Token::Float(f) => f.to_string(),
                    Token::Str(v) => v,
                    _ => String::new(),
                };
                while !matches!(self.peek(), Token::Semicolon | Token::Eof) {
                    self.advance();
                }
                Ok(Statement::SetConfig { key, value })
            }
            "RESTORE" => {
                self.advance();
                self.expect_kw("TABLE")?;
                self.expect_kw("SNAPSHOT")?;
                let name = self.expect_ident()?;
                Ok(Statement::RestoreSnapshot { name })
            }
            "USE" => {
                self.advance();
                let name = self.expect_ident()?;
                Ok(Statement::Use { name })
            }
            "SET" | "SNAPSHOT" | "DUMP" | "PRAGMA" | "COMPACT" | "OPTIMIZE" | "FLUSH" | "CHECK" => {
                self.advance();
                let mut parts: Vec<String> = Vec::new();
                while !matches!(self.peek(), Token::Semicolon | Token::Eof) {
                    parts.push(match self.advance() {
                        Token::Ident(s) => s,
                        Token::Int(i) => i.to_string(),
                        Token::Float(f) => f.to_string(),
                        Token::Str(s) => s,
                        Token::Star => "*".to_string(),
                        Token::Comma => ",".to_string(),
                        Token::LParen => "(".to_string(),
                        Token::RParen => ")".to_string(),
                        Token::Op(o) => o,
                        _ => String::new(),
                    });
                }
                Ok(Statement::Utility {
                    text: parts.join(" "),
                })
            }
            "INSERT" => {
                self.advance();
                self.expect_kw("INTO")?;
                let table = self.expect_table_name()?;
                // an explicit column list: INSERT INTO t (a, b) VALUES …
                let columns = if self.peek() == &Token::LParen {
                    self.advance();
                    let mut names = Vec::new();
                    while !matches!(self.peek(), Token::RParen | Token::Eof) {
                        if let Token::Ident(n) = self.peek().clone() {
                            names.push(n);
                        }
                        self.advance();
                        if self.peek() == &Token::Comma {
                            self.advance();
                        }
                    }
                    self.expect(Token::RParen)?;
                    Some(names)
                } else {
                    None
                };
                self.expect_kw("VALUES")?;
                let mut rows = Vec::new();
                loop {
                    self.expect(Token::LParen)?;
                    rows.push(self.literals()?);
                    self.expect(Token::RParen)?;
                    if self.peek() == &Token::Comma {
                        self.advance();
                    } else {
                        break;
                    }
                }
                Ok(Statement::Insert {
                    table,
                    columns,
                    rows,
                })
            }
            "ALTER" => {
                self.advance();
                if self.eat_kw("TABLE") {
                    let table = self.expect_table_name()?;
                    // tolerate `ADD COLUMN (...)`; other ALTER forms are no-ops.
                    if self.eat_kw("ADD") {
                        let _ = self.eat_kw("COLUMN");
                        let mut columns = Vec::new();
                        if self.peek() == &Token::LParen {
                            self.advance();
                            columns = self.column_defs()?;
                            self.expect(Token::RParen)?;
                        } else if matches!(self.peek(), Token::Ident(_)) {
                            columns = self.column_defs()?;
                        }
                        Ok(Statement::AlterTableAdd { table, columns })
                    } else if self.eat_kw("DROP") {
                        let _ = self.eat_kw("COLUMN");
                        let column = if self.peek() == &Token::LParen {
                            self.advance();
                            let c = self.expect_ident()?;
                            while !matches!(self.peek(), Token::RParen | Token::Eof) {
                                self.advance();
                            }
                            self.expect(Token::RParen)?;
                            c
                        } else {
                            self.expect_ident()?
                        };
                        Ok(Statement::AlterTableDropColumn { table, column })
                    } else {
                        while !matches!(self.peek(), Token::Semicolon | Token::Eof) {
                            self.advance();
                        }
                        Ok(Statement::Utility {
                            text: format!("ALTER TABLE {table}"),
                        })
                    }
                } else {
                    // `ALTER <index> ON <table> …` and other forms: tolerated.
                    while !matches!(self.peek(), Token::Semicolon | Token::Eof) {
                        self.advance();
                    }
                    Ok(Statement::Utility {
                        text: "ALTER".into(),
                    })
                }
            }
            "SELECT" => {
                self.advance();
                let distinct = self.eat_kw("DISTINCT");
                let items = self.select_items()?;
                let from = if self.eat_kw("FROM") {
                    Some(self.expect_table_name()?)
                } else {
                    None
                };
                let join = if self.eat_kw("JOIN") {
                    let table = self.expect_table_name()?;
                    self.expect_kw("ON")?;
                    let left = self.expect_ident()?;
                    self.expect(Token::Op("=".into()))?;
                    let right = self.expect_ident()?;
                    Some(JoinClause { table, left, right })
                } else {
                    None
                };
                let search = if self.eat_kw("SEARCH") {
                    Some(Box::new(self.search_clause()?))
                } else {
                    None
                };
                let where_pred = if self.eat_kw("WHERE") {
                    Some(self.predicate()?)
                } else {
                    None
                };
                let group_by = if self.eat_kw("GROUP") {
                    self.expect_kw("BY")?;
                    let mut keys = vec![self.expect_ident()?];
                    while self.peek() == &Token::Comma {
                        self.advance();
                        // tolerate expression keys: take the leading identifier
                        if matches!(self.peek(), Token::Ident(_)) {
                            keys.push(self.expect_ident()?);
                            while !matches!(
                                self.peek(),
                                Token::Comma | Token::Ident(_) | Token::Semicolon | Token::Eof
                            ) {
                                self.advance();
                            }
                        }
                    }
                    keys
                } else {
                    Vec::new()
                };
                let having = if self.eat_kw("HAVING") {
                    Some(Box::new(self.predicate()?))
                } else {
                    None
                };
                let order_by = if self.eat_kw("ORDER") {
                    self.expect_kw("BY")?;
                    let column = self.expect_ident()?;
                    let asc = if self.eat_kw("DESC") {
                        false
                    } else {
                        let _ = self.eat_kw("ASC");
                        true
                    };
                    // tolerate an expression key (`ORDER BY c2 + c3 DESC`)
                    while !matches!(
                        self.peek(),
                        Token::Comma | Token::Semicolon | Token::Eof | Token::Ident(_)
                    ) || matches!(self.peek(), Token::Ident(k) if !k.eq_ignore_ascii_case("LIMIT"))
                    {
                        if matches!(self.peek(), Token::Comma | Token::Semicolon | Token::Eof)
                            || matches!(self.peek(), Token::Ident(k) if k.eq_ignore_ascii_case("LIMIT"))
                        {
                            break;
                        }
                        self.advance();
                        let _ = self.eat_kw("ASC");
                        let _ = self.eat_kw("DESC");
                    }
                    // tolerate extra ORDER BY keys
                    while self.peek() == &Token::Comma {
                        self.advance();
                        if matches!(self.peek(), Token::Ident(_)) {
                            self.advance();
                            let _ = self.eat_kw("DESC");
                            let _ = self.eat_kw("ASC");
                        }
                    }
                    Some(OrderKey { column, asc })
                } else {
                    None
                };
                let mut limit = None;
                let mut offset = None;
                if self.eat_kw("LIMIT") {
                    match self.advance() {
                        Token::Int(n) if n >= 0 => limit = Some(n as u64),
                        _ => return self.err("expected non-negative integer after LIMIT"),
                    }
                    if self.eat_kw("OFFSET") {
                        if let Token::Int(n) = self.advance() {
                            if n >= 0 {
                                offset = Some(n as u64);
                            }
                        }
                    }
                } else if self.eat_kw("OFFSET") {
                    if let Token::Int(n) = self.advance() {
                        if n >= 0 {
                            offset = Some(n as u64);
                        }
                    }
                    if self.eat_kw("LIMIT") {
                        if let Token::Int(n) = self.advance() {
                            if n >= 0 {
                                limit = Some(n as u64);
                            }
                        }
                    }
                }
                Ok(Statement::Select {
                    items,
                    from,
                    join,
                    where_pred,
                    group_by,
                    having,
                    order_by,
                    limit,
                    offset,
                    search,
                    distinct,
                })
            }
            "DELETE" => {
                self.advance();
                self.expect_kw("FROM")?;
                let table = self.expect_table_name()?;
                let where_pred = if self.eat_kw("WHERE") {
                    Some(self.predicate()?)
                } else {
                    None
                };
                Ok(Statement::Delete { table, where_pred })
            }
            "UPDATE" => {
                self.advance();
                let table = self.expect_table_name()?;
                self.expect_kw("SET")?;
                let mut assignments = Vec::new();
                loop {
                    let col = self.expect_ident()?;
                    self.expect(Token::Op("=".into()))?;
                    let val = self.expr()?;
                    assignments.push((col, val));
                    if self.peek() == &Token::Comma {
                        self.advance();
                    } else {
                        break;
                    }
                }
                let where_pred = if self.eat_kw("WHERE") {
                    Some(self.predicate()?)
                } else {
                    None
                };
                Ok(Statement::Update {
                    table,
                    assignments,
                    where_pred,
                })
            }
            "SHOW" => {
                self.advance();
                // SHOW TABLE <t> COLUMNS
                let is_table = self.eat_kw("TABLE");
                if is_table && matches!(self.peek(), Token::Ident(_)) {
                    if let Ok(t) = self.expect_table_name() {
                        if self.eat_kw("COLUMNS") {
                            return Ok(Statement::ShowTableColumns { table: t });
                        }
                    }
                }
                // SHOW GLOBAL VARIABLE <key>
                let is_global = self.eat_kw("GLOBAL");
                if is_global && self.eat_kw("VARIABLE") {
                    if let Token::Ident(k) = self.peek().clone() {
                        self.advance();
                        return Ok(Statement::ShowVariable { key: k });
                    }
                }
                let mut parts: Vec<String> = Vec::new();
                while !matches!(self.peek(), Token::Semicolon | Token::Eof) {
                    parts.push(match self.advance() {
                        Token::Ident(s) => s,
                        Token::Int(i) => i.to_string(),
                        Token::Float(f) => f.to_string(),
                        Token::Str(s) => s,
                        Token::Star => "*".to_string(),
                        Token::Comma => ",".to_string(),
                        Token::LParen => "(".to_string(),
                        Token::RParen => ")".to_string(),
                        Token::Op(o) => o,
                        _ => String::new(),
                    });
                }
                Ok(Statement::Show {
                    text: parts.join(" "),
                })
            }
            "EXPLAIN" => {
                self.advance();
                let inner = self.statement()?;
                Ok(Statement::Explain {
                    inner: Box::new(inner),
                })
            }
            "COPY" => {
                self.advance();
                let table = self.expect_table_name()?;
                let from = if self.eat_kw("FROM") {
                    true
                } else {
                    self.expect_kw("TO")?;
                    false
                };
                let path = match self.advance() {
                    Token::Str(s) => s,
                    _ => return self.err("expected string path in COPY"),
                };
                let mut opts = Vec::new();
                if self.eat_kw("WITH") {
                    self.expect(Token::LParen)?;
                    while !matches!(self.peek(), Token::RParen | Token::Eof) {
                        if let Token::Ident(k) = self.peek().clone() {
                            self.advance();
                            if self.peek() == &Token::Op("=".into()) {
                                self.advance();
                            }
                            let v = self.copy_option_value();
                            opts.push((k.to_ascii_uppercase(), v));
                        } else {
                            self.advance();
                        }
                        if self.peek() == &Token::Comma {
                            self.advance();
                        } else {
                            break;
                        }
                    }
                    self.expect(Token::RParen)?;
                }
                // tolerate a trailing option list without WITH
                while !matches!(self.peek(), Token::Semicolon | Token::Eof) {
                    self.advance();
                }
                Ok(Statement::Copy {
                    table,
                    from,
                    path,
                    opts,
                })
            }
            _ => self.err("unsupported statement"),
        }
    }

    /// Parses a boolean `WHERE` predicate (`OR` > `AND` > `NOT` > comparison).
    fn predicate(&mut self) -> Result<Predicate> {
        self.or_predicate()
    }

    fn or_predicate(&mut self) -> Result<Predicate> {
        let mut p = self.and_predicate()?;
        while self.eat_kw("OR") {
            let r = self.and_predicate()?;
            p = Predicate::Bool(Box::new(p), BoolOp::Or, Box::new(r));
        }
        Ok(p)
    }

    fn and_predicate(&mut self) -> Result<Predicate> {
        let mut p = self.not_predicate()?;
        while self.eat_kw("AND") {
            let r = self.not_predicate()?;
            p = Predicate::Bool(Box::new(p), BoolOp::And, Box::new(r));
        }
        Ok(p)
    }

    fn not_predicate(&mut self) -> Result<Predicate> {
        if self.eat_kw("NOT") {
            return Ok(Predicate::Not(Box::new(self.not_predicate()?)));
        }
        if self.peek() == &Token::LParen {
            let save = self.pos;
            self.advance();
            if let Ok(p) = self.or_predicate() {
                if self.peek() == &Token::RParen {
                    self.advance();
                    return Ok(p);
                }
            }
            self.pos = save;
        }
        self.comparison()
    }

    fn comparison(&mut self) -> Result<Predicate> {
        let left = self.expr()?;
        if self.eat_kw("IS") {
            let neg = self.eat_kw("NOT");
            self.expect_kw("NULL")?;
            return Ok(Predicate::IsNull(left, neg));
        }
        // `[NOT] LIKE <pattern> [ESCAPE '<c>']`
        let like_neg = if matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("NOT")) {
            let save = self.pos;
            self.advance();
            if matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("LIKE")) {
                true
            } else {
                self.pos = save;
                false
            }
        } else {
            false
        };
        let is_like = matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("LIKE"));
        if is_like {
            self.advance();
            let pat = self.expr()?;
            let mut escape = None;
            if self.eat_kw("ESCAPE") {
                if let Token::Str(c) = self.advance() {
                    escape = c.chars().next();
                }
            }
            return Ok(Predicate::Like(left, pat, escape, like_neg));
        }
        let neg_in = if matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("NOT")) {
            let save = self.pos;
            self.advance();
            if !matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("IN")) {
                self.pos = save;
                false
            } else {
                true
            }
        } else {
            false
        };
        if matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("IN")) {
            self.advance();
            self.expect(Token::LParen)?;
            let mut values = Vec::new();
            while !matches!(self.peek(), Token::RParen | Token::Eof) {
                values.push(self.expr()?);
                if self.peek() == &Token::Comma {
                    self.advance();
                } else {
                    break;
                }
            }
            self.expect(Token::RParen)?;
            return Ok(Predicate::In(left, values, neg_in));
        }
        if let Token::Op(s) = self.peek().clone() {
            if let Some(op) = cmp_op(&s) {
                self.advance();
                let right = self.expr()?;
                return Ok(Predicate::Cmp(left, op, right));
            }
        }
        Ok(Predicate::IsTrue(left))
    }

    /// Parses a scalar expression with `+ - * / %` precedence.
    fn expr(&mut self) -> Result<ExprAst> {
        let mut left = self.term()?;
        loop {
            let op = match self.peek() {
                Token::Op(s) if s == "+" => Some(BinOp::Add),
                Token::Op(s) if s == "-" => Some(BinOp::Sub),
                _ => None,
            };
            match op {
                Some(op) => {
                    self.advance();
                    let right = self.term()?;
                    left = ExprAst::Binary(Box::new(left), op, Box::new(right));
                }
                None => break,
            }
        }
        Ok(left)
    }

    fn term(&mut self) -> Result<ExprAst> {
        let mut left = self.atom()?;
        loop {
            let op = match self.peek() {
                Token::Star => Some(BinOp::Mul),
                Token::Op(s) if s == "/" => Some(BinOp::Div),
                Token::Op(s) if s == "%" => Some(BinOp::Mod),
                _ => None,
            };
            match op {
                Some(op) => {
                    self.advance();
                    let right = self.atom()?;
                    left = ExprAst::Binary(Box::new(left), op, Box::new(right));
                }
                None => break,
            }
        }
        Ok(left)
    }

    /// Parses a scalar atom: column ref, literal (incl. array), or function call.
    fn atom(&mut self) -> Result<ExprAst> {
        // unary minus / plus
        if let Token::Op(op) = self.peek().clone() {
            if op == "-" || op == "+" {
                self.advance();
                let inner = self.atom()?;
                if op == "-" {
                    return Ok(match inner {
                        ExprAst::Literal(Literal::Int(i)) => ExprAst::Literal(Literal::Int(-i)),
                        ExprAst::Literal(Literal::Float(f)) => ExprAst::Literal(Literal::Float(-f)),
                        other => ExprAst::Binary(
                            Box::new(ExprAst::Literal(Literal::Int(0))),
                            BinOp::Sub,
                            Box::new(other),
                        ),
                    });
                }
                return Ok(inner);
            }
        }
        match self.peek().clone() {
            Token::Int(_) | Token::Float(_) | Token::Str(_) => {
                Ok(ExprAst::Literal(self.literal()?))
            }
            Token::LParen => {
                self.advance();
                let inner = self.expr()?;
                self.expect(Token::RParen)?;
                Ok(inner)
            }
            Token::LBracket => Ok(ExprAst::Literal(self.array_literal()?)),
            Token::LBrace => Ok(ExprAst::Literal(self.brace_literal()?)),
            Token::Ident(s) => {
                if s.eq_ignore_ascii_case("NULL") {
                    self.advance();
                    return Ok(ExprAst::Literal(Literal::Null));
                }
                if s.eq_ignore_ascii_case("TRUE") || s.eq_ignore_ascii_case("FALSE") {
                    self.advance();
                    return Ok(ExprAst::Literal(Literal::Str(s.to_ascii_lowercase())));
                }
                // `CAST(expr AS TYPE)` / `TRY_CAST(...)`: encode the type name as a
                // string literal argument.
                if s.eq_ignore_ascii_case("CAST") || s.eq_ignore_ascii_case("TRY_CAST") {
                    self.advance();
                    self.expect(Token::LParen)?;
                    let inner = self.expr()?;
                    self.expect_kw("AS")?;
                    let ty = self.expect_ident()?;
                    // consume an optional balanced parameter list (e.g. `VARCHAR(10)`)
                    if self.peek() == &Token::LParen {
                        let mut depth = 0i32;
                        loop {
                            match self.advance() {
                                Token::LParen => depth += 1,
                                Token::RParen => {
                                    depth -= 1;
                                    if depth == 0 {
                                        break;
                                    }
                                }
                                Token::Eof | Token::Semicolon => break,
                                _ => {}
                            }
                        }
                    }
                    self.expect(Token::RParen)?;
                    return Ok(ExprAst::Func {
                        name: "CAST".to_string(),
                        args: vec![
                            inner,
                            ExprAst::Literal(Literal::Str(ty.to_ascii_uppercase())),
                        ],
                    });
                }
                // `INTERVAL <n> <UNIT>` (unquoted form), e.g. `INTERVAL -100 YEAR`.
                if s.eq_ignore_ascii_case("INTERVAL")
                    && !matches!(self.toks.get(self.pos + 1), Some(Token::Str(_)))
                {
                    self.advance();
                    let mut sign = 1i64;
                    if let Token::Op(op) = self.peek().clone() {
                        if op == "-" {
                            self.advance();
                            sign = -1;
                        } else if op == "+" {
                            self.advance();
                        }
                    }
                    let n = match self.advance() {
                        Token::Int(i) => i,
                        Token::Float(f) => f as i64,
                        _ => 0,
                    };
                    let unit = self.expect_ident()?.to_ascii_uppercase();
                    return Ok(ExprAst::Func {
                        name: "INTERVAL".to_string(),
                        args: vec![ExprAst::Literal(Literal::Str(format!(
                            "{} {}",
                            sign * n,
                            unit
                        )))],
                    });
                }
                // `EXTRACT(<field> FROM <expr>)`
                if s.eq_ignore_ascii_case("EXTRACT") {
                    self.advance();
                    self.expect(Token::LParen)?;
                    let field = self.expr()?;
                    self.expect_kw("FROM")?;
                    let src = self.expr()?;
                    self.expect(Token::RParen)?;
                    return Ok(ExprAst::Func {
                        name: "EXTRACT".to_string(),
                        args: vec![field, src],
                    });
                }
                // Typed date/time literals can also appear inside expressions.
                if matches!(
                    s.to_ascii_uppercase().as_str(),
                    "DATE" | "TIME" | "TIMESTAMP" | "DATETIME" | "INTERVAL" | "JSON"
                ) && matches!(self.toks.get(self.pos + 1), Some(Token::Str(_)))
                {
                    self.advance();
                    let kind = s.to_ascii_uppercase();
                    let inner = self.literal()?;
                    return Ok(ExprAst::Literal(match inner {
                        Literal::Str(v) => Literal::Str(normalize_datetime(&kind, &v)),
                        other => other,
                    }));
                }
                if matches!(self.toks.get(self.pos + 1), Some(Token::LParen)) {
                    self.advance();
                    self.expect(Token::LParen)?;
                    let mut args = Vec::new();
                    if self.peek() != &Token::RParen {
                        loop {
                            args.push(self.expr()?);
                            if self.peek() == &Token::Comma {
                                self.advance();
                            } else {
                                break;
                            }
                        }
                    }
                    self.expect(Token::RParen)?;
                    Ok(ExprAst::Func { name: s, args })
                } else {
                    self.advance();
                    Ok(ExprAst::Column(s))
                }
            }
            _ => self.err("expected expression"),
        }
    }

    /// Parses `[a, b, …]`, `[a b c]` or a sparse `[i:v, …]`. Elements are stored as
    /// their textual form (re-joined without spaces) so rendering round-trips.
    fn array_literal(&mut self) -> Result<Literal> {
        self.expect(Token::LBracket)?;
        let mut items: Vec<String> = Vec::new();
        loop {
            if self.peek() == &Token::RBracket {
                break;
            }
            let first = self.atom_text()?;
            if self.peek() == &Token::Colon {
                self.advance();
                let second = self.atom_text()?;
                items.push(format!("{first}:{second}"));
            } else {
                items.push(first);
            }
            if self.peek() == &Token::Comma {
                self.advance();
            } else if self.peek() == &Token::RBracket {
                break;
            }
            // space-separated elements continue the loop
        }
        self.expect(Token::RBracket)?;
        Ok(Literal::Array(items))
    }

    fn atom_text(&mut self) -> Result<String> {
        if self.peek() == &Token::LBracket {
            return Ok(self.array_literal()?.to_slt_string());
        }
        if self.peek() == &Token::LBrace {
            return Ok(self.brace_literal()?.to_slt_string());
        }
        match self.advance() {
            Token::Int(i) => Ok(i.to_string()),
            Token::Float(f) => Ok(fmt_float(f)),
            Token::Str(s) => Ok(s),
            Token::Ident(s) => Ok(s),
            other => self.err(&format!("bad array element {other:?}")),
        }
    }

    /// Parses a brace set literal `{0, 1, 2}`.
    fn brace_literal(&mut self) -> Result<Literal> {
        self.expect(Token::LBrace)?;
        let mut items: Vec<String> = Vec::new();
        while self.peek() != &Token::RBrace && self.peek() != &Token::Eof {
            let t = self.atom_text()?;
            items.push(t);
            if self.peek() == &Token::Comma {
                self.advance();
            } else {
                break;
            }
        }
        self.expect(Token::RBrace)?;
        Ok(Literal::Array(items))
    }

    /// Parses `SEARCH MATCH <KIND> (col, args…) [, MATCH …, FUSION(…)] [WITH (…)…]`.
    fn search_clause(&mut self) -> Result<SearchClause> {
        let mut out = SearchClause::default();
        loop {
            self.expect_kw("MATCH")?;
            let kind = self.expect_ident()?.to_ascii_uppercase();
            self.expect(Token::LParen)?;
            let column = match self.expr()? {
                ExprAst::Column(c) => c,
                ExprAst::Literal(Literal::Str(s)) => s,
                _ => return self.err("expected match column"),
            };
            let mut args = Vec::new();
            let mut filter = None;
            while self.peek() == &Token::Comma {
                self.advance();
                if matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("WHERE")) {
                    self.advance();
                    filter = Some(self.predicate()?);
                    break;
                }
                args.push(self.expr()?);
            }
            self.expect(Token::RParen)?;
            out.matches.push(MatchClause {
                kind: kind.clone(),
                column: column.clone(),
                args,
                filter,
            });
            if self.eat_kw("WITH") {
                out.with_opts = Some(self.paren_list_text()?);
            }
            if self.eat_kw("USING") {
                self.expect_kw("INDEX")?;
                out.using_index = Some(self.paren_list_text()?);
            }
            let next_is_match = self.peek() == &Token::Comma
                && matches!(self.toks.get(self.pos + 1), Some(Token::Ident(s)) if s.eq_ignore_ascii_case("MATCH"));
            let next_is_fusion =
                matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("FUSION"));
            if next_is_match {
                self.advance(); // comma
                continue;
            }
            if next_is_fusion
                || (self.peek() == &Token::Comma
                    && {
                        matches!(self.toks.get(self.pos + 1), Some(Token::Ident(s)) if s.eq_ignore_ascii_case("FUSION"))
                    })
            {
                if self.peek() == &Token::Comma {
                    self.advance();
                }
                self.eat_kw("FUSION");
                self.expect(Token::LParen)?;
                out.fusion = Some(match self.expr()? {
                    ExprAst::Literal(Literal::Str(s)) => s,
                    ExprAst::Column(c) => c,
                    _ => String::new(),
                });
                if self.peek() == &Token::Comma {
                    self.advance();
                    out.fusion_params = Some(match self.expr()? {
                        ExprAst::Literal(Literal::Str(s)) => s,
                        ExprAst::Column(c) => c,
                        _ => String::new(),
                    });
                }
                self.expect(Token::RParen)?;
            }
            break;
        }
        Ok(out)
    }

    /// Reads a single COPY option value (string / identifier / number).
    fn copy_option_value(&mut self) -> String {
        match self.advance() {
            Token::Str(s) => s,
            Token::Ident(s) => s,
            Token::Int(i) => i.to_string(),
            Token::Float(f) => fmt_float(f),
            _ => String::new(),
        }
    }

    /// Consumes a parenthesised token run and returns its text joined by spaces.
    fn paren_list_text(&mut self) -> Result<String> {
        self.expect(Token::LParen)?;
        let mut depth = 1i32;
        let mut parts: Vec<String> = Vec::new();
        while depth > 0 {
            match self.advance() {
                Token::LParen => {
                    depth += 1;
                    parts.push("(".into());
                }
                Token::RParen => {
                    depth -= 1;
                    if depth > 0 {
                        parts.push(")".into());
                    }
                }
                Token::Comma => parts.push(",".into()),
                Token::Colon => parts.push(":".into()),
                Token::Star => parts.push("*".into()),
                Token::LBracket => parts.push("[".into()),
                Token::RBracket => parts.push("]".into()),
                Token::LBrace => parts.push("{".into()),
                Token::RBrace => parts.push("}".into()),
                Token::Ident(s) => parts.push(s),
                Token::Int(i) => parts.push(i.to_string()),
                Token::Float(f) => parts.push(fmt_float(f)),
                Token::Str(s) => parts.push(s),
                Token::Op(o) => parts.push(o),
                Token::Eof => break,
                Token::Semicolon => break,
            }
        }
        Ok(parts.join(" "))
    }

    fn column_defs(&mut self) -> Result<Vec<ColumnDef>> {
        let mut cols = Vec::new();
        loop {
            let name = self.expect_ident()?;
            let (ty, type_text) = self.parse_type()?;
            let mut default = None;
            let mut not_null = false;
            // Tolerate column constraints (NOT NULL, PRIMARY KEY, DEFAULT …, COMMENT …).
            loop {
                if self.eat_kw("NOT") {
                    let _ = self.eat_kw("NULL");
                    not_null = true;
                    continue;
                }
                if self.eat_kw("DEFAULT") {
                    default = Some(self.literal()?);
                    continue;
                }
                match self.peek() {
                    Token::Comma | Token::RParen | Token::Eof => break,
                    // consume a balanced group (e.g. `WITH (sort)`, `COMMENT '…'`)
                    Token::LParen => {
                        let mut depth = 0i32;
                        loop {
                            match self.advance() {
                                Token::LParen => depth += 1,
                                Token::RParen => {
                                    depth -= 1;
                                    if depth == 0 {
                                        break;
                                    }
                                }
                                Token::Eof | Token::Semicolon => break,
                                _ => {}
                            }
                        }
                    }
                    _ => {
                        self.advance();
                    }
                }
            }
            cols.push(ColumnDef {
                name,
                ty,
                type_text,
                default,
                not_null,
            });
            if self.peek() == &Token::Comma {
                self.advance();
            } else {
                break;
            }
        }
        Ok(cols)
    }

    fn parse_type(&mut self) -> Result<(LogicalType, String)> {
        let t = self.expect_ident()?.to_ascii_uppercase();
        let base = match t.as_str() {
            "BOOL" | "BOOLEAN" => LogicalType::Boolean,
            "TINYINT" => LogicalType::TinyInt,
            "SMALLINT" => LogicalType::SmallInt,
            "INT" | "INTEGER" => LogicalType::Integer,
            "BIGINT" => LogicalType::BigInt,
            "FLOAT" | "REAL" => LogicalType::Float,
            "DOUBLE" => LogicalType::Double,
            "VARCHAR" | "TEXT" | "STRING" => LogicalType::Varchar,
            _ => LogicalType::Invalid, // tolerate unknown/extra types
        };
        let mut text = t.clone();
        // Consume a balanced parameter list, e.g. `TENSOR(FLOAT,16)` or `VARCHAR(20)`.
        if self.peek() == &Token::LParen {
            let mut depth = 0i32;
            let mut inner = String::new();
            loop {
                match self.advance() {
                    Token::LParen => {
                        depth += 1;
                        if depth > 1 {
                            inner.push('(');
                        }
                    }
                    Token::RParen => {
                        depth -= 1;
                        if depth == 0 {
                            break;
                        }
                        inner.push(')');
                    }
                    Token::Eof | Token::Semicolon => break,
                    Token::Comma => inner.push(','),
                    Token::Ident(s) => inner.push_str(&s.to_ascii_uppercase()),
                    Token::Int(i) => inner.push_str(&i.to_string()),
                    Token::Star => inner.push('*'),
                    Token::LBracket => inner.push('['),
                    Token::RBracket => inner.push(']'),
                    Token::Str(s) => inner.push_str(&s),
                    Token::Op(o) => inner.push_str(&o),
                    Token::Float(f) => inner.push_str(&format!("{f}")),
                    _ => {}
                }
            }
            text = format!("{t}({inner})");
        }
        Ok((base, text))
    }

    fn literals(&mut self) -> Result<Vec<Literal>> {
        let mut out = Vec::new();
        loop {
            out.push(self.literal()?);
            if self.peek() == &Token::Comma {
                self.advance();
            } else {
                break;
            }
        }
        Ok(out)
    }

    fn literal(&mut self) -> Result<Literal> {
        match self.peek().clone() {
            Token::Int(i) => {
                self.advance();
                Ok(Literal::Int(i))
            }
            Token::Float(f) => {
                self.advance();
                Ok(Literal::Float(f))
            }
            Token::Str(s) => {
                self.advance();
                Ok(Literal::Str(s))
            }
            Token::LBracket => self.array_literal(),
            Token::LBrace => self.brace_literal(),
            Token::Ident(s) if s.eq_ignore_ascii_case("NULL") => {
                self.advance();
                Ok(Literal::Null)
            }
            Token::Ident(s)
                if s.eq_ignore_ascii_case("TRUE") || s.eq_ignore_ascii_case("FALSE") =>
            {
                self.advance();
                Ok(Literal::Str(s.to_ascii_lowercase()))
            }
            // Typed literals: `DATE '…'`, `TIMESTAMP '…'`, `INTERVAL '…'`, …
            Token::Ident(s)
                if matches!(
                    s.to_ascii_uppercase().as_str(),
                    "DATE" | "TIME" | "TIMESTAMP" | "DATETIME" | "INTERVAL" | "JSON"
                ) =>
            {
                self.advance();
                let kind = s.to_ascii_uppercase();
                let inner = self.literal()?;
                match inner {
                    Literal::Str(v) => Ok(Literal::Str(normalize_datetime(&kind, &v))),
                    other => Ok(other),
                }
            }
            _ => self.err("expected literal"),
        }
    }

    fn select_items(&mut self) -> Result<Vec<SelectItem>> {
        let mut items = Vec::new();
        loop {
            let item = match self.peek() {
                Token::Star => {
                    self.advance();
                    SelectItem::Star
                }
                Token::LParen => SelectItem::Expr(self.expr()?),
                Token::Ident(n) if n.eq_ignore_ascii_case("NOT") => {
                    self.advance();
                    match select_item_to_expr(&self.select_items().map(|mut v| {
                        if v.is_empty() {
                            SelectItem::Literal(Literal::Null)
                        } else {
                            v.remove(0)
                        }
                    })?) {
                        Some(inner) => SelectItem::Expr(ExprAst::Not(Box::new(inner))),
                        None => SelectItem::Literal(Literal::Null),
                    }
                }
                Token::Int(_) | Token::Float(_) | Token::Str(_) => {
                    SelectItem::Literal(self.literal()?)
                }
                Token::Ident(s) if s.eq_ignore_ascii_case("NULL") => {
                    SelectItem::Literal(self.literal()?)
                }
                Token::Ident(_) => {
                    if matches!(self.toks.get(self.pos + 1), Some(Token::LParen)) {
                        let name = match self.peek() {
                            Token::Ident(s) => s.to_ascii_uppercase(),
                            _ => unreachable!(),
                        };
                        if is_agg_name(&name) {
                            let n = self.expect_ident()?;
                            SelectItem::Aggregate(self.aggregate_name(n)?)
                        } else {
                            // let `atom` handle CAST and generic function calls
                            match self.atom()? {
                                ExprAst::Func { name, args } => SelectItem::Func { name, args },
                                other => SelectItem::Expr(other),
                            }
                        }
                    } else {
                        SelectItem::Column(self.expect_ident()?)
                    }
                }
                _ => return self.err("expected select item"),
            };
            // Absorb a trailing `IS [NOT] NULL` (projected as a boolean expression).
            let item = if matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("IS")) {
                match select_item_to_expr(&item) {
                    Some(inner) => {
                        self.advance(); // IS
                        let neg = self.eat_kw("NOT");
                        self.expect_kw("NULL")?;
                        SelectItem::Func {
                            name: if neg { "IS_NOT_NULL" } else { "IS_NULL" }.to_string(),
                            args: vec![inner],
                        }
                    }
                    None => item,
                }
            } else {
                item
            };
            // Absorb boolean connectives (`expr AND expr` / `expr OR expr`).
            let item = {
                let mut acc = item;
                loop {
                    let is_and =
                        matches!(self.peek(), Token::Ident(k) if k.eq_ignore_ascii_case("AND"));
                    let is_or =
                        matches!(self.peek(), Token::Ident(k) if k.eq_ignore_ascii_case("OR"));
                    if !is_and && !is_or {
                        break;
                    }
                    let left = match select_item_to_expr(&acc) {
                        Some(l) => l,
                        None => break,
                    };
                    self.advance();
                    let right_item = match self.select_items() {
                        Ok(mut v) if !v.is_empty() => v.remove(0),
                        _ => SelectItem::Literal(Literal::Null),
                    };
                    let right = match select_item_to_expr(&right_item) {
                        Some(r) => r,
                        None => ExprAst::Literal(Literal::Null),
                    };
                    acc = SelectItem::Expr(if is_and {
                        ExprAst::And(Box::new(left), Box::new(right))
                    } else {
                        ExprAst::Or(Box::new(left), Box::new(right))
                    });
                }
                acc
            };
            // Absorb a trailing comparison (projected as a boolean expression).
            let item = match self.peek().clone() {
                Token::Op(s) if cmp_op(&s).is_some() => match select_item_to_expr(&item) {
                    Some(left) => {
                        let op = cmp_op(&s).unwrap();
                        self.advance();
                        let right = self.expr()?;
                        SelectItem::Expr(ExprAst::Cmp(Box::new(left), op, Box::new(right)))
                    }
                    None => item,
                },
                _ => item,
            };
            // Absorb a trailing arithmetic expression (`MAX(c1)+SUM(c2)`).
            let needs_more = match self.peek() {
                Token::Star => true,
                Token::Op(s) => s == "+" || s == "-" || s == "/" || s == "%",
                _ => false,
            };
            let item = if needs_more {
                match select_item_to_expr(&item) {
                    Some(left) => SelectItem::Expr(self.continue_binary(left)?),
                    None => item,
                }
            } else {
                item
            };
            items.push(item);
            // tolerate an output alias: `<expr> AS name`
            if self.eat_kw("AS") {
                let _ = self.advance();
            }
            if self.peek() == &Token::Comma {
                self.advance();
            } else {
                break;
            }
        }
        Ok(items)
    }

    fn aggregate_name(&mut self, name: String) -> Result<SelectAgg> {
        let name = name.to_ascii_uppercase();
        let func =
            agg_func(&name).ok_or_else(|| Error::Invalid(format!("unknown aggregate {name}")))?;
        self.expect(Token::LParen)?;
        let distinct = self.eat_kw("DISTINCT");
        let mut args: Vec<String> = Vec::new();
        if self.peek() == &Token::Star {
            self.advance();
        } else {
            loop {
                match self.peek() {
                    Token::Ident(_) => args.push(self.expect_ident()?),
                    Token::RParen => break,
                    _ => {
                        self.advance();
                    }
                }
                if self.peek() == &Token::Comma {
                    self.advance();
                } else {
                    break;
                }
            }
        }
        // tolerate trailing tokens before the closing paren
        while !matches!(self.peek(), Token::RParen | Token::Eof) {
            self.advance();
        }
        self.expect(Token::RParen)?;
        Ok(SelectAgg {
            func,
            args,
            distinct,
        })
    }

    /// Continues a binary expression from an already-parsed left operand.
    fn continue_binary(&mut self, mut left: ExprAst) -> Result<ExprAst> {
        loop {
            let op = match self.peek() {
                Token::Star => Some(BinOp::Mul),
                Token::Op(s) if s == "/" => Some(BinOp::Div),
                Token::Op(s) if s == "%" => Some(BinOp::Mod),
                _ => None,
            };
            match op {
                Some(op) => {
                    self.advance();
                    let right = self.atom()?;
                    left = ExprAst::Binary(Box::new(left), op, Box::new(right));
                }
                None => break,
            }
        }
        loop {
            let op = match self.peek() {
                Token::Op(s) if s == "+" => Some(BinOp::Add),
                Token::Op(s) if s == "-" => Some(BinOp::Sub),
                _ => None,
            };
            match op {
                Some(op) => {
                    self.advance();
                    let right = self.term()?;
                    left = ExprAst::Binary(Box::new(left), op, Box::new(right));
                }
                None => break,
            }
        }
        Ok(left)
    }
}

/// Converts a select item into an expression so a binary tail can be parsed.
fn select_item_to_expr(item: &SelectItem) -> Option<ExprAst> {
    match item {
        SelectItem::Column(c) => Some(ExprAst::Column(c.clone())),
        SelectItem::Literal(l) => Some(ExprAst::Literal(l.clone())),
        SelectItem::Func { name, args } => Some(ExprAst::Func {
            name: name.clone(),
            args: args.clone(),
        }),
        SelectItem::Aggregate(a) => Some(ExprAst::Func {
            name: a.func.as_str().to_ascii_uppercase(),
            args: a.args.iter().cloned().map(ExprAst::Column).collect(),
        }),
        _ => None,
    }
}

/// Normalises a typed date/time literal to Infinity's zero-padded text form.
fn normalize_datetime(kind: &str, raw: &str) -> String {
    if kind == "INTERVAL" {
        return raw.trim().to_string();
    }
    let s = raw.trim();
    let (date_part, time_part) = if let Some((a, b)) = s.split_once(' ') {
        (Some(a.to_string()), Some(b.to_string()))
    } else if s.contains(':') {
        (None, Some(s.to_string()))
    } else {
        (Some(s.to_string()), None)
    };
    let date_norm = date_part.map(|d| {
        let parts: Vec<&str> = d.split('-').collect();
        if parts.len() == 3 {
            let y: i64 = parts[0].parse().unwrap_or(0);
            let m: i64 = parts[1].parse().unwrap_or(0);
            let dd: i64 = parts[2].parse().unwrap_or(0);
            format!("{y:04}-{m:02}-{dd:02}")
        } else {
            d
        }
    });
    let time_norm = time_part.map(|t| {
        let parts: Vec<&str> = t.split(':').collect();
        let get = |i: usize| -> i64 {
            parts
                .get(i)
                .map(|v| v.split('.').next().unwrap_or(v).trim().parse().unwrap_or(0))
                .unwrap_or(0)
        };
        format!("{:02}:{:02}:{:02}", get(0), get(1), get(2))
    });
    match kind {
        "DATE" => date_norm.unwrap_or_default(),
        "TIME" => time_norm.unwrap_or_else(|| "00:00:00".to_string()),
        _ => format!(
            "{} {}",
            date_norm.unwrap_or_default(),
            time_norm.unwrap_or_else(|| "00:00:00".to_string())
        ),
    }
}

fn cmp_op(s: &str) -> Option<CmpOp> {
    Some(match s {
        "=" => CmpOp::Eq,
        "!=" | "<>" => CmpOp::Ne,
        "<" => CmpOp::Lt,
        "<=" => CmpOp::Le,
        ">" => CmpOp::Gt,
        ">=" => CmpOp::Ge,
        _ => return None,
    })
}

fn is_agg_name(name: &str) -> bool {
    agg_func(name).is_some()
}

fn agg_func(name: &str) -> Option<AggFunc> {
    Some(match name {
        "COUNT" => AggFunc::Count,
        "SUM" => AggFunc::Sum,
        "AVG" => AggFunc::Avg,
        "MIN" => AggFunc::Min,
        "MAX" => AggFunc::Max,
        "FIRST" => AggFunc::First,
        "LAST" => AggFunc::Last,
        _ => return None,
    })
}

/// Renders a float the way Infinity does inside vectors (shortest, no trailing zeros).
fn fmt_float(f: f64) -> String {
    let s = format!("{f}");
    s
}

#[cfg(test)]
mod tests {
    use super::*;
    fn one(sql: &str) -> Statement {
        let mut v = parse(sql).unwrap();
        assert_eq!(v.len(), 1);
        v.remove(0)
    }

    #[test]
    fn create_table() {
        let s = one("CREATE TABLE t (id INT, name VARCHAR, v DOUBLE);");
        assert_eq!(
            s,
            Statement::CreateTable {
                name: "t".into(),
                columns: vec![
                    ColumnDef {
                        name: "id".into(),
                        ty: LogicalType::Integer,
                        type_text: "INT".into(),
                        default: None,
                        not_null: false
                    },
                    ColumnDef {
                        name: "name".into(),
                        ty: LogicalType::Varchar,
                        type_text: "VARCHAR".into(),
                        default: None,
                        not_null: false
                    },
                    ColumnDef {
                        name: "v".into(),
                        ty: LogicalType::Double,
                        type_text: "DOUBLE".into(),
                        default: None,
                        not_null: false
                    },
                ],
            }
        );
    }

    #[test]
    fn drop_table() {
        assert_eq!(
            one("DROP TABLE t"),
            Statement::DropTable { name: "t".into() }
        );
    }

    #[test]
    fn insert_values() {
        assert_eq!(
            one("INSERT INTO t VALUES (1, 'x', 3.5)"),
            Statement::Insert {
                table: "t".into(),
                columns: None,
                rows: vec![vec![
                    Literal::Int(1),
                    Literal::Str("x".into()),
                    Literal::Float(3.5)
                ]],
            }
        );
        assert_eq!(
            one("INSERT INTO t VALUES (1, 'a'), (2, 'b')"),
            Statement::Insert {
                table: "t".into(),
                columns: None,
                rows: vec![
                    vec![Literal::Int(1), Literal::Str("a".into())],
                    vec![Literal::Int(2), Literal::Str("b".into())],
                ],
            }
        );
    }

    #[test]
    fn select_star_and_literal() {
        assert_eq!(
            one("SELECT * FROM t"),
            Statement::Select {
                items: vec![SelectItem::Star],
                from: Some("t".into()),
                join: None,
                where_pred: None,
                group_by: vec![],
                having: None,
                order_by: None,
                limit: None,
                offset: None,
                search: None,
                distinct: false,
            }
        );
        assert_eq!(
            one("SELECT 1, 'a'"),
            Statement::Select {
                items: vec![
                    SelectItem::Literal(Literal::Int(1)),
                    SelectItem::Literal(Literal::Str("a".into()))
                ],
                from: None,
                join: None,
                where_pred: None,
                group_by: vec![],
                having: None,
                order_by: None,
                limit: None,
                offset: None,
                search: None,
                distinct: false,
            }
        );
    }

    #[test]
    fn select_where_and_limit() {
        assert_eq!(
            one("SELECT id, name FROM t WHERE id >= 2 LIMIT 10"),
            Statement::Select {
                items: vec![
                    SelectItem::Column("id".into()),
                    SelectItem::Column("name".into())
                ],
                from: Some("t".into()),
                join: None,
                where_pred: Some(Predicate::simple("id", CmpOp::Ge, Literal::Int(2))),
                group_by: vec![],
                having: None,
                order_by: None,
                limit: Some(10),
                offset: None,
                search: None,
                distinct: false,
            }
        );
    }

    #[test]
    fn select_aggregate_group_order() {
        assert_eq!(
            one("SELECT COUNT(*), SUM(id) FROM t"),
            Statement::Select {
                items: vec![
                    SelectItem::Aggregate(SelectAgg {
                        func: AggFunc::Count,
                        args: vec![],
                        distinct: false
                    }),
                    SelectItem::Aggregate(SelectAgg {
                        func: AggFunc::Sum,
                        args: vec!["id".into()],
                        distinct: false
                    }),
                ],
                from: Some("t".into()),
                join: None,
                where_pred: None,
                group_by: vec![],
                having: None,
                order_by: None,
                limit: None,
                offset: None,
                search: None,
                distinct: false,
            }
        );
        assert_eq!(
            one("SELECT name, COUNT(*) FROM t GROUP BY name ORDER BY name DESC LIMIT 5"),
            Statement::Select {
                items: vec![
                    SelectItem::Column("name".into()),
                    SelectItem::Aggregate(SelectAgg {
                        func: AggFunc::Count,
                        args: vec![],
                        distinct: false
                    }),
                ],
                from: Some("t".into()),
                join: None,
                where_pred: None,
                group_by: vec!["name".into()],
                having: None,
                order_by: Some(OrderKey {
                    column: "name".into(),
                    asc: false
                }),
                limit: Some(5),
                offset: None,
                search: None,
                distinct: false,
            }
        );
    }

    #[test]
    fn select_join() {
        assert_eq!(
            one("SELECT * FROM a JOIN b ON aid = bid WHERE aid >= 1"),
            Statement::Select {
                items: vec![SelectItem::Star],
                from: Some("a".into()),
                join: Some(JoinClause {
                    table: "b".into(),
                    left: "aid".into(),
                    right: "bid".into()
                }),
                where_pred: Some(Predicate::simple("aid", CmpOp::Ge, Literal::Int(1))),
                group_by: vec![],
                having: None,
                order_by: None,
                limit: None,
                offset: None,
                search: None,
                distinct: false,
            }
        );
    }

    #[test]
    fn create_index() {
        assert_eq!(
            one("CREATE INDEX idx_name ON t (name)"),
            Statement::CreateIndex {
                name: "idx_name".into(),
                table: "t".into(),
                column: "name".into()
            }
        );
    }

    #[test]
    fn multiple_statements_and_empty() {
        let v = parse("SELECT 1; SELECT 2;").unwrap();
        assert_eq!(v.len(), 2);
        assert!(parse("   ").unwrap().is_empty());
    }

    #[test]
    fn errors_on_bad_input() {
        assert!(parse("CREATE TABLE").is_err());
        assert!(parse("SELECT * FROM").is_err());
        assert!(parse("SELECT * FROM t WHERE id 2").is_err());
        assert!(parse("BOGUS").is_err());
    }

    #[test]
    fn array_and_brace_literals() {
        let s = one("INSERT INTO t VALUES (1, [0.1, 0.2], {0, 1}, [2 3 4])");
        match s {
            Statement::Insert { rows, .. } => {
                assert_eq!(rows[0][1], Literal::Array(vec!["0.1".into(), "0.2".into()]));
                assert_eq!(rows[0][2], Literal::Array(vec!["0".into(), "1".into()]));
                assert_eq!(
                    rows[0][3],
                    Literal::Array(vec!["2".into(), "3".into(), "4".into()])
                );
            }
            other => panic!("expected insert, got {other:?}"),
        }
        // sparse + nested tensor
        let s = one("SELECT c1 FROM t SEARCH MATCH SPARSE (c2, [0:1.0,20:2.0], 'ip', 3)");
        assert!(matches!(
            s,
            Statement::Select {
                search: Some(_),
                ..
            }
        ));
    }

    #[test]
    fn function_select_items_and_predicates() {
        let s = one("SELECT ROUND(x, 4), ROW_ID() FROM t WHERE sqrt(x) > 1 AND y = z");
        match s {
            Statement::Select {
                items, where_pred, ..
            } => {
                assert!(matches!(&items[0], SelectItem::Func { name, .. } if name == "ROUND"));
                assert!(matches!(&items[1], SelectItem::Func { name, .. } if name == "ROW_ID"));
                assert!(matches!(where_pred, Some(Predicate::Bool(..))));
            }
            other => panic!("expected select, got {other:?}"),
        }
    }

    #[test]
    fn search_match_vector_with_options() {
        let s = one(
            "SELECT c1 FROM t SEARCH MATCH VECTOR (c2, [0.3, 0.3], 'float', 'cosine', 3) \
             WITH (threshold = 0.85) USING INDEX (idx1)",
        );
        match s {
            Statement::Select {
                search: Some(sc), ..
            } => {
                assert_eq!(sc.matches.len(), 1);
                assert_eq!(sc.matches[0].kind, "VECTOR");
                assert_eq!(sc.matches[0].column, "c2");
                assert!(sc.with_opts.as_deref().unwrap().contains("threshold"));
                assert_eq!(sc.using_index.as_deref(), Some("idx1"));
            }
            other => panic!("expected search, got {other:?}"),
        }
    }

    #[test]
    fn drop_if_exists_and_copy_and_utility() {
        assert_eq!(
            one("DROP TABLE IF EXISTS t"),
            Statement::DropTable { name: "t".into() }
        );
        assert_eq!(
            one("DROP INDEX idx1 ON t"),
            Statement::DropIndex {
                name: "idx1".into()
            }
        );
        match one("COPY t FROM '/x/y.csv' WITH (DELIMITER ',', FORMAT CSV)") {
            Statement::Copy {
                table,
                from,
                path,
                opts,
            } => {
                assert_eq!(table, "t");
                assert!(from);
                assert_eq!(path, "/x/y.csv");
                assert_eq!(opts[0].0, "DELIMITER");
            }
            other => panic!("expected copy, got {other:?}"),
        }
        assert!(matches!(one("SET CONFIG a 1"), Statement::SetConfig { .. }));
    }

    #[test]
    fn parameterised_column_types() {
        assert_eq!(
            one("CREATE TABLE t (a INT, b TENSOR(FLOAT,16), c EMBEDDING(FLOAT, 4))"),
            Statement::CreateTable {
                name: "t".into(),
                columns: vec![
                    ColumnDef {
                        name: "a".into(),
                        ty: LogicalType::Integer,
                        type_text: "INT".into(),
                        default: None,
                        not_null: false
                    },
                    ColumnDef {
                        name: "b".into(),
                        ty: LogicalType::Invalid,
                        type_text: "TENSOR(FLOAT,16)".into(),
                        default: None,
                        not_null: false
                    },
                    ColumnDef {
                        name: "c".into(),
                        ty: LogicalType::Invalid,
                        type_text: "EMBEDDING(FLOAT,4)".into(),
                        default: None,
                        not_null: false
                    },
                ],
            }
        );
    }
}
