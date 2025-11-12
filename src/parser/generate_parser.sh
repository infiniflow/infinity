flex -di --reentrant --bison-bridge --bison-location -Cem -olexer.cpp --header-file=lexer.h -Psql lexer.l
bison -Wall -v -oparser.cpp --defines=parser.h parser.y

flex -di --reentrant --bison-bridge --bison-location -Cem -oexpression_lexer.cpp --header-file=expression_lexer.h -Psql expression_lexer.l
bison -Wall -v -oexpression_parser.cpp --defines=expression_parser.h expression_parser.y

flex -+dvB8 -Cem -osearch_lexer.cpp search_lexer.l
bison -Wall -v -osearch_parser.cpp --defines=search_parser.h search_parser.y

# Script to fix "reference to 'yy_input_file' is ambiguous" issue
sed -i 's/std::streambuf\* yy_input_file;/std::streambuf* yy_input_file_;/g' search_lexer.cpp
sed -i 's/->yy_input_file/->yy_input_file_/g' search_lexer.cpp
sed -i 's/\.yy_input_file_/\.search_yy_input_file_/g' search_lexer.cpp
