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





#include "query_parser.h"


// Unqualified %code blocks.
#line 30 "query_parser.y"

    #include "query_driver.h"
    #include "query_scanner.h"

    #undef yylex
    #define yylex scanner.yylex

    #include <iostream>
    #include <sstream>
    #include <string>
    #include "search/all_filter.hpp"
    #include "search/boolean_filter.hpp"
    #include "search/column_existence_filter.hpp"
    #include "search/granular_range_filter.hpp"
    #include "search/levenshtein_filter.hpp"
    #include "search/nested_filter.hpp"
    #include "search/ngram_similarity_filter.hpp"
    #include "search/phrase_filter.hpp"
    #include "search/prefix_filter.hpp"
    #include "search/proxy_filter.hpp"
    #include "search/range_filter.hpp"
    #include "search/same_position_filter.hpp"
    #include "search/term_filter.hpp"
    #include "search/terms_filter.hpp"
    #include "search/wildcard_filter.hpp"

    // Convert basic_string<char> to basic_string<unsigned char>
    static irs::bstring toBstring(const std::string &str) {
        irs::bstring converted_str((uint8_t *)str.c_str(), str.length());
        return converted_str;
    }

#line 79 "query_parser.cpp"


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

#line 9 "query_parser.y"
namespace infinity {
#line 172 "query_parser.cpp"

  /// Build a parser object.
  QueryParser::QueryParser (QueryScanner &scanner_yyarg, QueryDriver &driver_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      scanner (scanner_yyarg),
      driver (driver_yyarg)
  {}

  QueryParser::~QueryParser ()
  {}

  QueryParser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/

  // basic_symbol.
  template <typename Base>
  QueryParser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->kind ())
    {
      case symbol_kind::S_CARAT: // CARAT
        value.copy< float > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_TILDE: // TILDE
      case symbol_kind::S_opCompare: // opCompare
      case symbol_kind::S_rangeStart: // rangeStart
      case symbol_kind::S_rangeEnd: // rangeEnd
        value.copy< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_REGEXPTERM: // REGEXPTERM
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_boundVal: // boundVal
        value.copy< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_topLevelQuery: // topLevelQuery
      case symbol_kind::S_query: // query
      case symbol_kind::S_clause: // clause
      case symbol_kind::S_term: // term
      case symbol_kind::S_basic_filter_boost: // basic_filter_boost
      case symbol_kind::S_basic_filter: // basic_filter
      case symbol_kind::S_rangeExpr: // rangeExpr
      case symbol_kind::S_regexExpr: // regexExpr
      case symbol_kind::S_fuzzyExpr: // fuzzyExpr
        value.copy< std::unique_ptr<irs::filter> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  QueryParser::symbol_kind_type
  QueryParser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  QueryParser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  QueryParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_CARAT: // CARAT
        value.move< float > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_TILDE: // TILDE
      case symbol_kind::S_opCompare: // opCompare
      case symbol_kind::S_rangeStart: // rangeStart
      case symbol_kind::S_rangeEnd: // rangeEnd
        value.move< int > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_REGEXPTERM: // REGEXPTERM
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_boundVal: // boundVal
        value.move< std::string > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_topLevelQuery: // topLevelQuery
      case symbol_kind::S_query: // query
      case symbol_kind::S_clause: // clause
      case symbol_kind::S_term: // term
      case symbol_kind::S_basic_filter_boost: // basic_filter_boost
      case symbol_kind::S_basic_filter: // basic_filter
      case symbol_kind::S_rangeExpr: // rangeExpr
      case symbol_kind::S_regexExpr: // regexExpr
      case symbol_kind::S_fuzzyExpr: // fuzzyExpr
        value.move< std::unique_ptr<irs::filter> > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_kind.
  QueryParser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  QueryParser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  QueryParser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  QueryParser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  void
  QueryParser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  QueryParser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  QueryParser::symbol_kind_type
  QueryParser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  QueryParser::symbol_kind_type
  QueryParser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }



  // by_state.
  QueryParser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  QueryParser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  QueryParser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  QueryParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  QueryParser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  QueryParser::symbol_kind_type
  QueryParser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  QueryParser::stack_symbol_type::stack_symbol_type ()
  {}

  QueryParser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_CARAT: // CARAT
        value.YY_MOVE_OR_COPY< float > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_TILDE: // TILDE
      case symbol_kind::S_opCompare: // opCompare
      case symbol_kind::S_rangeStart: // rangeStart
      case symbol_kind::S_rangeEnd: // rangeEnd
        value.YY_MOVE_OR_COPY< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_REGEXPTERM: // REGEXPTERM
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_boundVal: // boundVal
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_topLevelQuery: // topLevelQuery
      case symbol_kind::S_query: // query
      case symbol_kind::S_clause: // clause
      case symbol_kind::S_term: // term
      case symbol_kind::S_basic_filter_boost: // basic_filter_boost
      case symbol_kind::S_basic_filter: // basic_filter
      case symbol_kind::S_rangeExpr: // rangeExpr
      case symbol_kind::S_regexExpr: // regexExpr
      case symbol_kind::S_fuzzyExpr: // fuzzyExpr
        value.YY_MOVE_OR_COPY< std::unique_ptr<irs::filter> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  QueryParser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_CARAT: // CARAT
        value.move< float > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_TILDE: // TILDE
      case symbol_kind::S_opCompare: // opCompare
      case symbol_kind::S_rangeStart: // rangeStart
      case symbol_kind::S_rangeEnd: // rangeEnd
        value.move< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_REGEXPTERM: // REGEXPTERM
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_boundVal: // boundVal
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_topLevelQuery: // topLevelQuery
      case symbol_kind::S_query: // query
      case symbol_kind::S_clause: // clause
      case symbol_kind::S_term: // term
      case symbol_kind::S_basic_filter_boost: // basic_filter_boost
      case symbol_kind::S_basic_filter: // basic_filter
      case symbol_kind::S_rangeExpr: // rangeExpr
      case symbol_kind::S_regexExpr: // regexExpr
      case symbol_kind::S_fuzzyExpr: // fuzzyExpr
        value.move< std::unique_ptr<irs::filter> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  QueryParser::stack_symbol_type&
  QueryParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_CARAT: // CARAT
        value.copy< float > (that.value);
        break;

      case symbol_kind::S_TILDE: // TILDE
      case symbol_kind::S_opCompare: // opCompare
      case symbol_kind::S_rangeStart: // rangeStart
      case symbol_kind::S_rangeEnd: // rangeEnd
        value.copy< int > (that.value);
        break;

      case symbol_kind::S_REGEXPTERM: // REGEXPTERM
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_boundVal: // boundVal
        value.copy< std::string > (that.value);
        break;

      case symbol_kind::S_topLevelQuery: // topLevelQuery
      case symbol_kind::S_query: // query
      case symbol_kind::S_clause: // clause
      case symbol_kind::S_term: // term
      case symbol_kind::S_basic_filter_boost: // basic_filter_boost
      case symbol_kind::S_basic_filter: // basic_filter
      case symbol_kind::S_rangeExpr: // rangeExpr
      case symbol_kind::S_regexExpr: // regexExpr
      case symbol_kind::S_fuzzyExpr: // fuzzyExpr
        value.copy< std::unique_ptr<irs::filter> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  QueryParser::stack_symbol_type&
  QueryParser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_CARAT: // CARAT
        value.move< float > (that.value);
        break;

      case symbol_kind::S_TILDE: // TILDE
      case symbol_kind::S_opCompare: // opCompare
      case symbol_kind::S_rangeStart: // rangeStart
      case symbol_kind::S_rangeEnd: // rangeEnd
        value.move< int > (that.value);
        break;

      case symbol_kind::S_REGEXPTERM: // REGEXPTERM
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_boundVal: // boundVal
        value.move< std::string > (that.value);
        break;

      case symbol_kind::S_topLevelQuery: // topLevelQuery
      case symbol_kind::S_query: // query
      case symbol_kind::S_clause: // clause
      case symbol_kind::S_term: // term
      case symbol_kind::S_basic_filter_boost: // basic_filter_boost
      case symbol_kind::S_basic_filter: // basic_filter
      case symbol_kind::S_rangeExpr: // rangeExpr
      case symbol_kind::S_regexExpr: // regexExpr
      case symbol_kind::S_fuzzyExpr: // fuzzyExpr
        value.move< std::unique_ptr<irs::filter> > (that.value);
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
  QueryParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  QueryParser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
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
  QueryParser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  QueryParser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  QueryParser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  QueryParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  QueryParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  QueryParser::debug_level_type
  QueryParser::debug_level () const
  {
    return yydebug_;
  }

  void
  QueryParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  QueryParser::state_type
  QueryParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  QueryParser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  QueryParser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  QueryParser::operator() ()
  {
    return parse ();
  }

  int
  QueryParser::parse ()
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
      case symbol_kind::S_CARAT: // CARAT
        yylhs.value.emplace< float > ();
        break;

      case symbol_kind::S_TILDE: // TILDE
      case symbol_kind::S_opCompare: // opCompare
      case symbol_kind::S_rangeStart: // rangeStart
      case symbol_kind::S_rangeEnd: // rangeEnd
        yylhs.value.emplace< int > ();
        break;

      case symbol_kind::S_REGEXPTERM: // REGEXPTERM
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_boundVal: // boundVal
        yylhs.value.emplace< std::string > ();
        break;

      case symbol_kind::S_topLevelQuery: // topLevelQuery
      case symbol_kind::S_query: // query
      case symbol_kind::S_clause: // clause
      case symbol_kind::S_term: // term
      case symbol_kind::S_basic_filter_boost: // basic_filter_boost
      case symbol_kind::S_basic_filter: // basic_filter
      case symbol_kind::S_rangeExpr: // rangeExpr
      case symbol_kind::S_regexExpr: // regexExpr
      case symbol_kind::S_fuzzyExpr: // fuzzyExpr
        yylhs.value.emplace< std::unique_ptr<irs::filter> > ();
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
#line 104 "query_parser.y"
            {
    driver.result = std::move(yystack_[1].value.as < std::unique_ptr<irs::filter> > ());
}
#line 867 "query_parser.cpp"
    break;

  case 3: // query: clause
#line 109 "query_parser.y"
         { yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(yystack_[0].value.as < std::unique_ptr<irs::filter> > ()); }
#line 873 "query_parser.cpp"
    break;

  case 4: // query: query clause
#line 110 "query_parser.y"
               {
    auto query = std::make_unique<irs::Or>();
    irs::filter::ptr fptr = std::move(yystack_[1].value.as < std::unique_ptr<irs::filter> > ());
    query->add(std::move(fptr));
    fptr = std::move(yystack_[0].value.as < std::unique_ptr<irs::filter> > ());
    query->add(std::move(fptr));
    yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(query);
}
#line 886 "query_parser.cpp"
    break;

  case 5: // query: query OR clause
#line 118 "query_parser.y"
                  {
    auto query = std::make_unique<irs::Or>();
    irs::filter::ptr fptr = std::move(yystack_[2].value.as < std::unique_ptr<irs::filter> > ());
    query->add(std::move(fptr));
    fptr = std::move(yystack_[0].value.as < std::unique_ptr<irs::filter> > ());
    query->add(std::move(fptr));
    yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(query);
}
#line 899 "query_parser.cpp"
    break;

  case 6: // clause: term
#line 128 "query_parser.y"
       { yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(yystack_[0].value.as < std::unique_ptr<irs::filter> > ()); }
#line 905 "query_parser.cpp"
    break;

  case 7: // clause: clause AND term
#line 129 "query_parser.y"
                  {
    auto query = std::make_unique<irs::And>();
    irs::filter::ptr fptr = std::move(yystack_[2].value.as < std::unique_ptr<irs::filter> > ());
    query->add(std::move(fptr));
    fptr = std::move(yystack_[0].value.as < std::unique_ptr<irs::filter> > ());
    query->add(std::move(fptr));
    yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(query);
}
#line 918 "query_parser.cpp"
    break;

  case 8: // term: basic_filter_boost
#line 139 "query_parser.y"
                     { yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(yystack_[0].value.as < std::unique_ptr<irs::filter> > ()); }
#line 924 "query_parser.cpp"
    break;

  case 9: // term: NOT term
#line 140 "query_parser.y"
           {
    auto query = std::make_unique<irs::And>();
    irs::filter::ptr fptr = std::move(yystack_[0].value.as < std::unique_ptr<irs::filter> > ());
    query->add<irs::Not>().set_filter(std::move(fptr));
    yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(query);
}
#line 935 "query_parser.cpp"
    break;

  case 10: // term: LPAREN query RPAREN
#line 146 "query_parser.y"
                      { yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(yystack_[1].value.as < std::unique_ptr<irs::filter> > ()); }
#line 941 "query_parser.cpp"
    break;

  case 11: // term: LPAREN query RPAREN CARAT
#line 147 "query_parser.y"
                            {
    float new_boost = yystack_[2].value.as < std::unique_ptr<irs::filter> > ()->boost() * yystack_[0].value.as < float > ();
    yystack_[2].value.as < std::unique_ptr<irs::filter> > ()->boost(new_boost);
    yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(yystack_[2].value.as < std::unique_ptr<irs::filter> > ());
}
#line 951 "query_parser.cpp"
    break;

  case 12: // basic_filter_boost: basic_filter
#line 154 "query_parser.y"
               {
    yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(yystack_[0].value.as < std::unique_ptr<irs::filter> > ());
}
#line 959 "query_parser.cpp"
    break;

  case 13: // basic_filter_boost: basic_filter CARAT
#line 157 "query_parser.y"
                     {
    float new_boost = yystack_[1].value.as < std::unique_ptr<irs::filter> > ()->boost() * yystack_[0].value.as < float > ();
    yystack_[1].value.as < std::unique_ptr<irs::filter> > ()->boost(new_boost);
    yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(yystack_[1].value.as < std::unique_ptr<irs::filter> > ());
}
#line 969 "query_parser.cpp"
    break;

  case 14: // basic_filter: STRING
#line 164 "query_parser.y"
         {
    std::istringstream iss(yystack_[0].value.as < std::string > ());
    std::string term;
    std::vector<std::string> terms;
    while (std::getline(iss, term, ' ')) {
        terms.push_back(term);
    }
    if(terms.size()==1){
        auto query = std::make_unique<irs::by_term>();
        query->mutable_options()->term = toBstring(terms[0]);
        yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(query);
    } else {
        auto query = std::make_unique<irs::by_phrase>();
        auto* opts = query->mutable_options();
        for(size_t i=0; i<terms.size(); i++){
            opts->push_back<irs::by_term_options>().term = toBstring(terms[i]);
        }
        yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(query);
    }
}
#line 994 "query_parser.cpp"
    break;

  case 15: // basic_filter: rangeExpr
#line 184 "query_parser.y"
            { yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(yystack_[0].value.as < std::unique_ptr<irs::filter> > ()); }
#line 1000 "query_parser.cpp"
    break;

  case 16: // basic_filter: regexExpr
#line 185 "query_parser.y"
            { yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(yystack_[0].value.as < std::unique_ptr<irs::filter> > ()); }
#line 1006 "query_parser.cpp"
    break;

  case 17: // basic_filter: fuzzyExpr
#line 186 "query_parser.y"
            { yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(yystack_[0].value.as < std::unique_ptr<irs::filter> > ()); }
#line 1012 "query_parser.cpp"
    break;

  case 18: // basic_filter: STRING OP_COLON STRING
#line 187 "query_parser.y"
                         {
    std::istringstream iss(yystack_[0].value.as < std::string > ());
    std::string term;
    std::vector<std::string> terms;
    while (std::getline(iss, term, ' ')) {
        terms.push_back(term);
    }
    if(terms.size()==1){
        auto query = std::make_unique<irs::by_term>();
        query->mutable_options()->term = toBstring(terms[0]);
        *query->mutable_field() = yystack_[2].value.as < std::string > ();
        yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(query);
    } else {
        auto query = std::make_unique<irs::by_phrase>();
        auto* opts = query->mutable_options();
        for(size_t i=0; i<terms.size(); i++){
            opts->push_back<irs::by_term_options>().term = toBstring(terms[i]);
        }
        *query->mutable_field() = yystack_[2].value.as < std::string > ();
        yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(query);
    }
}
#line 1039 "query_parser.cpp"
    break;

  case 19: // basic_filter: STRING OP_COLON rangeExpr
#line 209 "query_parser.y"
                            {
    *dynamic_cast<irs::by_range*>(yystack_[0].value.as < std::unique_ptr<irs::filter> > ().get())->mutable_field() = yystack_[2].value.as < std::string > ();
    yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(yystack_[0].value.as < std::unique_ptr<irs::filter> > ());
}
#line 1048 "query_parser.cpp"
    break;

  case 20: // basic_filter: STRING OP_COLON regexExpr
#line 213 "query_parser.y"
                            {
    *dynamic_cast<irs::by_wildcard*>(yystack_[0].value.as < std::unique_ptr<irs::filter> > ().get())->mutable_field() = yystack_[2].value.as < std::string > ();
    yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(yystack_[0].value.as < std::unique_ptr<irs::filter> > ());
}
#line 1057 "query_parser.cpp"
    break;

  case 21: // basic_filter: STRING OP_COLON fuzzyExpr
#line 217 "query_parser.y"
                            {
    *dynamic_cast<irs::by_edit_distance*>(yystack_[0].value.as < std::unique_ptr<irs::filter> > ().get())->mutable_field() = yystack_[2].value.as < std::string > ();
    yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(yystack_[0].value.as < std::unique_ptr<irs::filter> > ());
}
#line 1066 "query_parser.cpp"
    break;

  case 22: // rangeExpr: opCompare STRING
#line 223 "query_parser.y"
                   {
    auto query = std::make_unique<irs::by_range>();
    auto &range = query->mutable_options()->range;
    switch(yystack_[1].value.as < int > ()){
        case int(token::OP_EQUAL):
            range.min = toBstring(yystack_[0].value.as < std::string > ());
            range.max = toBstring(yystack_[0].value.as < std::string > ());
            range.min_type = irs::BoundType::INCLUSIVE;
            range.max_type = irs::BoundType::INCLUSIVE;
            break;
        case int(token::OP_LESSTHAN):
            range.max = toBstring(yystack_[0].value.as < std::string > ());
            range.max_type = irs::BoundType::EXCLUSIVE;
            break;
        case int(token::OP_MORETHAN):
            range.min = toBstring(yystack_[0].value.as < std::string > ());
            range.min_type = irs::BoundType::EXCLUSIVE;
            break;
        case int(token::OP_LESSTHANEQ):
            range.max = toBstring(yystack_[0].value.as < std::string > ());
            range.max_type = irs::BoundType::INCLUSIVE;
            break;
        case int(token::OP_MORETHANEQ):
            range.min = toBstring(yystack_[0].value.as < std::string > ());
            range.min_type = irs::BoundType::INCLUSIVE;
            break;
        default:
            YYERROR;
    }
    yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(query);
}
#line 1102 "query_parser.cpp"
    break;

  case 23: // rangeExpr: rangeStart boundVal RANGE_TO boundVal rangeEnd
#line 254 "query_parser.y"
                                                 {
    const std::string &beg = yystack_[3].value.as < std::string > ();
    const std::string &end = yystack_[1].value.as < std::string > ();
    auto query = std::make_unique<irs::by_range>();
    auto &range = query->mutable_options()->range;
    range.min = toBstring(beg);
    range.max = toBstring(end);
    if(beg.length()>0){
        if(yystack_[4].value.as < int > () == token::RANGEIN_START){
            range.min_type = irs::BoundType::INCLUSIVE;
        } else {
            range.min_type = irs::BoundType::EXCLUSIVE;
        }
    }
    if(end.length()>0){
        if(yystack_[0].value.as < int > () == token::RANGEIN_END){
            range.max_type = irs::BoundType::INCLUSIVE;
        } else {
            range.max_type = irs::BoundType::EXCLUSIVE;
        }
    }
    yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(query);
}
#line 1130 "query_parser.cpp"
    break;

  case 24: // opCompare: OP_EQUAL
#line 279 "query_parser.y"
           { yylhs.value.as < int > () = int(token::OP_EQUAL); }
#line 1136 "query_parser.cpp"
    break;

  case 25: // opCompare: OP_LESSTHAN
#line 280 "query_parser.y"
              { yylhs.value.as < int > () = int(token::OP_LESSTHAN); }
#line 1142 "query_parser.cpp"
    break;

  case 26: // opCompare: OP_MORETHAN
#line 281 "query_parser.y"
              { yylhs.value.as < int > () = int(token::OP_MORETHAN); }
#line 1148 "query_parser.cpp"
    break;

  case 27: // opCompare: OP_LESSTHANEQ
#line 282 "query_parser.y"
                { yylhs.value.as < int > () = int(token::OP_LESSTHANEQ); }
#line 1154 "query_parser.cpp"
    break;

  case 28: // opCompare: OP_MORETHANEQ
#line 283 "query_parser.y"
                { yylhs.value.as < int > () = int(token::OP_MORETHANEQ); }
#line 1160 "query_parser.cpp"
    break;

  case 29: // rangeStart: RANGEIN_START
#line 287 "query_parser.y"
                { yylhs.value.as < int > () = int(token::RANGEIN_START); }
#line 1166 "query_parser.cpp"
    break;

  case 30: // rangeStart: RANGEEX_START
#line 288 "query_parser.y"
                { yylhs.value.as < int > () = int(token::RANGEEX_START); }
#line 1172 "query_parser.cpp"
    break;

  case 31: // rangeEnd: RANGEIN_END
#line 292 "query_parser.y"
              { yylhs.value.as < int > () = int(token::RANGEIN_END); }
#line 1178 "query_parser.cpp"
    break;

  case 32: // rangeEnd: RANGEEX_END
#line 293 "query_parser.y"
              { yylhs.value.as < int > () = int(token::RANGEEX_END); }
#line 1184 "query_parser.cpp"
    break;

  case 33: // boundVal: STRING
#line 297 "query_parser.y"
         {
    yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > ();
}
#line 1192 "query_parser.cpp"
    break;

  case 34: // boundVal: %empty
#line 300 "query_parser.y"
 {
    yylhs.value.as < std::string > () = std::string("");
}
#line 1200 "query_parser.cpp"
    break;

  case 35: // regexExpr: REGEXPTERM
#line 305 "query_parser.y"
             {
    auto query = std::make_unique<irs::by_wildcard>();
    size_t leng = yystack_[0].value.as < std::string > ().length();
    query->mutable_options()->term = toBstring(yystack_[0].value.as < std::string > ().substr(1, leng-2));
    yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(query);
}
#line 1211 "query_parser.cpp"
    break;

  case 36: // fuzzyExpr: STRING TILDE
#line 313 "query_parser.y"
               {
    auto query = std::make_unique<irs::by_edit_distance>();
    auto* opts = query->mutable_options();
    opts->max_terms = 50;  // same as Lucene by default
    opts->max_distance = yystack_[0].value.as < int > ();
    opts->term = toBstring(yystack_[1].value.as < std::string > ());
    yylhs.value.as < std::unique_ptr<irs::filter> > () = std::move(query);
}
#line 1224 "query_parser.cpp"
    break;


#line 1228 "query_parser.cpp"

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
  QueryParser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  QueryParser::yytnamerr_ (const char *yystr)
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
  QueryParser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // QueryParser::context.
  QueryParser::context::context (const QueryParser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  QueryParser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
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
  QueryParser::yy_syntax_error_arguments_ (const context& yyctx,
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
  QueryParser::yysyntax_error_ (const context& yyctx) const
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


  const signed char QueryParser::yypact_ninf_ = -15;

  const signed char QueryParser::yytable_ninf_ = -1;

  const signed char
  QueryParser::yypact_[] =
  {
      35,    35,    35,   -15,   -15,   -15,   -15,   -15,   -15,   -15,
     -15,    -2,     3,     1,     6,   -15,   -15,   -10,   -15,    -6,
      -3,   -15,   -15,   -15,    21,    58,   -15,   -15,   -15,    35,
       6,    35,   -15,   -15,   -15,     2,     7,    16,   -15,   -15,
     -15,     6,   -15,    -3,   -15,   -14,   -15,   -15,   -15
  };

  const signed char
  QueryParser::yydefact_[] =
  {
       0,     0,     0,    24,    25,    27,    26,    28,    29,    30,
      35,    14,     0,     0,     3,     6,     8,    12,    15,     0,
      34,    16,    17,     9,     0,     0,    36,     1,     2,     0,
       4,     0,    13,    22,    33,     0,    10,    18,    19,    20,
      21,     5,     7,    34,    11,     0,    31,    32,    23
  };

  const signed char
  QueryParser::yypgoto_[] =
  {
     -15,   -15,    20,    38,    -1,   -15,   -15,    14,   -15,   -15,
     -15,    10,    29,    30
  };

  const signed char
  QueryParser::yydefgoto_[] =
  {
       0,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      48,    35,    21,    22
  };

  const signed char
  QueryParser::yytable_[] =
  {
      23,    28,    46,    27,    47,    29,     1,    25,     2,    31,
      32,     3,     4,     5,     6,     7,     8,    33,     9,    26,
      34,    43,    24,    10,    11,    29,     1,    44,     2,    36,
      42,     3,     4,     5,     6,     7,     8,    26,     9,    38,
       1,     0,     2,    10,    11,     3,     4,     5,     6,     7,
       8,    30,     9,    45,    39,    40,     0,    10,    11,     0,
       0,     0,    30,     0,     0,     0,     0,    41,     3,     4,
       5,     6,     7,     8,     0,     9,     0,     0,     0,     0,
      10,    37
  };

  const signed char
  QueryParser::yycheck_[] =
  {
       1,     0,    16,     0,    18,     4,     5,     9,     7,     3,
      20,    10,    11,    12,    13,    14,    15,    23,    17,    21,
      23,    19,     2,    22,    23,     4,     5,    20,     7,     8,
      31,    10,    11,    12,    13,    14,    15,    21,    17,    25,
       5,    -1,     7,    22,    23,    10,    11,    12,    13,    14,
      15,    13,    17,    43,    25,    25,    -1,    22,    23,    -1,
      -1,    -1,    24,    -1,    -1,    -1,    -1,    29,    10,    11,
      12,    13,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,
      22,    23
  };

  const signed char
  QueryParser::yystos_[] =
  {
       0,     5,     7,    10,    11,    12,    13,    14,    15,    17,
      22,    23,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    36,    37,    28,    26,     9,    21,     0,     0,     4,
      27,     3,    20,    23,    23,    35,     8,    23,    31,    36,
      37,    27,    28,    19,    20,    35,    16,    18,    34
  };

  const signed char
  QueryParser::yyr1_[] =
  {
       0,    24,    25,    26,    26,    26,    27,    27,    28,    28,
      28,    28,    29,    29,    30,    30,    30,    30,    30,    30,
      30,    30,    31,    31,    32,    32,    32,    32,    32,    33,
      33,    34,    34,    35,    35,    36,    37
  };

  const signed char
  QueryParser::yyr2_[] =
  {
       0,     2,     2,     1,     2,     3,     1,     3,     1,     2,
       3,     4,     1,     2,     1,     1,     1,     1,     3,     3,
       3,     3,     2,     5,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     1,     2
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const QueryParser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "AND", "OR", "NOT",
  "FN_PREFIX", "LPAREN", "RPAREN", "OP_COLON", "OP_EQUAL", "OP_LESSTHAN",
  "OP_LESSTHANEQ", "OP_MORETHAN", "OP_MORETHANEQ", "RANGEIN_START",
  "RANGEIN_END", "RANGEEX_START", "RANGEEX_END", "RANGE_TO", "CARAT",
  "TILDE", "REGEXPTERM", "STRING", "$accept", "topLevelQuery", "query",
  "clause", "term", "basic_filter_boost", "basic_filter", "rangeExpr",
  "opCompare", "rangeStart", "rangeEnd", "boundVal", "regexExpr",
  "fuzzyExpr", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  QueryParser::yyrline_[] =
  {
       0,   104,   104,   109,   110,   118,   128,   129,   139,   140,
     146,   147,   154,   157,   164,   184,   185,   186,   187,   209,
     213,   217,   223,   254,   279,   280,   281,   282,   283,   287,
     288,   292,   293,   297,   300,   305,   313
  };

  void
  QueryParser::yy_stack_print_ () const
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
  QueryParser::yy_reduce_print_ (int yyrule) const
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

  QueryParser::symbol_kind_type
  QueryParser::yytranslate_ (int t) YY_NOEXCEPT
  {
    return static_cast<symbol_kind_type> (t);
  }

#line 9 "query_parser.y"
} // infinity
#line 1738 "query_parser.cpp"

#line 322 "query_parser.y"


namespace infinity{
void QueryParser::error(const location_type &l, const std::string &err_message) {
    std::cerr << "Error: " << err_message << " at " << l << "\n";
}
} //namespace infinity
