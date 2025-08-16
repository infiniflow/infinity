flex -di --reentrant --bison-bridge --bison-location -Cem -olexer.cpp --header-file=lexer.h -Psql lexer.l
bison -oparser.cpp --header=parser.h parser.y -Wcounterexamples -d -v

flex -di --reentrant --bison-bridge --bison-location -Cem -oexpression_lexer.cpp --header-file=expression_lexer.h -Psql expression_lexer.l
bison -oexpression_parser.cpp --header=expression_parser.h expression_parser.y -Wcounterexamples -d -v

flex -+dvB8 -Cem -osearch_lexer.cpp search_lexer.l
bison -osearch_parser.cpp --header=search_parser.h search_parser.y -Wcounterexamples -d -v

# If you encounter a conflict between the C and C++ versions of yy_input_file,
# you can rename yy_input_file to yy_input_file_ as a workaround
