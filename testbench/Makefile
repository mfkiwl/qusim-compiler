all: proctest testgen

proctest: proctest.cpp
testgen: testgen.cpp
.PHONY: clean gen genclean
clean:
	rm proctest testgen
gen: proctest testgen
	./proctest
	./testgen
genclean:
	rm sys_tb.c testdata.txt
