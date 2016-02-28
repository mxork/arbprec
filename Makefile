test:
	gcc test.c natural.c multiply.c shift.c addsubtract.c divide.c compare.c -o test

main:
	gcc main.c natural.c multiply.c shift.c addsubtract.c divide.c compare.c -o main
	
