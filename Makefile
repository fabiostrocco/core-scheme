SRC=./src

compile:
	clang++ -g -O3 $(SRC)/*.cpp `llvm-config --cxxflags --ldflags --libs core` -std=c++11 -o csc-comp

compile-test:
	clang++ -g -O3 -fsanitize=address $(SRC)/*.cpp `llvm-config --cxxflags --ldflags --libs core` -std=c++11 -o csc-comp

test:
	make clean
	make compile-test
	./testcases.sh

clean:
	rm -f csc-comp
	rm -rf *.pp *.o *.ll *.pp *.s *.generated *.out
	rm -f *.pp *.o *.ll *.pp *.s *~ src/*~ runtime/*~ lib/*~ *.generated *.out
