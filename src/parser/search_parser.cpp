// A Bison parser, made by GNU Bison 3.5.1.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2020 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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

// Undocumented macros, especially those whose name start with YY_,
// are private implementation details.  Do not rely on them.





#include "search_parser.h"


// Unqualified %code blocks.
#line 43 "search_parser.y"

    #include <iostream>
    #include <sstream>
    #include <string>
    #include <vector>

    // Import modules for implementation
    import infinity_core;
    import search_scanner;

    #undef yylex
    #define yylex scanner.yylex

    // Avoid warnings with the error counter.
    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
    #endif

#line 64 "search_parser.cpp"


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
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE (Symbol)
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
#line 156 "search_parser.cpp"


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

  /*---------------.
  | Symbol types.  |
  `---------------*/

  // basic_symbol.
#if 201103L <= YY_CPLUSPLUS
  template <typename Base>
  SearchParser::basic_symbol<Base>::basic_symbol (basic_symbol&& that)
    : Base (std::move (that))
    , value ()
    , location (std::move (that.location))
  {
    switch (this->type_get ())
    {
      case 11: // STRING
        value.move< InfString > (std::move (that.value));
        break;

      case 10: // CARAT
        value.move< float > (std::move (that.value));
        break;

      case 13: // topLevelQuery
      case 14: // query
      case 15: // clause
      case 16: // term
      case 17: // basic_filter_boost
      case 18: // basic_filter
        value.move< std::unique_ptr<QueryNode> > (std::move (that.value));
        break;

      case 9: // TILDE
        value.move< unsigned long > (std::move (that.value));
        break;

      default:
        break;
    }

  }
#endif

  template <typename Base>
  SearchParser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->type_get ())
    {
      case 11: // STRING
        value.copy< InfString > (YY_MOVE (that.value));
        break;

      case 10: // CARAT
        value.copy< float > (YY_MOVE (that.value));
        break;

      case 13: // topLevelQuery
      case 14: // query
      case 15: // clause
      case 16: // term
      case 17: // basic_filter_boost
      case 18: // basic_filter
        value.copy< std::unique_ptr<QueryNode> > (YY_MOVE (that.value));
        break;

      case 9: // TILDE
        value.copy< unsigned long > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }



  template <typename Base>
  bool
  SearchParser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  void
  SearchParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->type_get ())
    {
      case 11: // STRING
        value.move< InfString > (YY_MOVE (s.value));
        break;

      case 10: // CARAT
        value.move< float > (YY_MOVE (s.value));
        break;

      case 13: // topLevelQuery
      case 14: // query
      case 15: // clause
      case 16: // term
      case 17: // basic_filter_boost
      case 18: // basic_filter
        value.move< std::unique_ptr<QueryNode> > (YY_MOVE (s.value));
        break;

      case 9: // TILDE
        value.move< unsigned long > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_type.
  SearchParser::by_type::by_type ()
    : type (empty_symbol)
  {}

#if 201103L <= YY_CPLUSPLUS
  SearchParser::by_type::by_type (by_type&& that)
    : type (that.type)
  {
    that.clear ();
  }
#endif

  SearchParser::by_type::by_type (const by_type& that)
    : type (that.type)
  {}

  SearchParser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  void
  SearchParser::by_type::clear ()
  {
    type = empty_symbol;
  }

  void
  SearchParser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  int
  SearchParser::by_type::type_get () const YY_NOEXCEPT
  {
    return type;
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

  SearchParser::symbol_number_type
  SearchParser::by_state::type_get () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[+state];
  }

  SearchParser::stack_symbol_type::stack_symbol_type ()
  {}

  SearchParser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.type_get ())
    {
      case 11: // STRING
        value.YY_MOVE_OR_COPY< InfString > (YY_MOVE (that.value));
        break;

      case 10: // CARAT
        value.YY_MOVE_OR_COPY< float > (YY_MOVE (that.value));
        break;

      case 13: // topLevelQuery
      case 14: // query
      case 15: // clause
      case 16: // term
      case 17: // basic_filter_boost
      case 18: // basic_filter
        value.YY_MOVE_OR_COPY< std::unique_ptr<QueryNode> > (YY_MOVE (that.value));
        break;

      case 9: // TILDE
        value.YY_MOVE_OR_COPY< unsigned long > (YY_MOVE (that.value));
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
    switch (that.type_get ())
    {
      case 11: // STRING
        value.move< InfString > (YY_MOVE (that.value));
        break;

      case 10: // CARAT
        value.move< float > (YY_MOVE (that.value));
        break;

      case 13: // topLevelQuery
      case 14: // query
      case 15: // clause
      case 16: // term
      case 17: // basic_filter_boost
      case 18: // basic_filter
        value.move< std::unique_ptr<QueryNode> > (YY_MOVE (that.value));
        break;

      case 9: // TILDE
        value.move< unsigned long > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

#if YY_CPLUSPLUS < 201103L
  SearchParser::stack_symbol_type&
  SearchParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.type_get ())
    {
      case 11: // STRING
        value.copy< InfString > (that.value);
        break;

      case 10: // CARAT
        value.copy< float > (that.value);
        break;

      case 13: // topLevelQuery
      case 14: // query
      case 15: // clause
      case 16: // term
      case 17: // basic_filter_boost
      case 18: // basic_filter
        value.copy< std::unique_ptr<QueryNode> > (that.value);
        break;

      case 9: // TILDE
        value.copy< unsigned long > (that.value);
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
    switch (that.type_get ())
    {
      case 11: // STRING
        value.move< InfString > (that.value);
        break;

      case 10: // CARAT
        value.move< float > (that.value);
        break;

      case 13: // topLevelQuery
      case 14: // query
      case 15: // clause
      case 16: // term
      case 17: // basic_filter_boost
      case 18: // basic_filter
        value.move< std::unique_ptr<QueryNode> > (that.value);
        break;

      case 9: // TILDE
        value.move< unsigned long > (that.value);
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
  SearchParser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
#if defined __GNUC__ && ! defined __clang__ && ! defined __ICC && __GNUC__ * 100 + __GNUC_MINOR__ <= 408
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
#endif
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
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
  SearchParser::yypop_ (int n)
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
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  bool
  SearchParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  SearchParser::yy_table_value_is_error_ (int yyvalue)
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
        YYCDEBUG << "Reading a token: ";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location));
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

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
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
      case 11: // STRING
        yylhs.value.emplace< InfString > ();
        break;

      case 10: // CARAT
        yylhs.value.emplace< float > ();
        break;

      case 13: // topLevelQuery
      case 14: // query
      case 15: // clause
      case 16: // term
      case 17: // basic_filter_boost
      case 18: // basic_filter
        yylhs.value.emplace< std::unique_ptr<QueryNode> > ();
        break;

      case 9: // TILDE
        yylhs.value.emplace< unsigned long > ();
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
  case 2:
#line 86 "search_parser.y"
            {
    parse_result = std::move(yystack_[1].value.as < std::unique_ptr<QueryNode> > ());
}
#line 848 "search_parser.cpp"
    break;

  case 3:
#line 91 "search_parser.y"
         { yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ()); }
#line 854 "search_parser.cpp"
    break;

  case 4:
#line 92 "search_parser.y"
               {
    assert(driver.operator_option_ == FulltextQueryOperatorOption::kInfinitySyntax);
    if (!(yystack_[1].value.as < std::unique_ptr<QueryNode> > ())) {
        yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ());
    } else if (!(yystack_[0].value.as < std::unique_ptr<QueryNode> > ())) {
        yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[1].value.as < std::unique_ptr<QueryNode> > ());
    } else {
        auto q = std::make_unique<OrQueryNode>();
        q->Add(std::move(yystack_[1].value.as < std::unique_ptr<QueryNode> > ()));
        q->Add(std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ()));
        yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(q);
    }
}
#line 872 "search_parser.cpp"
    break;

  case 5:
#line 105 "search_parser.y"
                  {
    if (!(yystack_[2].value.as < std::unique_ptr<QueryNode> > ())) {
        yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ());
    } else if (!(yystack_[0].value.as < std::unique_ptr<QueryNode> > ())) {
        yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[2].value.as < std::unique_ptr<QueryNode> > ());
    } else {
        auto q = std::make_unique<OrQueryNode>();
        q->Add(std::move(yystack_[2].value.as < std::unique_ptr<QueryNode> > ()));
        q->Add(std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ()));
        yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(q);
    }
}
#line 889 "search_parser.cpp"
    break;

  case 6:
#line 119 "search_parser.y"
       { yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ()); }
#line 895 "search_parser.cpp"
    break;

  case 7:
#line 120 "search_parser.y"
                  {
    if (!(yystack_[2].value.as < std::unique_ptr<QueryNode> > ())) {
        yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ());
    } else if (!(yystack_[0].value.as < std::unique_ptr<QueryNode> > ())) {
        yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[2].value.as < std::unique_ptr<QueryNode> > ());
    } else {
        auto query = std::make_unique<AndQueryNode>();
        query->Add(std::move(yystack_[2].value.as < std::unique_ptr<QueryNode> > ()));
        query->Add(std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ()));
        yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(query);
    }
}
#line 912 "search_parser.cpp"
    break;

  case 8:
#line 134 "search_parser.y"
                     { yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ()); }
#line 918 "search_parser.cpp"
    break;

  case 9:
#line 135 "search_parser.y"
           {
    if (!(yystack_[0].value.as < std::unique_ptr<QueryNode> > ())) {
        yylhs.value.as < std::unique_ptr<QueryNode> > () = nullptr;
    } else {
        auto query = std::make_unique<NotQueryNode>();
        query->Add(std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ()));
        yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(query);
    }
}
#line 932 "search_parser.cpp"
    break;

  case 10:
#line 144 "search_parser.y"
                      { yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[1].value.as < std::unique_ptr<QueryNode> > ()); }
#line 938 "search_parser.cpp"
    break;

  case 11:
#line 145 "search_parser.y"
                            {
    yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[2].value.as < std::unique_ptr<QueryNode> > ());
    if (yylhs.value.as < std::unique_ptr<QueryNode> > ()) {
        yylhs.value.as < std::unique_ptr<QueryNode> > ()->MultiplyWeight(yystack_[0].value.as < float > ());
    }
}
#line 949 "search_parser.cpp"
    break;

  case 12:
#line 153 "search_parser.y"
               {
    yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[0].value.as < std::unique_ptr<QueryNode> > ());
}
#line 957 "search_parser.cpp"
    break;

  case 13:
#line 156 "search_parser.y"
                     {
    yylhs.value.as < std::unique_ptr<QueryNode> > () = std::move(yystack_[1].value.as < std::unique_ptr<QueryNode> > ());
    if (yylhs.value.as < std::unique_ptr<QueryNode> > ()) {
        yylhs.value.as < std::unique_ptr<QueryNode> > ()->MultiplyWeight(yystack_[0].value.as < float > ());
    }
}
#line 968 "search_parser.cpp"
    break;

  case 14:
#line 164 "search_parser.y"
         {
    const std::string &field = default_field;
    if(field.empty()){
        error(yystack_[0].location, "default_field is empty");
        YYERROR;
    }
    std::string text = SearchDriver::Unescape(yystack_[0].value.as < InfString > ().text_);
    yylhs.value.as < std::unique_ptr<QueryNode> > () = driver.AnalyzeAndBuildQueryNode(field, text, yystack_[0].value.as < InfString > ().from_quoted_);
}
#line 982 "search_parser.cpp"
    break;

  case 15:
#line 173 "search_parser.y"
                         {
    std::string field = SearchDriver::Unescape(yystack_[2].value.as < InfString > ().text_);
    std::string text = SearchDriver::Unescape(yystack_[0].value.as < InfString > ().text_);
    yylhs.value.as < std::unique_ptr<QueryNode> > () = driver.AnalyzeAndBuildQueryNode(field, text, yystack_[0].value.as < InfString > ().from_quoted_);
}
#line 992 "search_parser.cpp"
    break;

  case 16:
#line 178 "search_parser.y"
               {
    const std::string &field = default_field;
    if(field.empty()){
        error(yystack_[1].location, "default_field is empty");
        YYERROR;
    }
    std::string text = SearchDriver::Unescape(yystack_[1].value.as < InfString > ().text_);
    yylhs.value.as < std::unique_ptr<QueryNode> > () = driver.AnalyzeAndBuildQueryNode(field, text, yystack_[1].value.as < InfString > ().from_quoted_, yystack_[0].value.as < unsigned long > ());
}
#line 1006 "search_parser.cpp"
    break;

  case 17:
#line 187 "search_parser.y"
                               {
    std::string field = SearchDriver::Unescape(yystack_[3].value.as < InfString > ().text_);
    std::string text = SearchDriver::Unescape(yystack_[1].value.as < InfString > ().text_);
    yylhs.value.as < std::unique_ptr<QueryNode> > () = driver.AnalyzeAndBuildQueryNode(field, text, yystack_[1].value.as < InfString > ().from_quoted_, yystack_[0].value.as < unsigned long > ());
}
#line 1016 "search_parser.cpp"
    break;


#line 1020 "search_parser.cpp"

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
      YY_STACK_PRINT ();

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
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
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
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[+yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yy_error_token_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yy_error_token_)
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

  // Generate an error message.
  std::string
  SearchParser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    std::ptrdiff_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

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
    if (!yyla.empty ())
      {
        symbol_number_type yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];

        int yyn = yypact_[+yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yy_error_token_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

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
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char SearchParser::yypact_ninf_ = -4;

  const signed char SearchParser::yytable_ninf_ = -1;

  const signed char
  SearchParser::yypact_[] =
  {
      -2,    -2,    -2,     2,    15,     1,    11,    -4,    -4,     6,
      -4,    14,    12,    -4,    -4,    -4,    -2,    11,    -2,    -4,
      16,    13,    11,    -4,    -4,    -4
  };

  const signed char
  SearchParser::yydefact_[] =
  {
       0,     0,     0,    14,     0,     0,     3,     6,     8,    12,
       9,     0,     0,    16,     1,     2,     0,     4,     0,    13,
      10,    15,     5,     7,    11,    17
  };

  const signed char
  SearchParser::yypgoto_[] =
  {
      -4,    -4,    22,    -3,    -1,    -4,    -4
  };

  const signed char
  SearchParser::yydefgoto_[] =
  {
      -1,     4,     5,     6,     7,     8,     9
  };

  const signed char
  SearchParser::yytable_[] =
  {
      10,    15,    17,     1,     2,    16,     1,     2,    17,     3,
      12,    13,     3,    22,    18,    14,    19,    23,    16,     1,
       2,    20,    25,    21,    11,     3,    24
  };

  const signed char
  SearchParser::yycheck_[] =
  {
       1,     0,     5,     5,     6,     4,     5,     6,    11,    11,
       8,     9,    11,    16,     3,     0,    10,    18,     4,     5,
       6,     7,     9,    11,     2,    11,    10
  };

  const signed char
  SearchParser::yystos_[] =
  {
       0,     5,     6,    11,    13,    14,    15,    16,    17,    18,
      16,    14,     8,     9,     0,     0,     4,    15,     3,    10,
       7,    11,    15,    16,    10,     9
  };

  const signed char
  SearchParser::yyr1_[] =
  {
       0,    12,    13,    14,    14,    14,    15,    15,    16,    16,
      16,    16,    17,    17,    18,    18,    18,    18
  };

  const signed char
  SearchParser::yyr2_[] =
  {
       0,     2,     2,     1,     2,     3,     1,     3,     1,     2,
       3,     4,     1,     2,     1,     3,     2,     4
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const SearchParser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "AND", "OR", "NOT", "LPAREN",
  "RPAREN", "OP_COLON", "TILDE", "CARAT", "STRING", "$accept",
  "topLevelQuery", "query", "clause", "term", "basic_filter_boost",
  "basic_filter", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned char
  SearchParser::yyrline_[] =
  {
       0,    86,    86,    91,    92,   105,   119,   120,   134,   135,
     144,   145,   153,   156,   164,   173,   178,   187
  };

  // Print the state stack on the debug stream.
  void
  SearchParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  SearchParser::yy_reduce_print_ (int yyrule)
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

  SearchParser::token_number_type
  SearchParser::yytranslate_ (int t)
  {
    return static_cast<token_number_type> (t);
  }

#line 10 "search_parser.y"
} // infinity
#line 1419 "search_parser.cpp"

#line 193 "search_parser.y"


namespace infinity{
void SearchParser::error(const location_type &l, const std::string &err_message) {
    std::cerr << "Error: " << err_message << " at " << l << "\n";
}
} //namespace infinity
