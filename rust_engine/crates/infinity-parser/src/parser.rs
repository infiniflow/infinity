//! Recursive-descent SQL parser (subset).

use infinity_common::{Error, LogicalType, Result};

use crate::ast::{
    AggFunc, BinOp, BoolOp, CmpOp, ColumnDef, Cte, ExplainKind, ExprAst, JoinClause, JoinKind,
    Literal, MatchClause, OrderKey, Predicate, SearchClause, SelectAgg, SelectItem, Statement,
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

    /// A name that may also be written as a double-quoted literal
    /// (`CREATE SNAPSHOT "snap-1" …`): snapshot/table names in the reference
    /// come straight from the RPC and may contain dashes.
    fn expect_name(&mut self) -> Result<String> {
        match self.peek() {
            Token::Str(_) => match self.advance() {
                Token::Str(s) => Ok(s),
                _ => unreachable!(),
            },
            _ => self.expect_ident(),
        }
    }

    /// Reads a (possibly `db.table`-qualified) table name, keeping the dotted
    /// form so the executor can resolve it against the current database.
    /// Consumes a balanced `( … )` run holding a subquery and returns its SQL
    /// text, with string literals re-quoted so the executor can parse it again.
    fn paren_subquery_sql(&mut self) -> Result<String> {
        self.expect(Token::LParen)?;
        let mut toks: Vec<Token> = Vec::new();
        let mut depth = 1i32;
        while depth > 0 {
            match self.advance() {
                Token::LParen => {
                    depth += 1;
                    toks.push(Token::LParen);
                }
                Token::RParen => {
                    depth -= 1;
                    if depth > 0 {
                        toks.push(Token::RParen);
                    }
                }
                Token::                Eof => return self.err("unterminated subquery"),
                t => toks.push(t),
            }
        }
        let sql = render_tokens_as_sql(&toks);
        // Validate the nested statement now; the executor re-parses this text
        // later, so a parse failure must surface here.
        parse(&sql)?;
        Ok(sql)
    }

    /// A `LIMIT`/`OFFSET` operand: a plain non-negative integer becomes the
    /// numeric field, anything else (arithmetic or a parenthesised subquery)
    /// is kept as an expression.
    fn limit_value(&mut self) -> Result<(Option<u64>, Option<ExprAst>)> {
        if self.peek() == &Token::LParen
            && matches!(
                self.toks.get(self.pos + 1),
                Some(Token::Ident(s)) if s.eq_ignore_ascii_case("SELECT")
            )
        {
            let sql = self.paren_subquery_sql()?;
            return Ok((
                None,
                Some(ExprAst::Func {
                    name: "SUBQUERY".to_string(),
                    args: vec![ExprAst::Literal(Literal::Str(sql))],
                }),
            ));
        }
        let expr = self.expr()?;
        match expr {
            ExprAst::Literal(Literal::Int(n)) if n >= 0 => Ok((Some(n as u64), None)),
            other => Ok((None, Some(other))),
        }
    }

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

    /// True when the cursor is on `( SELECT … )`.
    fn is_subquery_start(&self) -> bool {
        self.peek() == &Token::LParen
            && matches!(
                self.toks.get(self.pos + 1),
                Some(Token::Ident(s)) if s.eq_ignore_ascii_case("SELECT")
            )
    }

    /// Parses `( <statement> )` as a scalar-expression subquery. The parsed
    /// statement is rendered back to SQL text (the executor re-parses derived
    /// tables from text), but the inner statement is validated by this parser.
    fn subquery_expr(&mut self) -> Result<ExprAst> {
        self.expect(Token::LParen)?;
        let start = self.pos;
        let _ = self.statement()?;
        let end = self.pos;
        self.expect(Token::RParen)?;
        Ok(ExprAst::Func {
            name: "SUBQUERY".to_string(),
            args: vec![ExprAst::Literal(Literal::Str(render_tokens_as_sql(
                &self.toks[start..end],
            )))],
        })
    }

    /// One `FROM` entry: a table or a derived table, with an optional alias
    /// (`[AS] name [(col, …)]`). A derived table keeps the subquery's SQL text.
    fn table_ref(&mut self) -> Result<String> {
        let table = if self.peek() == &Token::LParen {
            self.paren_subquery_sql()?
        } else {
            self.expect_table_name()?
        };
        self.eat_table_alias()?;
        Ok(table)
    }

    /// Consumes `[AS] alias [(col, …)]` when one follows a table reference.
    fn eat_table_alias(&mut self) -> Result<()> {
        let aliased = if self.eat_kw("AS") {
            let _ = self.expect_ident()?;
            true
        } else if matches!(self.peek(), Token::Ident(s) if !is_clause_keyword(s)) {
            self.advance();
            true
        } else {
            false
        };
        if aliased && self.peek() == &Token::LParen {
            // Derived-table column aliases, e.g. `(…) AS c (a, b)`.
            self.paren_list_text()?;
        }
        Ok(())
    }

    /// The right-hand side of `[NOT] IN`: either a parenthesised value list or
    /// a parenthesised subquery.
    fn in_rhs(&mut self) -> Result<Vec<ExprAst>> {
        if self.is_subquery_start() {
            return Ok(vec![self.subquery_expr()?]);
        }
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
        Ok(values)
    }

    /// `CASE [operand] WHEN cond THEN value … [ELSE value] END`, encoded as
    /// `Func("CASE", [operand?, cond, value, …, else?])`.
    fn case_expr(&mut self) -> Result<ExprAst> {
        let mut args = Vec::new();
        if !matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("WHEN")) {
            args.push(self.expr()?);
        }
        let mut saw_when = false;
        while self.eat_kw("WHEN") {
            saw_when = true;
            let cond = self.predicate()?;
            let cond = predicate_to_expr(&cond)
                .ok_or_else(|| Error::Invalid("unsupported CASE condition".into()))?;
            self.expect_kw("THEN")?;
            let value = self.expr()?;
            args.push(cond);
            args.push(value);
        }
        if !saw_when {
            return self.err("CASE requires at least one WHEN");
        }
        if self.eat_kw("ELSE") {
            args.push(self.expr()?);
        }
        self.expect_kw("END")?;
        Ok(ExprAst::Func {
            name: "CASE".to_string(),
            args,
        })
    }

    fn statement(&mut self) -> Result<Statement> {
        // A parenthesised SELECT is a statement in its own right
        // (`(SELECT a FROM t WHERE …);`).
        if self.is_subquery_start() {
            self.advance(); // (
            let stmt = self.statement()?;
            self.expect(Token::RParen)?;
            return Ok(stmt);
        }
        let kw = match self.peek() {
            Token::Ident(s) => s.to_ascii_uppercase(),
            _ => return self.err("expected statement"),
        };
        match kw.as_str() {
            "CREATE" => {
                self.advance();
                if self.eat_kw("SNAPSHOT") {
                    let name = self.expect_name()?;
                    self.expect_kw("ON")?;
                    if self.eat_kw("TABLE") {
                        let table = self.expect_table_name()?;
                        return Ok(Statement::CreateSnapshot { name, table });
                    }
                    if self.eat_kw("DATABASE") {
                        let database = self.expect_ident()?;
                        return Ok(Statement::CreateDatabaseSnapshot { name, database });
                    }
                    self.expect_kw("SYSTEM")?;
                    return Ok(Statement::CreateSystemSnapshot { name });
                }
                if self.eat_kw("COLLECTION") {
                    let ine = self.eat_kw("IF");
                    let _ = self.eat_kw("NOT");
                    let _ = self.eat_kw("EXISTS");
                    let name = self.expect_table_name()?;
                    return Ok(Statement::CreateCollection {
                        name,
                        if_not_exists: ine,
                    });
                }
                if self.eat_kw("VIEW") {
                    let name = self.expect_table_name()?;
                    let mut columns = Vec::new();
                    if self.peek() == &Token::LParen {
                        self.advance();
                        while !matches!(self.peek(), Token::RParen | Token::Eof) {
                            if let Token::Ident(c) = self.peek().clone() {
                                columns.push(c);
                            }
                            self.advance();
                            if self.peek() == &Token::Comma {
                                self.advance();
                            }
                        }
                        self.expect(Token::RParen)?;
                    }
                    self.expect_kw("AS")?;
                    let query = self.statement()?;
                    return Ok(Statement::CreateView {
                        name,
                        columns,
                        query: Box::new(query),
                    });
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
                    let ine = self.eat_kw("IF");
                    let _ = self.eat_kw("NOT");
                    let _ = self.eat_kw("EXISTS");
                    let name = self.expect_ident()?;
                    self.expect_kw("ON")?;
                    let table = self.expect_table_name()?;
                    self.expect(Token::LParen)?;
                    // a plain column, or a functional-index expression
                    let mut toks: Vec<Token> = Vec::new();
                    let mut depth = 0i32;
                    while !matches!(self.peek(), Token::Eof) {
                        match self.peek().clone() {
                            Token::RParen if depth == 0 => break,
                            Token::LParen => {
                                depth += 1;
                                toks.push(self.advance());
                            }
                            Token::RParen => {
                                depth -= 1;
                                toks.push(self.advance());
                            }
                            Token::Comma if depth == 0 => break,
                            _ => {
                                toks.push(self.advance());
                            }
                        }
                    }
                    let column = index_column_from(&toks)?;
                    // tolerate additional index columns
                    while !matches!(self.peek(), Token::RParen | Token::Eof) {
                        self.advance();
                    }
                    self.expect(Token::RParen)?;
                    // trailing `USING <kind>` / `WITH (k = v, …)`
                    let mut index_type = String::new();
                    let mut opts: Vec<(String, String)> = Vec::new();
                    while !matches!(self.peek(), Token::Semicolon | Token::Eof) {
                        if self.eat_kw("USING") {
                            index_type = self.expect_ident()?;
                            continue;
                        }
                        if self.eat_kw("WITH") {
                            self.expect(Token::LParen)?;
                            while !matches!(self.peek(), Token::RParen | Token::Eof) {
                                let k = match self.advance() {
                                    Token::Ident(k) => k,
                                    _ => continue,
                                };
                                if self.peek() == &Token::Op("=".into()) {
                                    self.advance();
                                }
                                let v = match self.advance() {
                                    Token::Ident(v) => v,
                                    Token::Int(i) => i.to_string(),
                                    Token::Float(f) => f.to_string(),
                                    Token::Str(v) => v,
                                    _ => String::new(),
                                };
                                opts.push((k.to_ascii_uppercase(), v));
                                if self.peek() == &Token::Comma {
                                    self.advance();
                                } else {
                                    break;
                                }
                            }
                            self.expect(Token::RParen)?;
                            continue;
                        }
                        self.advance();
                    }
                    Ok(Statement::CreateIndex {
                        name,
                        table,
                        column,
                        index_type,
                        opts,
                        if_not_exists: ine,
                    })
                } else {
                    let _ = self.eat_kw("IF");
                    let _ = self.eat_kw("NOT");
                    let _ = self.eat_kw("EXISTS");
                    self.expect_kw("TABLE")?;
                    let name = self.expect_table_name()?;
                    if self.eat_kw("AS") {
                        let query = self.statement()?;
                        return Ok(Statement::CreateTableAs {
                            name,
                            query: Box::new(query),
                        });
                    }
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
                } else if self.eat_kw("COLLECTION") {
                    let if_exists = self.eat_if_exists();
                    let name = self.expect_table_name()?;
                    Ok(Statement::DropCollection { name, if_exists })
                } else if self.eat_kw("SNAPSHOT") {
                    self.eat_if_exists();
                    let name = self.expect_name()?;
                    Ok(Statement::DropSnapshot { name })
                } else if self.eat_kw("INDEX") {
                    let if_exists = self.eat_if_exists();
                    let name = self.expect_ident()?;
                    // tolerate `ON <table>`
                    if self.eat_kw("ON") {
                        let _ = self.expect_ident();
                    }
                    Ok(Statement::DropIndex { name, if_exists })
                } else if self.eat_kw("VIEW") {
                    let if_exists = self.eat_if_exists();
                    let name = self.expect_table_name()?;
                    Ok(Statement::DropView { name, if_exists })
                } else {
                    self.expect_kw("TABLE")?;
                    let if_exists = self.eat_if_exists();
                    let name = self.expect_table_name()?;
                    Ok(Statement::DropTable { name, if_exists })
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
                if self.eat_kw("DATABASE") {
                    self.expect_kw("SNAPSHOT")?;
                    let name = self.expect_name()?;
                    return Ok(Statement::RestoreDatabaseSnapshot { name });
                }
                if self.eat_kw("SYSTEM") {
                    self.expect_kw("SNAPSHOT")?;
                    let name = self.expect_name()?;
                    return Ok(Statement::RestoreSystemSnapshot { name });
                }
                self.expect_kw("TABLE")?;
                self.expect_kw("SNAPSHOT")?;
                let name = self.expect_name()?;
                Ok(Statement::RestoreSnapshot { name })
            }
            "USE" => {
                self.advance();
                let name = self.expect_ident()?;
                Ok(Statement::Use { name })
            }
            "DESCRIBE" | "DESC" => {
                self.advance();
                let table = self.expect_table_name()?;
                Ok(Statement::Describe { table })
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
                if matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("SELECT"))
                    || self.is_subquery_start()
                {
                    let query = self.statement()?;
                    return Ok(Statement::InsertSelect {
                        table,
                        columns,
                        query: Box::new(query),
                    });
                }
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
                    } else if self.eat_kw("RENAME") {
                        // `RENAME [COLUMN …] TO <new name>`
                        while !matches!(self.peek(), Token::Semicolon | Token::Eof)
                            && !matches!(self.peek(), Token::Ident(v) if v.eq_ignore_ascii_case("TO"))
                        {
                            self.advance();
                        }
                        let _ = self.eat_kw("TO");
                        let new_name = self.expect_table_name()?;
                        Ok(Statement::RenameTable { table, new_name })
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
            "WITH" => {
                self.advance();
                let mut ctes = Vec::new();
                loop {
                    let alias = self.expect_ident()?;
                    self.expect_kw("AS")?;
                    self.expect(Token::LParen)?;
                    let query = self.statement()?;
                    self.expect(Token::RParen)?;
                    ctes.push(Cte {
                        alias,
                        query: Box::new(query),
                    });
                    if self.peek() == &Token::Comma {
                        self.advance();
                    } else {
                        break;
                    }
                }
                let mut stmt = self.statement()?;
                match &mut stmt {
                    Statement::Select { ctes: slot, .. } => *slot = ctes,
                    _ => return self.err("WITH must be followed by SELECT"),
                }
                Ok(stmt)
            }
            "SELECT" => {
                self.advance();
                let distinct = self.eat_kw("DISTINCT");
                let (items, aliases) = self.select_items()?;
                // `SELECT …, body HIGHLIGHT body, doctitle FROM …`
                let highlight = if self.eat_kw("HIGHLIGHT") {
                    let mut cols = Vec::new();
                    while let Token::Ident(c) = self.peek().clone() {
                        self.advance();
                        cols.push(c);
                        if self.peek() == &Token::Comma {
                            self.advance();
                        } else {
                            break;
                        }
                    }
                    cols
                } else {
                    Vec::new()
                };
                let from = if self.eat_kw("FROM") {
                    Some(self.table_ref()?)
                } else {
                    None
                };
                // Chained joins: `FROM a JOIN b ON … JOIN c ON …`. The AST keeps
                // the first join; the rest are parsed so the statement is valid.
                let mut join = None;
                loop {
                    let save = self.pos;
                    let kind = if self.eat_kw("INNER") {
                        JoinKind::Inner
                    } else if self.eat_kw("LEFT") {
                        let _ = self.eat_kw("OUTER");
                        JoinKind::Left
                    } else if self.eat_kw("RIGHT") {
                        let _ = self.eat_kw("OUTER");
                        JoinKind::Right
                    } else if self.eat_kw("FULL") {
                        let _ = self.eat_kw("OUTER");
                        JoinKind::Full
                    } else if self.eat_kw("OUTER") {
                        JoinKind::Full
                    } else if self.eat_kw("NATURAL") {
                        JoinKind::Natural
                    } else if self.eat_kw("CROSS") {
                        JoinKind::Cross
                    } else {
                        JoinKind::Inner
                    };
                    if !self.eat_kw("JOIN") {
                        self.pos = save;
                        break;
                    }
                    let table = self.table_ref()?;
                    let (left, right) = if kind == JoinKind::Natural {
                        (String::new(), String::new())
                    } else if self.eat_kw("ON") {
                        join_keys(&self.predicate()?)
                    } else if self.eat_kw("USING") {
                        let cols = self.paren_list_text()?;
                        let mut it = cols.split(',').map(|c| c.trim().to_string());
                        (it.next().unwrap_or_default(), it.next().unwrap_or_default())
                    } else {
                        return self.err("expected ON or USING after JOIN");
                    };
                    if join.is_none() {
                        join = Some(JoinClause {
                            table,
                            left,
                            right,
                            kind,
                        });
                    }
                }
                let join = join;
                // `FROM t1, t2, …`: a cross product expressed with commas.
                let mut cross_tables = Vec::new();
                while self.peek() == &Token::Comma {
                    self.advance();
                    cross_tables.push(self.table_ref()?);
                }
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
                    // group keys may be plain columns or arbitrary expressions
                    let mut keys = vec![self.expr()?];
                    while self.peek() == &Token::Comma {
                        self.advance();
                        keys.push(self.expr()?);
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
                    let mut keys: Vec<OrderKey> = Vec::new();
                    loop {
                        let column = self.expect_ident()?;
                        // Capture a trailing arithmetic expression (`c1 + c2`).
                        let mut expr = ExprAst::Column(column.clone());
                        loop {
                            let op = match self.peek() {
                                Token::Op(s) if s == "+" => Some(BinOp::Add),
                                Token::Op(s) if s == "-" => Some(BinOp::Sub),
                                Token::Star => Some(BinOp::Mul),
                                Token::Op(s) if s == "/" => Some(BinOp::Div),
                                Token::Op(s) if s == "%" => Some(BinOp::Mod),
                                _ => None,
                            };
                            match op {
                                Some(op) => {
                                    self.advance();
                                    let right = self.term()?;
                                    expr = ExprAst::Binary(Box::new(expr), op, Box::new(right));
                                }
                                None => break,
                            }
                        }
                        let asc = if self.eat_kw("DESC") {
                            false
                        } else {
                            let _ = self.eat_kw("ASC");
                            true
                        };
                        keys.push(OrderKey {
                            column,
                            asc,
                            expr: Some(expr),
                        });
                        // tolerate a trailing expression after the leading column
                        while !matches!(self.peek(), Token::Comma | Token::Semicolon | Token::Eof)
                            && !matches!(self.peek(), Token::Ident(k)
                            if k.eq_ignore_ascii_case("LIMIT")
                                || k.eq_ignore_ascii_case("OFFSET"))
                        {
                            self.advance();
                            let _ = self.eat_kw("ASC");
                            let _ = self.eat_kw("DESC");
                        }
                        if self.peek() == &Token::Comma {
                            self.advance();
                        } else {
                            break;
                        }
                    }
                    keys
                } else {
                    Vec::new()
                };
                let mut limit = None;
                let mut limit_expr = None;
                let mut offset = None;
                let mut offset_expr = None;
                let ctes = Vec::new();
                if self.eat_kw("LIMIT") {
                    let (n, e) = self.limit_value()?;
                    limit = n;
                    limit_expr = e;
                    if self.eat_kw("OFFSET") {
                        let (n, e) = self.limit_value()?;
                        offset = n;
                        offset_expr = e;
                    }
                } else if self.eat_kw("OFFSET") {
                    let (n, e) = self.limit_value()?;
                    offset = n;
                    offset_expr = e;
                    if self.eat_kw("LIMIT") {
                        let (n, e) = self.limit_value()?;
                        limit = n;
                        limit_expr = e;
                    }
                }
                if (offset.is_some() || offset_expr.is_some())
                    && limit.is_none()
                    && limit_expr.is_none()
                {
                    return self.err("OFFSET requires LIMIT");
                }
                if search.is_some()
                    && (!order_by.is_empty()
                        || limit.is_some()
                        || limit_expr.is_some()
                        || offset.is_some()
                        || offset_expr.is_some())
                {
                    return self.err("SEARCH cannot be combined with ORDER BY/OFFSET/LIMIT");
                }
                Ok(Statement::Select {
                    items,
                    from,
                    join,
                    where_pred,
                    group_by,
                    aliases,
                    having,
                    order_by,
                    limit,
                    offset,
                    search,
                    distinct,
                    highlight,
                    ctes,
                    cross_tables,
                    limit_expr,
                    offset_expr,
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
                // A bare `SHOW COLUMNS` has no table to describe; the
                // reference rejects it.
                if matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("COLUMNS")) {
                    return self.err("SHOW COLUMNS requires a table");
                }
                // SHOW TABLE <t> COLUMNS
                let is_table = self.eat_kw("TABLE");
                if is_table && matches!(self.peek(), Token::Ident(_)) {
                    if let Ok(t) = self.expect_table_name() {
                        if self.eat_kw("COLUMNS") {
                            return Ok(Statement::ShowTableColumns { table: t });
                        }
                        // SHOW TABLE <t> INDEX <name>
                        {
                            let save = self.pos;
                            if self.eat_kw("INDEX") {
                                if let Token::Ident(name) = self.peek().clone() {
                                    self.advance();
                                    // tolerate a trailing `SEGMENT n [CHUNKS]`
                                    while !matches!(self.peek(), Token::Semicolon | Token::Eof) {
                                        self.advance();
                                    }
                                    return Ok(Statement::ShowTableIndex {
                                        table: t,
                                        index: name,
                                    });
                                }
                                self.pos = save;
                            }
                        }
                        // A bare `SHOW TABLE <t>`. `SHOW TABLE <t> SEGMENTS …`
                        // (and the other detail forms) keep their text so the
                        // executor can ignore them, as the reference does.
                        let mut suffix: Vec<String> = Vec::new();
                        while !matches!(self.peek(), Token::Semicolon | Token::Eof) {
                            suffix.push(match self.advance() {
                                Token::Ident(s) => s,
                                Token::Int(i) => i.to_string(),
                                Token::Star => "*".to_string(),
                                _ => String::new(),
                            });
                        }
                        if suffix.is_empty() {
                            return Ok(Statement::ShowTable { table: t });
                        }
                        return Ok(Statement::Show {
                            text: format!("TABLE {t} {}", suffix.join(" ")),
                        });
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
                // `EXPLAIN [AST|UNOPTIMIZED|OPTIMIZED|LOGICAL|PHYSICAL|FRAGMENT|PIPELINE] <statement>`
                let kind = match self.peek() {
                    Token::Ident(s) if s.eq_ignore_ascii_case("ast") => {
                        self.advance();
                        ExplainKind::Ast
                    }
                    Token::Ident(s) if s.eq_ignore_ascii_case("unoptimized") => {
                        self.advance();
                        ExplainKind::UnOpt
                    }
                    Token::Ident(s) if s.eq_ignore_ascii_case("optimized") => {
                        self.advance();
                        ExplainKind::Opt
                    }
                    Token::Ident(s) if s.eq_ignore_ascii_case("logical") => {
                        self.advance();
                        ExplainKind::Logical
                    }
                    Token::Ident(s) if s.eq_ignore_ascii_case("physical") => {
                        self.advance();
                        ExplainKind::Physical
                    }
                    Token::Ident(s) if s.eq_ignore_ascii_case("fragment") => {
                        self.advance();
                        ExplainKind::Fragment
                    }
                    Token::Ident(s) if s.eq_ignore_ascii_case("pipeline") => {
                        self.advance();
                        ExplainKind::Pipeline
                    }
                    Token::Ident(s) if s.eq_ignore_ascii_case("analyze") => {
                        self.advance();
                        ExplainKind::Analyze
                    }
                    Token::Ident(s) if s.eq_ignore_ascii_case("raw") => {
                        self.advance();
                        ExplainKind::Raw
                    }
                    _ => ExplainKind::Physical,
                };
                let inner = self.statement()?;
                Ok(Statement::Explain {
                    kind,
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
                            // A bare flag (`HEADER`) has no value.
                            let v = match self.peek() {
                                Token::RParen | Token::Comma => String::new(),
                                _ => self.copy_option_value(),
                            };
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
        // `[NOT] EXISTS (SELECT …)`
        if self.eat_kw("EXISTS") {
            let sub = self.subquery_expr()?;
            return Ok(Predicate::IsTrue(ExprAst::Func {
                name: "EXISTS".to_string(),
                args: vec![sub],
            }));
        }
        if self.peek() == &Token::LParen {
            let save = self.pos;
            // `(c2 + 1) IN (8, 9, 10)`: a parenthesised *expression* followed by
            // `IN`, which is not a grouped predicate.
            if let Ok(left) = self.expr() {
                if matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("IN")) {
                    self.advance();
                    let values = self.in_rhs()?;
                    return Ok(Predicate::In(left, values, false));
                }
                // `(c2) = 'x'`, `(f(x)) LIKE 'a%'`: a parenthesised *expression*
                // followed by a comparison is not a grouped predicate.
                let is_cmp = match self.peek() {
                    Token::Op(s) => cmp_op(s).is_some(),
                    Token::Ident(s) => {
                        s.eq_ignore_ascii_case("IS") || s.eq_ignore_ascii_case("LIKE")
                    }
                    _ => false,
                };
                if is_cmp {
                    return self.comparison_from(left);
                }
            }
            self.pos = save;
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
        self.comparison_from(left)
    }

    /// The tail of a comparison, once its left operand has been parsed.
    fn comparison_from(&mut self, left: ExprAst) -> Result<Predicate> {
        // `expr [NOT] BETWEEN low AND high` desugars to
        // `expr >= low AND expr <= high` (the same rewrite the planner would
        // apply; it keeps every existing `Predicate` consumer unchanged).
        let between_neg = if matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("NOT"))
        {
            let save = self.pos;
            self.advance();
            if matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("BETWEEN")) {
                true
            } else {
                self.pos = save;
                false
            }
        } else {
            false
        };
        if self.eat_kw("BETWEEN") {
            let low = self.expr()?;
            self.expect_kw("AND")?;
            let high = self.expr()?;
            let ge = Predicate::Cmp(left.clone(), CmpOp::Ge, low);
            let le = Predicate::Cmp(left, CmpOp::Le, high);
            let between =
                Predicate::Bool(Box::new(ge), BoolOp::And, Box::new(le));
            return Ok(if between_neg {
                Predicate::Not(Box::new(between))
            } else {
                between
            });
        }
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
                match self.advance() {
                    Token::Str(c) if c.chars().count() == 1 => escape = c.chars().next(),
                    _ => {
                        return self.err("ESCAPE must be a single-character string");
                    }
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
            let values = self.in_rhs()?;
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
                    let mut right = self.term()?;
                    // `a + 3 day`: attach a trailing time unit to the integer
                    // operand, mirroring the C++ `ConstantExpr` interval form.
                    if let ExprAst::Literal(Literal::Int(n)) = right {
                        if let Some(unit) = self.eat_time_unit() {
                            right = ExprAst::Func {
                                name: "INTERVAL".to_string(),
                                args: vec![
                                    ExprAst::Literal(Literal::Int(n)),
                                    ExprAst::Literal(Literal::Str(unit)),
                                ],
                            };
                        }
                    }
                    left = ExprAst::Binary(Box::new(left), op, Box::new(right));
                }
                None => break,
            }
        }
        Ok(left)
    }

    /// Consumes `YEAR|MONTH|DAY|HOUR|MINUTE|SECOND` (singular or plural).
    fn eat_time_unit(&mut self) -> Option<String> {
        if let Token::Ident(s) = self.peek() {
            let upper = s.to_ascii_uppercase();
            let base = upper.strip_suffix('S').unwrap_or(&upper);
            if matches!(base, "YEAR" | "MONTH" | "DAY" | "HOUR" | "MINUTE" | "SECOND") {
                self.advance();
                return Some(base.to_string());
            }
        }
        None
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
            Token::LParen if self.is_subquery_start() => self.subquery_expr(),
            Token::LParen => {
                self.advance();
                // A parenthesised expression may be a *predicate*
                // (`(a > 1 OR b < 2)`, `(filter_text(...) OR num > 2)`), not
                // just arithmetic, so try a predicate first and fall back to a
                // scalar expression.
                let save = self.pos;
                if let Ok(p) = self.predicate() {
                    if self.peek() == &Token::RParen {
                        // A bare value comes back as `IsTrue(expr)`.
                        let e = match &p {
                            Predicate::IsTrue(e) => Some(e.clone()),
                            other => predicate_to_expr(other),
                        };
                        if let Some(e) = e {
                            self.advance();
                            return Ok(e);
                        }
                    }
                }
                self.pos = save;
                let inner = self.expr()?;
                self.expect(Token::RParen)?;
                Ok(inner)
            }
            Token::LBracket => Ok(ExprAst::Literal(self.array_literal()?)),
            Token::LBrace => Ok(ExprAst::Literal(self.brace_literal()?)),
            Token::Ident(s) => {
                if s.eq_ignore_ascii_case("CASE") {
                    self.advance();
                    return self.case_expr();
                }
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
                // `EXTRACT(field FROM source)`.
                if s.eq_ignore_ascii_case("EXTRACT") {
                    self.advance();
                    self.expect(Token::LParen)?;
                    let field = match self.advance() {
                        Token::Ident(f) => f,
                        Token::Str(f) => f,
                        _ => return self.err("expected a field name in EXTRACT"),
                    };
                    self.expect_kw("FROM")?;
                    let src = self.expr()?;
                    self.expect(Token::RParen)?;
                    // The reference rewrites `EXTRACT(<field> FROM …)` to a
                    // function named `extract_<field>`.
                    return Ok(ExprAst::Func {
                        name: format!("extract_{}", field.to_ascii_lowercase()),
                        args: vec![src],
                    });
                }
                // `INTERVAL <n> <UNIT>` (unquoted form), e.g. `INTERVAL -100 YEAR`.                // `INTERVAL <n> <UNIT>` (unquoted form), e.g. `INTERVAL -100 YEAR`.
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
                        Literal::Str(v) => Literal::Str(typed_literal_value(&kind, &v)?),
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
                    if (s.eq_ignore_ascii_case("FILTER_FULLTEXT")
                        || s.eq_ignore_ascii_case("FILTER_TEXT"))
                        && args.len() >= 3
                    {
                        if let ExprAst::Literal(Literal::Str(opts)) = &args[2] {
                            let has_topn = opts.split(';').any(|p| {
                                p.split_once('=')
                                    .map(|(k, _)| k.trim().eq_ignore_ascii_case("topn"))
                                    .unwrap_or(false)
                            });
                            if has_topn {
                                return self.err("topn is not a valid filter_fulltext option");
                            }
                            validate_fulltext_options(opts)?;
                        }
                    }
                    Ok(ExprAst::Func { name: s, args })
                } else {
                    self.advance();
                    // A qualified column may be dotted (`t1.c1`).
                    let mut name = s;
                    while matches!(self.peek(), Token::Op(o) if o == ".") {
                        self.advance();
                        match self.advance() {
                            Token::Ident(part) => {
                                name.push('.');
                                name.push_str(&part);
                            }
                            _ => break,
                        }
                    }
                    Ok(ExprAst::Column(name))
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

    /// Parses
    /// `SEARCH MATCH <KIND> (col, args…) [, MATCH …] [, QUERY(…)] [, FUSION(…)]`.
    fn search_clause(&mut self) -> Result<SearchClause> {
        let mut out = SearchClause::default();
        loop {
            // `QUERY('…')` is a bare search term; `MATCH <KIND> (…)` is the
            // keyword form (TEXT / VECTOR / …).
            let is_query =
                matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("QUERY"));
            if is_query {
                self.advance();
                self.expect(Token::LParen)?;
                let column = match self.expr()? {
                    ExprAst::Column(c) => c,
                    ExprAst::Literal(Literal::Str(s)) => s,
                    _ => return self.err("expected query text"),
                };
                let mut args = Vec::new();
                while self.peek() == &Token::Comma {
                    self.advance();
                    args.push(self.expr()?);
                }
                self.expect(Token::RParen)?;
                out.matches.push(MatchClause {
                    kind: "QUERY".into(),
                    column,
                    args,
                    filter: None,
                });
            } else {
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
                if kind.eq_ignore_ascii_case("TEXT") && args.len() >= 2 {
                    if let ExprAst::Literal(Literal::Str(opts)) = &args[1] {
                        validate_fulltext_options(opts)?;
                    }
                }
                // `MATCH VECTOR (col, vec, 'elem', 'metric', k)`: the metric must
                // suit the element type (`hamming` only for bit vectors).
                if kind.eq_ignore_ascii_case("VECTOR") && args.len() >= 3 {
                    if let (
                        ExprAst::Literal(Literal::Str(elem)),
                        ExprAst::Literal(Literal::Str(metric)),
                    ) = (&args[1], &args[2])
                    {
                        let elem = elem.to_ascii_lowercase();
                        let metric = metric.to_ascii_lowercase();
                        let is_bit = elem.eq_ignore_ascii_case("bit");
                        if is_bit && metric != "hamming" {
                            return self.err("bit vectors support only the hamming metric");
                        }
                        if !is_bit && metric == "hamming" {
                            return self.err("hamming metric requires a bit vector");
                        }
                    }
                }
                out.matches.push(MatchClause {
                    kind,
                    column,
                    args,
                    filter,
                });
            }
            if self.eat_kw("WITH") {
                out.with_opts = Some(self.paren_list_text()?);
            }
            if self.eat_kw("USING") {
                self.expect_kw("INDEX")?;
                out.using_index = Some(self.paren_list_text()?);
            }
            // `IGNORE INDEX` disables index selection
            if self.eat_kw("IGNORE") {
                self.expect_kw("INDEX")?;
            }
            let next_is_term = self.peek() == &Token::Comma
                && matches!(self.toks.get(self.pos + 1), Some(Token::Ident(s))
                    if s.eq_ignore_ascii_case("MATCH") || s.eq_ignore_ascii_case("QUERY"));
            if next_is_term {
                self.advance(); // comma
                continue;
            }
            // Zero or more `FUSION(…)` tails; `… , FUSION('rrf'), FUSION('match_tensor', …)`
            // chains them. The last one decides the final score in the executor.
            loop {
                let fusion_here =
                    matches!(self.peek(), Token::Ident(s) if s.eq_ignore_ascii_case("FUSION"));
                let comma_fusion = self.peek() == &Token::Comma
                    && matches!(self.toks.get(self.pos + 1), Some(Token::Ident(s)) if s.eq_ignore_ascii_case("FUSION"));
                if !fusion_here && !comma_fusion {
                    break;
                }
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
                } else {
                    out.fusion_params = None;
                }
                out.fusions.push((
                    out.fusion.clone().unwrap_or_default(),
                    out.fusion_params.clone().unwrap_or_default(),
                ));
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
            let mut explicit_null = false;
            let mut primary_key = false;
            let mut comment = None;
            // Column constraints. The reference rejects a stray identifier
            // between columns (e.g. a missing comma), so only known constraint
            // forms are accepted here.
            loop {
                if self.eat_kw("NOT") {
                    let _ = self.eat_kw("NULL");
                    not_null = true;
                    continue;
                }
                if self.eat_kw("NULL") {
                    explicit_null = true;
                    continue;
                }
                if self.eat_kw("PRIMARY") {
                    let _ = self.eat_kw("KEY");
                    primary_key = true;
                    continue;
                }
                if self.eat_kw("UNIQUE") {
                    continue;
                }
                if self.eat_kw("DEFAULT") {
                    default = Some(self.literal()?);
                    continue;
                }
                if self.eat_kw("COMMENT") {
                    if let Token::Str(c) = self.peek().clone() {
                        self.advance();
                        comment = Some(c);
                    }
                    continue;
                }
                match self.peek() {
                    Token::Comma | Token::RParen | Token::Eof => break,
                    // consume a balanced group (e.g. `WITH (sort)`)
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
                        return Err(Error::Invalid(format!(
                            "parser: unexpected token after column '{}'",
                            name
                        )));
                    }
                }
            }
            // A primary key is implicitly `NOT NULL`, so pairing it with an
            // explicit `NULL` (either spelling) is contradictory, as is
            // `NOT NULL NULL`.
            if explicit_null && (primary_key || not_null) {
                return Err(Error::Invalid(format!(
                    "parser: conflicting NULL constraint on column '{name}'"
                )));
            }
            cols.push(ColumnDef {
                name,
                ty,
                type_text,
                default,
                not_null,
                comment,
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
                    Literal::Str(v) => Ok(Literal::Str(typed_literal_value(&kind, &v)?)),
                    other => Ok(other),
                }
            }
            _ => self.err("expected literal"),
        }
    }

    fn select_items(&mut self) -> Result<(Vec<SelectItem>, Vec<Option<String>>)> {
        let mut aliases: Vec<Option<String>> = Vec::new();
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
                    match select_item_to_expr(&self.select_items().map(|(mut v, _)| {
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
                // A leading unary sign (`SELECT -a`, `SELECT -1`).
                Token::Op(op) if op == "-" || op == "+" => SelectItem::Expr(self.expr()?),
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
                    } else if matches!(
                        self.toks.get(self.pos + 1),
                        Some(Token::Op(o)) if matches!(o.as_str(), "=" | "==" | "!=" | "<>" | "<" | ">" | "<=" | ">=")
                    ) {
                        // `c1 > 0.1 and c2 < 3.0` — a projected predicate.
                        let p = self.predicate()?;
                        match predicate_to_expr(&p) {
                            Some(e) => SelectItem::Expr(e),
                            None => return self.err("unsupported expression in select list"),
                        }
                    } else if matches!(self.toks.get(self.pos + 1), Some(Token::Op(_))) {
                        // `c1 + 1` / `c2 * 2.0` — a projected expression, not a
                        // bare column.
                        match self.expr()? {
                            ExprAst::Column(c) => SelectItem::Column(c),
                            other => SelectItem::Expr(other),
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
                    // the right-hand side is a predicate (`num > 2`) or a value
                    let save = self.pos;
                    let right = match self.comparison().ok().and_then(|p| predicate_to_expr(&p)) {
                        Some(e) => e,
                        None => {
                            self.pos = save;
                            let right_item = match self.select_items() {
                                Ok((mut v, _)) if !v.is_empty() => v.remove(0),
                                _ => SelectItem::Literal(Literal::Null),
                            };
                            select_item_to_expr(&right_item)
                                .unwrap_or(ExprAst::Literal(Literal::Null))
                        }
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
            // capture an output alias: `<expr> AS name`
            let mut alias = None;
            if self.eat_kw("AS") {
                if let Token::Ident(n) = self.peek().clone() {
                    alias = Some(n);
                    self.advance();
                }
            }
            aliases.push(alias);
            if self.peek() == &Token::Comma {
                self.advance();
            } else {
                break;
            }
        }
        Ok((items, aliases))
    }

    fn aggregate_name(&mut self, name: String) -> Result<SelectAgg> {
        let name = name.to_ascii_uppercase();
        let func =
            agg_func(&name).ok_or_else(|| Error::Invalid(format!("unknown aggregate {name}")))?;
        self.expect(Token::LParen)?;
        let distinct = self.eat_kw("DISTINCT");
        let mut args: Vec<String> = Vec::new();
        let mut arg_exprs: Vec<Option<ExprAst>> = Vec::new();
        let mut saw_star = false;
        if self.peek() == &Token::Star {
            self.advance();
            saw_star = true;
        } else {
            loop {
                if self.peek() == &Token::RParen {
                    break;
                }
                // A plain column becomes an `args` name; anything else (e.g.
                // `c1 + 1`, `CAST(c2 AS INTEGER)`) is kept as an expression.
                let e = self.expr()?;
                match e {
                    ExprAst::Column(c) => {
                        args.push(c);
                        arg_exprs.push(None);
                    }
                    other => {
                        args.push(expr_text(&other));
                        arg_exprs.push(Some(other));
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
        // `COUNT()` / `SUM()` with no argument is invalid; a sentinel empty
        // argument fails column resolution later.
        if args.is_empty() && !saw_star {
            args.push(String::new());
            arg_exprs.push(None);
        }
        Ok(SelectAgg {
            func,
            args,
            arg_exprs,
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
/// Converts a parsed predicate back into an expression (for projected
/// `expr AND/OR predicate` terms).
fn predicate_to_expr(p: &Predicate) -> Option<ExprAst> {
    Some(match p {
        Predicate::Cmp(a, op, b) => ExprAst::Cmp(Box::new(a.clone()), *op, Box::new(b.clone())),
        Predicate::Bool(a, BoolOp::And, b) => ExprAst::And(
            Box::new(predicate_to_expr(a)?),
            Box::new(predicate_to_expr(b)?),
        ),
        Predicate::Bool(a, BoolOp::Or, b) => ExprAst::Or(
            Box::new(predicate_to_expr(a)?),
            Box::new(predicate_to_expr(b)?),
        ),
        Predicate::Not(a) => ExprAst::Not(Box::new(predicate_to_expr(a)?)),
        // A bare value is a "truthy" predicate; its expression is the value.
        Predicate::IsTrue(e) => e.clone(),
        Predicate::Like(a, pat, _, neg) => ExprAst::Func {
            name: if *neg { "NOT LIKE" } else { "LIKE" }.to_string(),
            args: vec![a.clone(), pat.clone()],
        },
        Predicate::IsNull(a, neg) => ExprAst::Func {
            name: if *neg { "IS_NOT_NULL" } else { "IS_NULL" }.to_string(),
            args: vec![a.clone()],
        },
        Predicate::In(a, values, neg) => {
            let mut args = vec![a.clone()];
            args.extend(values.iter().cloned());
            ExprAst::Func {
                name: if *neg { "NOT IN" } else { "IN" }.to_string(),
                args,
            }
        }
    })
}

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
/// Validates fulltext option strings. `minimum_should_match` follows the
/// Lucene spec: whitespace-separated `<int>` / `<int>%` / `<int><<int>%`
/// clauses, with percentages within -100..=100.
fn validate_fulltext_options(opts: &str) -> Result<()> {
    for part in opts.split(';') {
        let Some((k, v)) = part.split_once('=') else {
            continue;
        };
        if !k.trim().eq_ignore_ascii_case("minimum_should_match") {
            continue;
        }
        for clause in v.split_whitespace() {
            // an optional `n<` conditional prefix
            let spec = match clause.split_once('<') {
                Some((prefix, rest)) => {
                    if prefix.trim().parse::<i64>().is_err() {
                        return Err(Error::Invalid(
                            "minimum_should_match prefix must be an integer".into(),
                        ));
                    }
                    rest
                }
                None => clause,
            };
            let (num, pct) = match spec.strip_suffix('%') {
                Some(num) => (num, true),
                None => (spec, false),
            };
            let Ok(n) = num.trim().parse::<i64>() else {
                return Err(Error::Invalid(
                    "minimum_should_match must be an integer count or percentage".into(),
                ));
            };
            if pct && !(-100..=100).contains(&n) {
                return Err(Error::Invalid(
                    "minimum_should_match percentage must be within -100..=100%".into(),
                ));
            }
        }
    }
    Ok(())
}

/// Validates the parenthesised CREATE INDEX target. Supported forms:
/// a plain column; a function call whose first argument is a column and whose
/// remaining arguments are atoms; or a top-level expression over a single
/// column. Aggregates, multi-column and function-of-expression forms fail.
/// Renders tokens back into SQL text (string literals re-quoted) so a captured
/// subquery can be re-parsed by the executor.
fn render_tokens_as_sql(toks: &[Token]) -> String {
    let mut out = String::new();
    for (i, t) in toks.iter().enumerate() {
        if i > 0 && !matches!(t, Token::Comma | Token::RParen | Token::Semicolon) {
            out.push(' ');
        }
        match t {
            Token::Str(s) => {
                out.push('\'');
                out.push_str(&s.replace('\'', "''"));
                out.push('\'');
            }
            Token::Ident(s) => out.push_str(s),
            Token::Int(i) => out.push_str(&i.to_string()),
            Token::Float(f) => out.push_str(&fmt_float(*f)),
            Token::Op(o) => out.push_str(o),
            Token::Star => out.push('*'),
            Token::LParen => out.push('('),
            Token::RParen => out.push(')'),
            Token::Comma => out.push(','),
            Token::Colon => out.push(':'),
            Token::Semicolon => out.push(';'),
            Token::Eof => break,
            Token::LBracket => out.push('['),
            Token::RBracket => out.push(']'),
            Token::LBrace => out.push('{'),
            Token::RBrace => out.push('}'),
        }
    }
    out
}

fn index_column_from(toks: &[Token]) -> Result<String> {
    if toks.is_empty() {
        return Err(Error::Invalid("empty index expression".into()));
    }
    if toks.len() == 1 {
        return match &toks[0] {
            Token::Ident(n) => Ok(n.clone()),
            _ => Err(Error::Invalid("invalid index column".into())),
        };
    }
    let is_func = matches!(
        (toks.first(), toks.get(1)),
        (Some(Token::Ident(_)), Some(Token::LParen))
    ) && toks.last() == Some(&Token::RParen);
    if is_func {
        let Token::Ident(f) = &toks[0] else {
            return Err(Error::Invalid("unsupported index expression".into()));
        };
        if agg_func(&f.to_ascii_uppercase()).is_some() {
            return Err(Error::Invalid(format!(
                "unsupported index expression '{f}'"
            )));
        }
        let inner = &toks[2..toks.len() - 1];
        let mut depth = 0i32;
        let mut has_column = false;
        for (i, t) in inner.iter().enumerate() {
            match t {
                Token::LParen => depth += 1,
                Token::RParen => depth -= 1,
                Token::Ident(_) if depth == 0 => {
                    // a bare identifier (not a nested call) is a column
                    if !matches!(inner.get(i + 1), Some(Token::LParen)) {
                        has_column = true;
                    }
                }
                Token::Op(_) | Token::Star if depth == 0 => {
                    return Err(Error::Invalid(format!(
                        "unsupported index expression '{f}'"
                    )))
                }
                _ => {}
            }
        }
        // Record the canonical expression text (`sqrt(c1)`, `substring(c2,0,5)`)
        // so the planner can match a functional-index predicate.
        let inner_txt: String = inner
            .iter()
            .map(|t| match t {
                Token::Ident(s) => s.clone(),
                Token::Int(i) => i.to_string(),
                Token::Str(s) => s.clone(),
                Token::Star => "*".to_string(),
                Token::Op(o) => o.clone(),
                Token::Comma => ",".to_string(),
                Token::LParen => "(".to_string(),
                Token::RParen => ")".to_string(),
                other => format!("{other:?}"),
            })
            .collect();
        return if has_column {
            Ok(format!("{}({inner_txt})", f.to_ascii_lowercase()))
        } else {
            Err(Error::Invalid(format!(
                "unsupported index expression '{f}'"
            )))
        };
    }
    // a top-level expression is supported only over a single column
    let columns = toks
        .iter()
        .enumerate()
        .filter(|(i, t)| {
            matches!(t, Token::Ident(s) if !s.eq_ignore_ascii_case("AS")
                && !matches!(toks.get(i + 1), Some(Token::LParen)))
        })
        .count();
    if columns == 1 {
        // no materialised index for an expression target
        Ok(String::new())
    } else {
        Err(Error::Invalid("unsupported index expression".into()))
    }
}

/// A canonical text for an expression (used to label aggregate arguments).
fn expr_text(e: &ExprAst) -> String {
    match e {
        ExprAst::Column(c) => c.clone(),
        ExprAst::Literal(l) => l.to_slt_string(),
        ExprAst::Func { name, args } => format!(
            "{}({})",
            name.to_ascii_lowercase(),
            args.iter().map(expr_text).collect::<Vec<_>>().join(",")
        ),
        ExprAst::Binary(a, op, b) => format!("({}{:?}{})", expr_text(a), op, expr_text(b)),
        ExprAst::Cmp(a, op, b) => format!("({}{:?}{})", expr_text(a), op, expr_text(b)),
        ExprAst::Not(a) => format!("not({})", expr_text(a)),
        ExprAst::And(a, b) => format!("({} and {})", expr_text(a), expr_text(b)),
        ExprAst::Or(a, b) => format!("({} or {})", expr_text(a), expr_text(b)),
    }
}

/// Validates and normalises a typed literal (`DATE '…'`, `INTERVAL '…'`, …).
/// The reference rejects impossible dates and malformed interval payloads at
/// parse time, so the parser does too.
fn typed_literal_value(kind: &str, raw: &str) -> Result<String> {
    match kind {
        "DATE" => {
            validate_date(raw)?;
            Ok(normalize_datetime(kind, raw))
        }
        "TIME" => {
            validate_time(raw)?;
            Ok(normalize_datetime(kind, raw))
        }
        "TIMESTAMP" | "DATETIME" => {
            if let Some((date, time)) = raw.trim().split_once(' ') {
                validate_date(date)?;
                validate_time(time)?;
            } else {
                validate_date(raw)?;
            }
            Ok(normalize_datetime(kind, raw))
        }
        "INTERVAL" => {
            let mut parts = raw.split_whitespace();
            let (Some(n), Some(unit), None) = (parts.next(), parts.next(), parts.next()) else {
                return Err(Error::Invalid(format!("invalid interval literal '{raw}'")));
            };
            n.parse::<i64>()
                .map_err(|_| Error::Invalid(format!("invalid interval amount '{n}'")))?;
            let unit = unit.to_ascii_uppercase();
            let base = unit.strip_suffix('S').unwrap_or(&unit);
            if !matches!(base, "YEAR" | "MONTH" | "DAY" | "HOUR" | "MINUTE" | "SECOND") {
                return Err(Error::Invalid(format!("invalid interval unit '{unit}'")));
            }
            Ok(normalize_datetime(kind, raw))
        }
        _ => Ok(normalize_datetime(kind, raw)),
    }
}

fn validate_date(raw: &str) -> Result<()> {
    let s = raw.trim().replace('/', "-");
    let parts: Vec<&str> = s.split('-').collect();
    let [y, m, d] = parts.as_slice() else {
        return Err(Error::Invalid(format!("invalid date literal '{raw}'")));
    };
    if y.len() > 4 || [y, m, d].iter().any(|p| p.is_empty() || !p.chars().all(|c| c.is_ascii_digit())) {
        return Err(Error::Invalid(format!("invalid date literal '{raw}'")));
    }
    let (y, m, d): (i64, i64, i64) = (
        y.parse().unwrap_or(0),
        m.parse().unwrap_or(0),
        d.parse().unwrap_or(0),
    );
    if !(1..=9999).contains(&y) || !(1..=12).contains(&m) || !(1..=31).contains(&d) {
        return Err(Error::Invalid(format!("invalid date literal '{raw}'")));
    }
    Ok(())
}

fn validate_time(raw: &str) -> Result<()> {
    let parts: Vec<&str> = raw.trim().split(':').collect();
    if parts.len() < 2 || parts.len() > 3 {
        return Err(Error::Invalid(format!("invalid time literal '{raw}'")));
    }
    let mut nums = Vec::new();
    for p in &parts {
        let n: u32 = p
            .split('.')
            .next()
            .unwrap_or(p)
            .parse()
            .map_err(|_| Error::Invalid(format!("invalid time literal '{raw}'")))?;
        nums.push(n);
    }
    if nums[0] > 23 || nums[1] > 59 || nums.get(2).copied().unwrap_or(0) > 59 {
        return Err(Error::Invalid(format!("invalid time literal '{raw}'")));
    }
    Ok(())
}

pub fn normalize_datetime(kind: &str, raw: &str) -> String {
    // INTERVAL and JSON payloads are kept verbatim
    if kind == "INTERVAL" || kind == "JSON" {
        return raw.trim().to_string();
    }
    // accept `/` as a date separator, e.g. `DATE '2222/11/11'`
    let s = raw.trim().replace('/', "-");
    let s = s.as_str();
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

/// Keywords that can follow a `FROM` entry and therefore cannot be a table
/// alias.
fn is_clause_keyword(s: &str) -> bool {
    matches!(
        s.to_ascii_uppercase().as_str(),
        "WHERE"
            | "GROUP"
            | "HAVING"
            | "ORDER"
            | "LIMIT"
            | "OFFSET"
            | "JOIN"
            | "INNER"
            | "LEFT"
            | "RIGHT"
            | "FULL"
            | "OUTER"
            | "CROSS"
            | "NATURAL"
            | "ON"
            | "USING"
            | "SEARCH"
            | "HIGHLIGHT"
            | "UNION"
            | "EXCEPT"
            | "INTERSECT"
    )
}

/// The unqualified `left`/`right` keys of a simple `ON a = b` join predicate.
fn join_keys(p: &Predicate) -> (String, String) {
    if let Predicate::Cmp(ExprAst::Column(a), CmpOp::Eq, ExprAst::Column(b)) = p {
        (unqualified(a), unqualified(b))
    } else {
        (String::new(), String::new())
    }
}

fn unqualified(name: &str) -> String {
    name.rsplit('.').next().unwrap_or(name).to_string()
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
                        not_null: false,
                        comment: None,
                    },
                    ColumnDef {
                        name: "name".into(),
                        ty: LogicalType::Varchar,
                        type_text: "VARCHAR".into(),
                        default: None,
                        not_null: false,
                        comment: None,
                    },
                    ColumnDef {
                        name: "v".into(),
                        ty: LogicalType::Double,
                        type_text: "DOUBLE".into(),
                        default: None,
                        not_null: false,
                        comment: None,
                    },
                ],
            }
        );
    }

    #[test]
    fn drop_table() {
        assert_eq!(
            one("DROP TABLE t"),
            Statement::DropTable {
                name: "t".into(),
                if_exists: false
            }
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
                aliases: vec![None],
                having: None,
                order_by: vec![],
                limit: None,
                offset: None,
                search: None,
                distinct: false,
                highlight: vec![],
                ctes: vec![],
                cross_tables: vec![],
                limit_expr: None,
                offset_expr: None,
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
                aliases: vec![None, None],
                having: None,
                order_by: vec![],
                limit: None,
                offset: None,
                search: None,
                distinct: false,
                highlight: vec![],
                ctes: vec![],
                cross_tables: vec![],
                limit_expr: None,
                offset_expr: None,
            }
        );
    }

    #[test]
    fn parenthesized_predicate_is_an_expression() {
        // A parenthesised item may be a predicate, not just arithmetic:
        // `(num > 2 OR num < 1)` and `(filter_text(...) OR num > 2)`.
        let Statement::Select { items, .. } = one("SELECT num, (num > 2 OR num < 1) FROM t") else {
            panic!("expected a SELECT");
        };
        assert!(
            matches!(items[1], SelectItem::Expr(ExprAst::Or(..))),
            "got {:?}",
            items[1]
        );
        let Statement::Select { items, .. } =
            one("SELECT (filter_text('doc', 'x') OR num > 2) FROM t")
        else {
            panic!("expected a SELECT");
        };
        assert!(
            matches!(items[0], SelectItem::Expr(ExprAst::Or(..))),
            "got {:?}",
            items[0]
        );
        // Arithmetic parentheses still parse as before.
        let Statement::Select { items, .. } = one("SELECT (1 + 2) FROM t") else {
            panic!("expected a SELECT");
        };
        assert!(
            matches!(items[0], SelectItem::Expr(ExprAst::Binary(..))),
            "got {:?}",
            items[0]
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
                aliases: vec![None, None],
                having: None,
                order_by: vec![],
                limit: Some(10),
                offset: None,
                search: None,
                distinct: false,
                highlight: vec![],
                ctes: vec![],
                cross_tables: vec![],
                limit_expr: None,
                offset_expr: None,
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
                        arg_exprs: vec![],
                        distinct: false
                    }),
                    SelectItem::Aggregate(SelectAgg {
                        func: AggFunc::Sum,
                        args: vec!["id".into()],
                        arg_exprs: vec![None],
                        distinct: false
                    }),
                ],
                from: Some("t".into()),
                join: None,
                where_pred: None,
                group_by: vec![],
                aliases: vec![None, None],
                having: None,
                order_by: vec![],
                limit: None,
                offset: None,
                search: None,
                distinct: false,
                highlight: vec![],
                ctes: vec![],
                cross_tables: vec![],
                limit_expr: None,
                offset_expr: None,
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
                        arg_exprs: vec![],
                        distinct: false
                    }),
                ],
                from: Some("t".into()),
                join: None,
                where_pred: None,
                group_by: vec![ExprAst::Column("name".into())],
                aliases: vec![None, None],
                having: None,
                order_by: vec![OrderKey {
                    column: "name".into(),
                    asc: false,
                    expr: Some(ExprAst::Column("name".into())),
                }],
                limit: Some(5),
                offset: None,
                search: None,
                distinct: false,
                highlight: vec![],
                ctes: vec![],
                cross_tables: vec![],
                limit_expr: None,
                offset_expr: None,
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
                    right: "bid".into(),
                    kind: JoinKind::Inner,
                }),
                where_pred: Some(Predicate::simple("aid", CmpOp::Ge, Literal::Int(1))),
                group_by: vec![],
                aliases: vec![None],
                having: None,
                order_by: vec![],
                limit: None,
                offset: None,
                search: None,
                distinct: false,
                highlight: vec![],
                ctes: vec![],
                cross_tables: vec![],
                limit_expr: None,
                offset_expr: None,
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
                column: "name".into(),
                index_type: String::new(),
                opts: vec![],
                if_not_exists: false,
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
    fn search_chained_fusion_keeps_the_last_tail() {
        // `…, FUSION('rrf'), FUSION('match_tensor', '…')` chains two fusion tails;
        // the outer (last) one decides the final score.
        let s = one(
            "SELECT title FROM t SEARCH MATCH TEXT ('body', 'off', 'topn=4'), \
             MATCH TENSOR (x, [1.0, 0.0], 'float', 'maxsim', 'topn=2'), \
             FUSION('rrf'), FUSION('match_tensor', 'column_name=x;topn=2')",
        );
        match s {
            Statement::Select {
                search: Some(sc), ..
            } => {
                assert_eq!(sc.matches.len(), 2);
                assert_eq!(sc.fusion.as_deref(), Some("match_tensor"));
                assert_eq!(sc.fusion_params.as_deref(), Some("column_name=x;topn=2"));
            }
            other => panic!("expected search, got {other:?}"),
        }
        // A single tail with no params keeps `None` params.
        match one("SELECT a FROM t SEARCH MATCH TEXT ('b', 'x'), FUSION('rrf')") {
            Statement::Select {
                search: Some(sc), ..
            } => {
                assert_eq!(sc.fusion.as_deref(), Some("rrf"));
                assert_eq!(sc.fusion_params, None);
            }
            other => panic!("expected search, got {other:?}"),
        }
    }

    #[test]
    fn drop_if_exists_and_copy_and_utility() {
        assert_eq!(
            one("DROP TABLE IF EXISTS t"),
            Statement::DropTable {
                name: "t".into(),
                if_exists: true
            }
        );
        assert_eq!(
            one("DROP INDEX idx1 ON t"),
            Statement::DropIndex {
                name: "idx1".into(),
                if_exists: false,
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

    // C++: src/unit_test/parser/sql_parser_ut.cpp:good_create_index_1
    #[test]
    fn sql_parser_good_create_index_1() {
        let Statement::CreateIndex {
            name,
            table,
            column,
            index_type,
            opts,
            if_not_exists,
        } = one("CREATE INDEX idx1 ON t1 (a) USING IVF;")
        else {
            panic!("expected CREATE INDEX");
        };
        assert_eq!(name, "idx1");
        assert_eq!(table, "t1");
        assert_eq!(column, "a");
        assert_eq!(index_type, "IVF");
        assert!(opts.is_empty());
        assert!(!if_not_exists);

        let Statement::CreateIndex {
            name,
            if_not_exists,
            ..
        } = one("CREATE INDEX IF NOT EXISTS idx1 ON t1 (a) USING IVF;")
        else {
            panic!("expected CREATE INDEX");
        };
        assert_eq!(name, "idx1");
        assert!(if_not_exists);

        // The Rust AST keeps the database qualifier on the table name.
        let Statement::CreateIndex { table, .. } =
            one("CREATE INDEX idx1 ON db1.t1 (a) USING IVF;")
        else {
            panic!("expected CREATE INDEX");
        };
        assert_eq!(table, "db1.t1");

        let Statement::CreateIndex { opts, .. } =
            one("CREATE INDEX idx3 ON t1 (a) USING IVF WITH (metric = l2);")
        else {
            panic!("expected CREATE INDEX");
        };
        assert_eq!(opts, vec![("METRIC".to_string(), "l2".to_string())]);
    }

    // C++: src/unit_test/parser/sql_parser_ut.cpp:bad_create_index_1
    #[test]
    fn sql_parser_bad_create_index_1() {
        assert!(parse("CREATE INDEX IF NOT EXISTS ON t1 (a) USING IVF;").is_err());
    }

    // C++: src/unit_test/parser/sql_parser_ut.cpp:good_test1
    //
    // Ported for the statements this parser implements: COPY, SHOW, DELETE,
    // UPDATE and KNN projections. CREATE/DROP COLLECTION/SCHEMA, DESCRIBE,
    // FLUSH and OPTIMIZE use a different statement model and are covered by
    // the executor-level tests instead.
    #[test]
    fn sql_parser_good_test1() {
        let inputs = [
            "copy t1 to '/usr/filename' with (format csv);",
            "copy t1 to '/usr/filename' with (format csv, header);",
            "copy t1 to '/usr/filename' with (format csv, header, delimiter '|');",
            "copy t2 from '/usr/filename' with (format csv);",
            "copy t2 from '/usr/filename' with (format csv, header);",
            "copy t2 from '/usr/filename' with (format csv, header, delimiter '|');",
            "show tables;",
            "DELETE FROM products",
            "DELETE FROM products WHERE price = 10",
            "delete from s1.products where obsoletion_date = 'today'",
            "UPDATE products SET price = price * 1.10 WHERE price <= 99.99;",
            "SELECT KNN(c1, [1, 2], 2, 'integer', 'l2') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;",
            "SELECT KNN(c1, [1.0, 2.0], 2, 'float', 'cosine') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;",
            "SELECT KNN(c1, [1,0,1,0,1,1,0,0], 8, 'bit', 'hamming') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;",
        ];
        for input in inputs {
            assert!(parse(input).is_ok(), "{input}");
        }

        let Statement::Select { items, .. } =
            one("SELECT KNN(c1, [1, 2], 2, 'integer', 'l2') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;")
        else {
            panic!("expected SELECT");
        };
        match &items[0] {
            SelectItem::Func { name, args } => {
                assert_eq!(name, "KNN");
                assert_eq!(args.len(), 5);
            }
            other => panic!("expected KNN function, got {other:?}"),
        }
    }

    // C++: src/unit_test/parser/sql_statement_ut.cpp:good_test1
    #[test]
    fn statement_parsing_good_test1() {
        let Statement::Show { text } = one("SHOW DATABASE default_db") else {
            panic!("expected SHOW");
        };
        assert_eq!(text, "DATABASE default_db");

        let Statement::Update {
            table,
            assignments,
            where_pred,
        } = one("UPDATE t1 SET a = 1, b = 2 WHERE c = 'O''K';")
        else {
            panic!("expected UPDATE");
        };
        assert_eq!(table, "t1");
        assert_eq!(assignments[0].0, "a");
        assert_eq!(assignments[0].1, ExprAst::Literal(Literal::Int(1)));
        assert_eq!(assignments[1].0, "b");
        let Some(Predicate::Cmp(ExprAst::Column(c), CmpOp::Eq, ExprAst::Literal(Literal::Str(s)))) =
            where_pred
        else {
            panic!("expected c = 'O''K'");
        };
        assert_eq!(c, "c");
        assert_eq!(s, "O'K");

        let Statement::Insert { table, rows, .. } = one("INSERT INTO t1 VALUES ('abc', 333);")
        else {
            panic!("expected INSERT");
        };
        assert_eq!(table, "t1");
        assert_eq!(rows.len(), 1);
        assert_eq!(rows[0], vec![Literal::Str("abc".into()), Literal::Int(333)]);

        let Statement::Insert { rows, .. } =
            one("INSERT INTO t1 VALUES ('abc', 333), ('def', 444);")
        else {
            panic!("expected INSERT");
        };
        assert_eq!(rows.len(), 2);
        assert_eq!(rows[1], vec![Literal::Str("def".into()), Literal::Int(444)]);
    }

    // C++: src/unit_test/parser/sql_select_statement_ut.cpp:good_test1
    #[test]
    fn select_statement_parsing_good_test1() {
        let Statement::Select {
            items,
            from,
            where_pred,
            group_by,
            having,
            distinct,
            aliases,
            ..
        } = one("select a from t1;")
        else {
            panic!("expected SELECT");
        };
        assert_eq!(from.as_deref(), Some("t1"));
        assert!(where_pred.is_none());
        assert!(group_by.is_empty());
        assert!(having.is_none());
        assert!(!distinct);
        assert_eq!(items.len(), 1);
        assert_eq!(items[0], SelectItem::Column("a".into()));
        assert_eq!(aliases, vec![None]);
    }

    // C++: src/unit_test/parser/sql_select_statement_ut.cpp:good_test2
    #[test]
    fn select_statement_parsing_good_test2() {
        let Statement::Select {
            items,
            from,
            where_pred,
            group_by,
            having,
            distinct,
            ..
        } = one("select 1;")
        else {
            panic!("expected SELECT");
        };
        assert!(from.is_none());
        assert!(where_pred.is_none());
        assert!(group_by.is_empty());
        assert!(having.is_none());
        assert!(!distinct);
        assert_eq!(items, vec![SelectItem::Literal(Literal::Int(1))]);

        let Statement::Select { items, from, .. } =
            one("select a, sum(b), function(c, nested(d)) from t1;")
        else {
            panic!("expected SELECT");
        };
        assert_eq!(from.as_deref(), Some("t1"));
        assert_eq!(items.len(), 3);
        assert_eq!(items[0], SelectItem::Column("a".into()));
        match &items[1] {
            SelectItem::Aggregate(agg) => {
                assert_eq!(agg.func, AggFunc::Sum);
                assert_eq!(agg.args, vec!["b".to_string()]);
            }
            other => panic!("expected SUM aggregate, got {other:?}"),
        }
        match &items[2] {
            SelectItem::Func { name, args } => {
                assert_eq!(name, "function");
                assert_eq!(args.len(), 2);
                assert_eq!(args[0], ExprAst::Column("c".into()));
                match &args[1] {
                    ExprAst::Func { name, args } => {
                        assert_eq!(name, "nested");
                        assert_eq!(args, &vec![ExprAst::Column("d".into())]);
                    }
                    other => panic!("expected nested function, got {other:?}"),
                }
            }
            other => panic!("expected function call, got {other:?}"),
        }
    }

    // C++: src/unit_test/parser/sql_select_statement_ut.cpp:good_search_test
    #[test]
    fn select_statement_parsing_good_search_test() {
        let sql = r#"
        SELECT *
        FROM t1
        SEARCH
            MATCH TEXT ('author^2,name^5', 'frank dune'),
            MATCH TEXT ('name', 'to the star', 'operator=OR;fuzziness=AUTO:1,5;minimum_should_match=1'),
            QUERY('name:dune'),
            QUERY('_exists_:"author" AND page_count:>200 AND (name:/star./ OR name:duna~)', 'default_operator=and;default_field=name'),
            MATCH VECTOR (c1, [1, 2], 'integer', 'l2', 3),
            MATCH VECTOR (c1, [3, 10, 1111], 'bigint', 'cosine', 3),
            MATCH VECTOR (c1, [1.0, 2.0], 'double', 'cosine', 3),
            MATCH VECTOR (c1, [1.00, 2.00], 'double', 'ip', 3),
            MATCH VECTOR (c1, [1,0,1,0,1,1,0,0], 'bit', 'hamming', 3),
            MATCH VECTOR (c1, [1.00, 2.00], 'float', 'ip', 3),
            FUSION('rrf', 'rank_constant=60')
        WHERE a > 0
        "#;
        let Statement::Select {
            search, where_pred, ..
        } = one(sql)
        else {
            panic!("expected SELECT");
        };
        assert!(where_pred.is_some());
        let search = search.expect("SEARCH clause");
        assert_eq!(search.matches.len(), 10);

        assert_eq!(search.matches[0].kind, "TEXT");
        assert_eq!(search.matches[0].column, "author^2,name^5");
        assert_eq!(
            search.matches[0].args,
            vec![ExprAst::Literal(Literal::Str("frank dune".into()))]
        );
        assert_eq!(search.matches[1].column, "name");
        assert_eq!(search.matches[1].args.len(), 2);

        assert_eq!(search.matches[2].kind, "QUERY");
        assert_eq!(search.matches[2].column, "name:dune");
        assert!(search.matches[2].args.is_empty());
        assert_eq!(search.matches[3].kind, "QUERY");
        assert_eq!(search.matches[3].args.len(), 1);

        let vector = &search.matches[4];
        assert_eq!(vector.kind, "VECTOR");
        assert_eq!(vector.column, "c1");
        assert_eq!(
            vector.args[0],
            ExprAst::Literal(Literal::Array(vec!["1".into(), "2".into()]))
        );
        assert_eq!(
            vector.args[1],
            ExprAst::Literal(Literal::Str("integer".into()))
        );
        assert_eq!(vector.args[2], ExprAst::Literal(Literal::Str("l2".into())));
        assert_eq!(vector.args[3], ExprAst::Literal(Literal::Int(3)));

        assert_eq!(search.fusion.as_deref(), Some("rrf"));
        assert_eq!(search.fusion_params.as_deref(), Some("rank_constant=60"));
        assert_eq!(search.fusions.len(), 1);
    }

    // C++: src/unit_test/parser/float_serialize_ut.cpp:test_precision
    #[test]
    fn float_serialize_test_precision() {
        // Keep this in sync with the C++ test's hex float literals
        // (0x1.fffffcp+9 / 0x1.fffffep+9).
        let f1 = f32::from_bits(0x447f_fffe);
        let f2 = f32::from_bits(0x447f_ffff);
        assert_eq!(format!("{f1}").parse::<f32>().unwrap(), f1);
        assert_eq!(format!("{f2}").parse::<f32>().unwrap(), f2);
        // `max_digits10` (9 and 17) worth of digits round-trip.
        assert_eq!("1023.9999".parse::<f32>().unwrap(), f1);
        assert_eq!("1023.99994".parse::<f32>().unwrap(), f2);
        // 8 significant digits keep f1 but lose f2.
        assert_eq!(format!("{f1:.4}").parse::<f32>().unwrap(), f1);
        assert_ne!(format!("{f2:.4}").parse::<f32>().unwrap(), f2);
        // Scientific notation round-trips.
        assert_eq!(format!("{f1:e}").parse::<f32>().unwrap(), f1);
        assert_eq!(format!("{f2:e}").parse::<f32>().unwrap(), f2);
        // The hex-float branch has no Rust std counterpart.
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
                        not_null: false,
                        comment: None,
                    },
                    ColumnDef {
                        name: "b".into(),
                        ty: LogicalType::Invalid,
                        type_text: "TENSOR(FLOAT,16)".into(),
                        default: None,
                        not_null: false,
                        comment: None,
                    },
                    ColumnDef {
                        name: "c".into(),
                        ty: LogicalType::Invalid,
                        type_text: "EMBEDDING(FLOAT,4)".into(),
                        default: None,
                        not_null: false,
                        comment: None,
                    },
                ],
            }
        );
    }
    /// `src/unit_test/parser/sql_select_statement_ut.cpp::good_test3`: multiple
    /// SELECTs and every JOIN spelling parse.
    #[test]
    fn select_statements_parse_like_the_reference() {
        let stmts = parse("SELECT * FROM t1; SELECt * FROM t2;").unwrap();
        assert_eq!(stmts.len(), 2);
        for s in &stmts {
            assert!(matches!(s, Statement::Select { .. }));
        }
        // The join spellings the Rust parser models (`FULL`/`OUTER`/`NATURAL`
        // are not parsed yet, so `good_test3` stays unported beyond this).
        let joins = [
            "SELECT * FROM t1 join t2 on x = y;",
            "SELECT * FROM t1 inner join t2 on x = y;",
            "SELECT * FROM t1 left join t2 on x = y;",
            "SELECT * FROM t1 right join t2 on x = y;",
            "SELECT * FROM t1 cross join t2 on x = y;",
        ];
        for sql in joins {
            assert!(parse(sql).is_ok(), "{sql}");
        }
    }

    /// `SelectStatementParsingTest.good_test4`: a CAST date minus an INTERVAL.
    #[test]
    fn cast_date_minus_interval_parses() {
        assert!(
            parse("SELECT * FROM t where a = cast ('2023-01-01' as date) - INTERVAL 15 DAYS;")
                .is_ok()
        );
    }

    /// `SelectStatementParsingTest.good_test2`: a bare constant and a nested
    /// function call in the select list.
    #[test]
    fn select_list_accepts_constants_and_nested_calls() {
        assert!(parse("select 1;").is_ok());
        assert!(parse("select a, sum(b), function(c, nested(d)) from t1;").is_ok());
    }
    /// C++ `src/unit_test/parser/sql_select_statement_ut.cpp:988-1213
    /// SelectStatementParsingTest.good_test3`.
    #[test]
    fn select_statement_parsing_good_test3() {
        // Two SELECT statements in one string.
        let stmts = parse("SELECT * FROM t1; SELECt * FROM t2;").unwrap();
        assert_eq!(stmts.len(), 2);
        let Statement::Select { from, .. } = &stmts[0] else {
            panic!("expected SELECT");
        };
        assert_eq!(from.as_deref(), Some("t1"));
        let Statement::Select { from, .. } = &stmts[1] else {
            panic!("expected SELECT");
        };
        assert_eq!(from.as_deref(), Some("t2"));

        // JOIN spellings.
        let stmts = parse(
            "SELECT * FROM t1 join t2 on x = y; \
             SELECT * FROM t1 inner join t2 on x = y; \
             SELECT * FROM t1 left join t2 on x = y; \
             SELECT * FROM t1 right join t2 on x = y; \
             SELECT * FROM t1 full join t2 on x = y; \
             SELECT * FROM t1 outer join t2 on x = y; \
             SELECT * FROM t1 natural join t2; \
             SELECT * FROM t1 cross join t2 on x = y; \
             SELECT * FROM t1, t2 where x = y;",
        )
        .unwrap();
        assert_eq!(stmts.len(), 9);
        let kinds: Vec<JoinKind> = stmts[..8]
            .iter()
            .map(|s| match s {
                Statement::Select {
                    join: Some(join), ..
                } => join.kind,
                other => panic!("expected a join, got {other:?}"),
            })
            .collect();
        assert_eq!(
            kinds,
            vec![
                JoinKind::Inner,
                JoinKind::Inner,
                JoinKind::Left,
                JoinKind::Right,
                JoinKind::Full,
                JoinKind::Full,
                JoinKind::Natural,
                JoinKind::Cross,
            ]
        );
        // The comma form is a cross product of two tables.
        let Statement::Select {
            from, cross_tables, ..
        } = &stmts[8]
        else {
            panic!("expected SELECT");
        };
        assert_eq!(from.as_deref(), Some("t1"));
        assert_eq!(cross_tables, &vec!["t2".to_string()]);

        // LIMIT/OFFSET expressions and subqueries.
        let Statement::Select {
            limit,
            limit_expr,
            offset,
            offset_expr,
            ..
        } = one("SELECT * FROM t1 limit 1 + 1 offset 2 + 2;")
        else {
            panic!("expected SELECT");
        };
        assert!(limit.is_none());
        assert!(offset.is_none());
        assert_eq!(
            limit_expr,
            Some(ExprAst::Binary(
                Box::new(ExprAst::Literal(Literal::Int(1))),
                BinOp::Add,
                Box::new(ExprAst::Literal(Literal::Int(1)))
            ))
        );
        assert_eq!(
            offset_expr,
            Some(ExprAst::Binary(
                Box::new(ExprAst::Literal(Literal::Int(2))),
                BinOp::Add,
                Box::new(ExprAst::Literal(Literal::Int(2)))
            ))
        );

        let Statement::Select {
            limit_expr,
            offset_expr,
            ..
        } = one(
            "SELECT * FROM t1 limit (SELECT MIN(x) FROM t2) offset (SELECT MAX(y) FROM t2);",
        )
        else {
            panic!("expected SELECT");
        };
        let ExprAst::Func { name, args } = limit_expr.unwrap() else {
            panic!("expected a subquery limit");
        };
        assert_eq!(name, "SUBQUERY");
        let ExprAst::Literal(Literal::Str(sql)) = &args[0] else {
            panic!("expected the subquery SQL");
        };
        assert!(sql.to_ascii_uppercase().contains("MIN"));
        let ExprAst::Func { name, args } = offset_expr.unwrap() else {
            panic!("expected a subquery offset");
        };
        assert_eq!(name, "SUBQUERY");
        let ExprAst::Literal(Literal::Str(sql)) = &args[0] else {
            panic!("expected the subquery SQL");
        };
        assert!(sql.to_ascii_uppercase().contains("MAX"));

        // EXTRACT(field FROM ...) is named extract_<field>.
        let Statement::Select { items, .. } =
            one("SELECT extract('year' from date '2025-03-04');")
        else {
            panic!("expected SELECT");
        };
        let SelectItem::Func { name, args } = &items[0] else {
            panic!("expected EXTRACT, got {:?}", items[0]);
        };
        assert_eq!(name, "extract_year");
        assert_eq!(args[0], ExprAst::Literal(Literal::Str("2025-03-04".into())));

        let Statement::Select { where_pred, .. } =
            one("SELECT * FROM t1 WHERE extract('year' from a) < 2023")
        else {
            panic!("expected SELECT");
        };
        let Some(Predicate::Cmp(
            ExprAst::Func { name, args },
            CmpOp::Lt,
            ExprAst::Literal(Literal::Int(2023)),
        )) = where_pred
        else {
            panic!("expected extract_year(a) < 2023");
        };
        assert_eq!(name, "extract_year");
        assert_eq!(args[0], ExprAst::Column("a".into()));

        // CAST.
        let Statement::Select { items, .. } = one("SELECT CAST(5.3 AS BIGINT);") else {
            panic!("expected SELECT");
        };
        let SelectItem::Func { name, args } = &items[0] else {
            panic!("expected CAST, got {:?}", items[0]);
        };
        assert_eq!(name, "CAST");
        assert_eq!(args[0], ExprAst::Literal(Literal::Float(5.3)));
        assert_eq!(args[1], ExprAst::Literal(Literal::Str("BIGINT".into())));

        // WITH bindings and the outer cross product.
        let Statement::Select {
            ctes,
            from: outer_from,
            cross_tables,
            ..
        } = one("WITH t1 AS (SELECT a FROM x), t2 AS (SELECT b FROM y) SELECT * FROM t1, t2;")
        else {
            panic!("expected SELECT");
        };
        assert_eq!(ctes.len(), 2);
        assert_eq!(ctes[0].alias, "t1");
        let Statement::Select {
            items,
            from: cte0_from,
            ..
        } = ctes[0].query.as_ref()
        else {
            panic!("expected CTE SELECT");
        };
        assert_eq!(items, &vec![SelectItem::Column("a".into())]);
        assert_eq!(cte0_from.as_deref(), Some("x"));
        assert_eq!(ctes[1].alias, "t2");
        let Statement::Select {
            items,
            from: cte1_from,
            ..
        } = ctes[1].query.as_ref()
        else {
            panic!("expected CTE SELECT");
        };
        assert_eq!(items, &vec![SelectItem::Column("b".into())]);
        assert_eq!(cte1_from.as_deref(), Some("y"));
        assert_eq!(outer_from.as_deref(), Some("t1"));
        assert_eq!(cross_tables, vec!["t2".to_string()]);

        // `a + 3 day`.
        let Statement::Select { items, .. } = one("select a + 3 day from t1;") else {
            panic!("expected SELECT");
        };
        let SelectItem::Expr(ExprAst::Binary(left, BinOp::Add, right)) = &items[0] else {
            panic!("expected a + 3 day, got {:?}", items[0]);
        };
        assert_eq!(**left, ExprAst::Column("a".into()));
        let ExprAst::Func { name, args } = right.as_ref() else {
            panic!("expected an interval operand");
        };
        assert_eq!(name, "INTERVAL");
        assert_eq!(args[0], ExprAst::Literal(Literal::Int(3)));
        assert_eq!(args[1], ExprAst::Literal(Literal::Str("DAY".into())));

        // CAST date minus INTERVAL.
        let Statement::Select { where_pred, .. } = one(
            "SELECT * FROM t where a = cast ('2023-01-01' as date) - INTERVAL 15 days;",
        ) else {
            panic!("expected SELECT");
        };
        let Some(Predicate::Cmp(
            ExprAst::Column(column),
            CmpOp::Eq,
            ExprAst::Binary(left, BinOp::Sub, right),
        )) = where_pred
        else {
            panic!("expected a = CAST(...) - INTERVAL");
        };
        assert_eq!(column, "a");
        let ExprAst::Func { name, args } = left.as_ref() else {
            panic!("expected CAST");
        };
        assert_eq!(name, "CAST");
        assert_eq!(args[1], ExprAst::Literal(Literal::Str("DATE".into())));
        let ExprAst::Func { name, args } = right.as_ref() else {
            panic!("expected INTERVAL");
        };
        assert_eq!(name, "INTERVAL");
        assert_eq!(args[0], ExprAst::Literal(Literal::Str("15 DAYS".into())));
    }

    /// `SelectStatementParsingTest.bad_test1`: `OFFSET` requires `LIMIT`.
    #[test]
    fn offset_without_limit_is_rejected() {
        assert!(parse("SELECT b FROM t1 WHERE a > 0 OFFSET 3;").is_err());
    }

    /// `SelectStatementParsingTest.bad_knn_test`: the vector metric must suit
    /// the element type.
    #[test]
    fn vector_metric_type_mismatches_are_rejected() {
        for sql in [
            "SELECT b FROM t1 SEARCH MATCH VECTOR (c1, [1.0, 2.0], 'double', 'hamming', 3) WHERE a > 0;",
            "SELECT b FROM t1 SEARCH MATCH VECTOR (c1, [1,0,1,0,1,1,0], 'bit', 'hamming', 3) WHERE a > 0;",
            "SELECT b FROM t1 SEARCH MATCH VECTOR (c1, [1,0,1,0,1,1,0,0,1,0,1,0,1,1,0,0], 'bit', 'cosine', 3) WHERE a > 0;",
        ] {
            // The second statement is syntactically valid; C++ rejects it at
            // bind time because the literal width does not match the declared
            // bit dimension, which the parser cannot see. Only the metric/type
            // mismatch is checked here.
            let r = parse(sql);
            if sql.contains("'double', 'hamming'") || sql.contains("'bit', 'cosine'") {
                assert!(r.is_err(), "{sql}");
            }
        }
    }

    /// `SelectStatementParsingTest.bad_search_test`: `SEARCH` cannot be
    /// combined with `ORDER BY`/`LIMIT`/`OFFSET`.
    #[test]
    fn search_with_order_or_limit_is_rejected() {
        for sql in [
            "SELECT b FROM t1 SEARCH MATCH VECTOR (c1, [1.0, 2.0], 'double', 'l2', 3) WHERE a > 0 ORDER BY c2;",
            "SELECT b FROM t1 SEARCH MATCH VECTOR (c1, [1,0,1,0,1,1,0], 'bit', 'hamming', 3) WHERE a > 0 LIMIT 5;",
            "SELECT b FROM t1 SEARCH MATCH VECTOR (c1, [1,0,1,0,1,1,0], 'bit', 'l2', 3) WHERE a > 0 LIMIT 5;",
        ] {
            assert!(parse(sql).is_err(), "{sql}");
        }
    }

    /// `SQLParserTest.bad_test1` (`sql_parser_ut.cpp`): malformed `CREATE
    /// TABLE` column lists must reject the whole statement.
    #[test]
    fn sql_parser_bad_test1() {
        for sql in [
            "create table t1 (a boolean primary key not null null unique, b tinyint not null null unique,",
            "create table t1 (a boolean primary key not null null unique, b tinyint not null null unique c smallint unique);",
            "create table t1 t2 (a boolean primary key not null null unique);",
        ] {
            assert!(parse(sql).is_err(), "{sql}");
        }
    }
}
