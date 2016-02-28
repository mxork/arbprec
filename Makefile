test_natural:
	gcc -std=gnu99 test_natural.c natural/*.c -o test

main:
	gcc main.c natural.c multiply.c shift.c addsubtract.c divide.c compare.c -o main
	
