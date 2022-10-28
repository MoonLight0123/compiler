
test:lex.yy.cpp
	g++ lex.yy.cpp -o test
lex.yy.cpp:lex.yy.c
	mv lex.yy.c lex.yy.cpp
lex.yy.c:test.l
	flex test.l
