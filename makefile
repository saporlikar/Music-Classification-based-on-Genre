mytry.exe : mytry.o
	gcc mytry.o -o mytry.exe -lm

main.o : mytry.c
	gcc -c mytry.c
