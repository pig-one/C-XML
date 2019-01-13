xml : main.o storeBlock.o firstcheck.o finalcheck.o
	gcc main.o storeBlock.o firstcheck.o finalcheck.o -o xml -lpthread
main.o  : main.c myXml.h
	gcc -c main.c
storeBlock.o : storeBlock.c myXml.h
	gcc -c storeBlock.c
firstcheck.o : firstcheck.c myXml.h
	gcc -c firstcheck.c
finalcheck.o : finalcheck.c myXml.h
	gcc -c finalcheck.c
