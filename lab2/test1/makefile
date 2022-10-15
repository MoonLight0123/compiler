test:test.tab.c
	gcc test.tab.c -o test
test.tab.c:test.y
	bison -d test.y
clean:
	rm -f test.tab.c test.tab.h
