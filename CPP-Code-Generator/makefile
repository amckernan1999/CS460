P3.out : Project3.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o Object.o
	g++ -g -o P3.out Project3.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o Object.o

Project3.o : Project3.cpp SetLimits.h SyntacticalAnalyzer.h
	g++ -g -c Project3.cpp

Object.o : Object.cpp Object.h
	g++ -g -c Object.cpp

SetLimits.o : SetLimits.cpp SetLimits.h
	g++ -g -c SetLimits.cpp

SyntacticalAnalyzer.o : SyntacticalAnalyzer.cpp SyntacticalAnalyzer.h LexicalAnalyzer.h CodeGenerator.h Object.h
	g++ -g -c SyntacticalAnalyzer.cpp

CodeGenerator.o : CodeGenerator.cpp CodeGenerator.h
	                g++ -g -c CodeGenerator.cpp

clean : 
	rm [SP]*.o P3.out *.gch

submit : Project3.cpp LexicalAnalyzer.h SyntacticalAnalyzer.h SyntacticalAnalyzer.cpp Object.cpp Object.h CodeGenerator.cpp CodeGenerator.h makefile README.txt
	rm -rf mckernanP3
	mkdir mckernanP3
	cp Project3.cpp mckernanP3
	cp SyntacticalAnalyzer.h mckernanP3
	cp SyntacticalAnalyzer.cpp mckernanP3
	cp CodeGenerator.cpp mckernanP3
	cp CodeGenerator.h mckernanP3
	cp Object.cpp mckernanP3
	cp Object.h mckernanp3
	cp makefile mckernanP3
	cp README.txt mckernanP3
	tar cfvz mckernanP3.tgz mckernanP3
	cp mckernanP3.tgz ~tiawatts/cs460drop
