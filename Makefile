all : test_premier test_deuxieme test_troisieme test_quatrieme

test_premier : test_premier.o premier.o
	gcc -Wall -o test_premier test_premier.o premier.o

test_premier.o : test_premier.c premier.h
	gcc -Wall -c test_premier.c

premier.o : premier.c premier.h
	gcc -Wall -c premier.c

test_deuxieme : test_deuxieme.o deuxieme.o premier.o
	gcc -Wall -o test_deuxieme test_deuxieme.o deuxieme.o premier.o

test_deuxieme.o : test_deuxieme.c premier.h deuxieme.h
	gcc -Wall -c test_deuxieme.c

deuxieme.o : deuxieme.c premier.h deuxieme.h
	gcc -Wall -c deuxieme.c

test_troisieme : test_troisieme.o troisieme.o deuxieme.o premier.o
	gcc -Wall -o test_troisieme test_troisieme.o troisieme.o deuxieme.o premier.o

test_troisieme.o : test_troisieme.c premier.h deuxieme.h troisieme.h
	gcc -Wall -c test_troisieme.c

troisieme.o : troisieme.c premier.h deuxieme.h troisieme.h
	gcc -Wall -c troisieme.c

test_quatrieme : test_quatrieme.o quatrieme.o troisieme.o deuxieme.o premier.o
	gcc -Wall -lssl -lcrypto -o test_quatrieme test_quatrieme.o quatrieme.o troisieme.o deuxieme.o premier.o -lm

test_quatrieme.o : test_quatrieme.c premier.h deuxieme.h troisieme.h quatrieme.h
	gcc -Wall -lssl -lcrypto -c test_quatrieme.c

quatrieme.o : quatrieme.c premier.h deuxieme.h troisieme.h quatrieme.h
	gcc -Wall -c quatrieme.c

clean :
	rm -f *.o test_premier test_deuxieme test_troisieme test_quatrieme
