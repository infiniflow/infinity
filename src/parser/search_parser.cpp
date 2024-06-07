// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "search_parser.h"


// Unqualified %code blocks.
#line 42 "search_parser.y"

    #include "search_driver.h"
    #include "search_scanner.h"

    #undef yylex
    #define yylex scanner.yylex

    #include <iostream>
    #include <sstream>
    #include <string>
    #include <vector>
    #include "query_node.h"

    // Avoid warnings with the error counter.
    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
    #endif

#line 65 "search_parser.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 10 "search_parser.y"
namespace infinity {
#line 158 "search_parser.cpp"

  /// Build a parser object.
  SearchParser::SearchParser (SearchScanner &scanner_yyarg, const SearchDriver &driver_yyarg, const std::string &default_field_yyarg, std::unique_ptr<QueryNode> &parse_result_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      scanner (scanner_yyarg),
      driver (driver_yyarg),
      default_field (default_field_yyarg),
      parse_result (parse_result_yyarg)
  {}

  SearchParser::~SearchParser ()
  {}

  SearchParser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/

  // basic_symbol.
  template <typename Base>
  SearchParser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->kind ())
    {
      case symbol_kind::S_STRING: // STRING
        value.copy< InfString > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_CARAT: // CARAT
        value.copy< float > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_topLevelQuery: // topLevelQuery
      case symbol_kind::S_query: // query
      case symbol_kind::S_clause: // clause
      case symbol_kind::S_term: // term
      case symbol_kind::S_basic_filter_boost: // basic_filter_boost
      case symbol_kind::S_basic_filter: // basic_filter
        value.copy< std::unique_ptr<QueryNode> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  SearchParser::symbol_kind_type
  SearchParser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  SearchParser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  SearchParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_STRING: // STRING
        value.move< InfString > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_CARAT: // CARAT
        value.move< float > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_topLevelQuery: // topLevelQuery
      case symbol_kind::S_query: // query
      case symbol_kind::S_clause: // clause
      case symbol_kind::S_term: // term
      case symbol_kind::S_basic_filter_boost: // basic_filter_boost
      case symbol_kind::S_basic_filter: // basic_filter
        value.move< std::unique_ptr<QueryNode> > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_kind.
  SearchParser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  SearchParser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  SearchParser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  SearchParser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  void
  SearchParser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  SearchParser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  SearchParser::symbol_kind_type
  SearchParser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  SearchParser::symbol_kind_type
  SearchParser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }



  // by_state.
  SearchParser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  SearchParser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  SearchParser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  SearchParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  SearchParser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  SearchParser::symbol_kind_type
  SearchParser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  SearchParser::stack_symbol_type::stack_symbol_type ()
  {}

  SearchParser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_STRING: // STRING
        value.YY_MOVE_OR_COPY< InfString > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_CARAT: // CARAT
        value.YY_MOVE_OR_COPY< float > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_topLevelQuery: // topLevelQuery
      case symbol_kind::S_query: // query
      case symbol_kind::S_clause: // clause
      case symbol_kind::S_term: // term
      case symbol_kind::S_basic_filter_boost: // basic_filter_boost
      case symbol_kind::S_basic_filter: // basic_filter
        value.YY_MOVE_OR_COPY< std::unique_ptr<QueryNode> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  SearchParser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_STRING: // STRING
        value.move< InfString > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_CARAT: // CARAT
        value.move< float > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_topLevelQuery: // topLevelQuery
      case symbol_kind::S_query: // query
      case symbol_kind::S_clause: // clause
      case symbol_kind::S_term: // term
      case symbol_kind::S_basic_filter_boost: // basic_filter_boost
      case symbol_kind::S_basic_filter: // basic_filter
        value.move< std::unique_ptr<QueryNode> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  SearchParser::stack_symbol_type&
  SearchParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_STRING: // STRING
        value.copy< InfString > (that.value);
        break;

      case symbol_kind::S_CARAT: // CARAT
        value.copy< float > (that.value);
        break;

      case symbol_kind::S_topLevelQuery: // topLevelQuery
      case symbol_kind::S_query: // query
      case symbol_kind::S_clause: // clause
      case symbol_kind::S_term: // term
      case symbol_kind::S_basic_filter_boost: // basic_filter_boost
      case symbol_kind::S_basic_filter: // basic_filter
        value.copy< std::unique_ptr<QueryNode> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  SearchParser::stack_symbol_type&
  SearchParser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_STRING: // STRING
        value.move< InfString > (that.value);
        break;

      case symbol_kind::S_CARAT: // CARAT
        value.move< float > (that.value);
        break;

      case symbol_kind::S_topLevelQuery: // topLevelQuery
      case symbol_kind::S_query: // query
      case symbol_kind::S_clause: // clause
      case symbol_kind::S_term: // term
      case symbol_kind::S_basic_filter_boost: // basic_filter_boost
      case symbol_kind::S_basic_filter: // basic_filter
        value.move< std::unique_ptr<QueryNode> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  SearchParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  SearchParser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  SearchParser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  SearchParser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  SearchParser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  SearchParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  SearchParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  SearchParser::debug_level_type
  SearchParser::debug_level () const
  {
    return yydebug_;
  }

  void
  SearchParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  SearchParser::state_type
  SearchParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  SearchParser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  SearchParser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  SearchParser::operator() ()
  {
    return parse ();
  }

  int
  SearchParser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_STRING: // STRING
        yylhs.value.emplace< InfString > ();
        break;

      case symbol_kind::S_CARAT: // CARAT
        yylhs.value.emplace< float > ();
        break;

      case symbol_kind::S_topLevelQuery: // topLevelQuery
      case symbol_kind::S_query: // query
      case symbol_kind::S_clause: // clause
      case symbol_kind::S_term: // term
      case symbol_kind::S_basic_filter_boost: // basic_filter_boost
      case symbol_kind::S_basic_filter: // basic_filter
        yylhs.value.emplace< std::unique_ptr<QueryNode> > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // topLevelQuery: query "end of file"
#line 84 "search_parser.y"
            {
    parse_result = std::move(yystack_[1].value.as < std::unique_ptr<QueryNode> > ());
}
#line 771 "search_parser.cpp"
    break;

  case 3: // query: clause
#line 89 "search_parser.y"
         { yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ()); }
#line 777 "search_parser.cpp"
    break;

  case 4: // query: query clause
#line 90 "search_parser.y"
               {
    auto query = std::make_unique<OrQueryNode>();
    query->Add(std::move(yystack_[1].value.as < std::unique_ptr<QueryNode> > ()));
    query->Add(std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ()));
    yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(query);
}
#line 788 "search_parser.cpp"
    break;

  case 5: // query: query OR clause
#line 96 "search_parser.y"
                  {
    auto query = std::make_unique<OrQueryNode>();
    query->Add(std::move(yystack_[2].value.as < std::unique_ptr<QueryNode> > ()));
    query->Add(std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ()));
    yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(query);
}
#line 799 "search_parser.cpp"
    break;

  case 6: // clause: term
#line 104 "search_parser.y"
       { yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ()); }
#line 805 "search_parser.cpp"
    break;

  case 7: // clause: clause AND term
#line 105 "search_parser.y"
                  {
    auto query = std::make_unique<AndQueryNode>();
    query->Add(std::move(yystack_[2].value.as < std::unique_ptr<QueryNode> > ()));
    query->Add(std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ()));
    yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(query);
}
#line 816 "search_parser.cpp"
    break;

  case 8: // term: basic_filter_boost
#line 113 "search_parser.y"
                     { yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ()); }
#line 822 "search_parser.cpp"
    break;

  case 9: // term: NOT term
#line 114 "search_parser.y"
           {
    auto query = std::make_unique<NotQueryNode>();
    query->Add(std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ()));
    yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(query);
}
#line 832 "search_parser.cpp"
    break;

  case 10: // term: LPAREN query RPAREN
#line 119 "search_parser.y"
                      { yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[1].value.as < std::unique_ptr<QueryNode> > ()); }
#line 838 "search_parser.cpp"
    break;

  case 11: // term: LPAREN query RPAREN CARAT
#line 120 "search_parser.y"
                            {
    yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[2].value.as < std::unique_ptr<QueryNode> > ());
    yylhs.value.as < std::unique_ptr<QueryNode> > ()->MultiplyWeight(yystack_[0].value.as < float > ());
}
#line 847 "search_parser.cpp"
    break;

  case 12: // basic_filter_boost: basic_filter
#line 126 "search_parser.y"
               {
    yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ());
}
#line 855 "search_parser.cpp"
    break;

  case 13: // basic_filter_boost: basic_filter CARAT
#line 129 "search_parser.y"
                     {
    yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[1].value.as < std::unique_ptr<QueryNode> > ());
    yylhs.value.as < std::unique_ptr<QueryNode> > ()->MultiplyWeight(yystack_[0].value.as < float > ());
}
#line 864 "search_parser.cpp"
    break;

  case 14: // basic_filter: STRING
#line 135 "search_parser.y"
         {
    const std::string &field = default_field;
    if(field.empty()){
        error(yystack_[0].location, "default_field is empty");
        YYERROR;
    }
    std::string text = SearchDriver::Unescape(yystack_[0].value.as < InfString > ().text_);
    yylhs.value.as < std::unique_ptr<QueryNode> > () = driver.AnalyzeAndBuildQueryNode(field, std::move(text), yystack_[0].value.as < InfString > ().from_quoted_);
}
#line 878 "search_parser.cpp"
    break;

  case 15: // basic_filter: STRING OP_COLON STRING
#line 144 "search_parser.y"
                         {
    std::string field = SearchDriver::Unescape(yystack_[2].value.as < InfString > ().text_);
    std::string text = SearchDriver::Unescape(yystack_[0].value.as < InfString > ().text_);
    yylhs.value.as < std::unique_ptr<QueryNode> > () = driver.AnalyzeAndBuildQueryNode(std::move(field), std::move(text), yystack_[0].value.as < InfString > ().from_quoted_);
}
#line 888 "search_parser.cpp"
    break;


#line 892 "search_parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  SearchParser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  SearchParser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  SearchParser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // SearchParser::context.
  SearchParser::context::context (const SearchParser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  SearchParser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
  SearchParser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  SearchParser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char SearchParser::yypact_ninf_ = -7;

  const signed char SearchParser::yytable_ninf_ = -1;

  const signed char
  SearchParser::yypact_[] =
  {
       4,     4,     4,    -5,    15,     1,    10,    -7,    -7,    12,
      -7,    13,    -6,    -7,    -7,     4,    10,     4,    -7,    16,
      -7,    10,    -7,    -7
  };

  const signed char
  SearchParser::yydefact_[] =
  {
       0,     0,     0,    14,     0,     0,     3,     6,     8,    12,
       9,     0,     0,     1,     2,     0,     4,     0,    13,    10,
      15,     5,     7,    11
  };

  const signed char
  SearchParser::yypgoto_[] =
  {
      -7,    -7,    20,    -3,    -1,    -7,    -7
  };

  const signed char
  SearchParser::yydefgoto_[] =
  {
       0,     4,     5,     6,     7,     8,     9
  };

  const signed char
  SearchParser::yytable_[] =
  {
      10,    14,    16,    12,    20,    15,     1,     2,    16,     1,
       2,     3,    21,    17,     3,    13,    22,    15,     1,     2,
      19,    18,    11,     3,     0,    23
  };

  const signed char
  SearchParser::yycheck_[] =
  {
       1,     0,     5,     8,    10,     4,     5,     6,    11,     5,
       6,    10,    15,     3,    10,     0,    17,     4,     5,     6,
       7,     9,     2,    10,    -1,     9
  };

  const signed char
  SearchParser::yystos_[] =
  {
       0,     5,     6,    10,    12,    13,    14,    15,    16,    17,
      15,    13,     8,     0,     0,     4,    14,     3,     9,     7,
      10,    14,    15,     9
  };

  const signed char
  SearchParser::yyr1_[] =
  {
       0,    11,    12,    13,    13,    13,    14,    14,    15,    15,
      15,    15,    16,    16,    17,    17
  };

  const signed char
  SearchParser::yyr2_[] =
  {
       0,     2,     2,     1,     2,     3,     1,     3,     1,     2,
       3,     4,     1,     2,     1,     3
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const SearchParser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "AND", "OR", "NOT",
  "LPAREN", "RPAREN", "OP_COLON", "CARAT", "STRING", "$accept",
  "topLevelQuery", "query", "clause", "term", "basic_filter_boost",
  "basic_filter", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const unsigned char
  SearchParser::yyrline_[] =
  {
       0,    84,    84,    89,    90,    96,   104,   105,   113,   114,
     119,   120,   126,   129,   135,   144
  };

  void
  SearchParser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  SearchParser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  SearchParser::symbol_kind_type
  SearchParser::yytranslate_ (int t) YY_NOEXCEPT
  {
    return static_cast<symbol_kind_type> (t);
  }

#line 10 "search_parser.y"
} // infinity
#line 1372 "search_parser.cpp"

#line 150 "search_parser.y"


namespace infinity{
void SearchParser::error(const location_type &l, const std::string &err_message) {
    std::cerr << "Error: " << err_message << " at " << l << "\n";
}
} //namespace infinity
