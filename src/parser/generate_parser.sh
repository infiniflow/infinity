flex -di --reentrant --bison-bridge --bison-location -Cem -olexer.cpp --header-file=lexer.h -Psql lexer.l
bison -oparser.cpp --header=parser.h parser.y -Wcounterexamples -d -v
flex -+dvB8 -Cem -oquery_lexer.cpp --header-file=query_lexer.h query_lexer.l
bison -oquery_parser.cpp --header=query_parser.h query_parser.y -Wcounterexamples -d -v
flex -+dvB8 -Cem -osearch_lexer.cpp --header-file=search_lexer.h search_lexer.l
bison -osearch_parser.cpp --header=search_parser.h search_parser.y -Wcounterexamples -d -v