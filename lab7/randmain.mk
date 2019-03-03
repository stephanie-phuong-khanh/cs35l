CC = gcc
CFLAGS = $ -O2 -g3 -Wall -Wextra -march=native -mtune=native -mrdrnd

randlibhw.o: randlibhw.c
	$(CC) $(CFLAGS) -fPIC -c randlibhw.c -o randlibhw.o

randlibsw.o: randlibsw.c
	$(CC) $(CFLAGS) -fPIC -c randlibsw.c -o randlibsw.o

randcpuid.o: randcpuid.c
	$(CC) $(CFLAGS) -c randcpuid.c -o randcpuid.o

randlibhw.so: randlibhw.o
	$(CC) $(CFLAGS) -shared -o randlibhw.so randlibhw.o

randlibsw.so: randlibsw.o
	$(CC) $(CFLAGS) -shared -o randlibsw.so randlibsw.o

randmain.o: randmain.c
	$(CC) $(CFLAGS) -c randmain.c -o randmain.o

randmain: randcpuid.o randmain.o
	$(CC) $(CFLAGS) -ldl -Wl,-rpath=${PWD} randcpuid.o randmain.o -o randmain
