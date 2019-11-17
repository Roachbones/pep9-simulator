#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int Mem[65536]; // Pep/9 main memory. initialized to zeroes
int N, Z, V, C; //treat these like Booleans. they should be 0 or 1. initialized to 0
int A, X; //registers. 1 word each...?

int digit_to_int(char d) {
	//example input: '5'
	//example output: 5
	switch (d) {
	case '0':
		return 0;
		break;
	case '1':
		return 1;
		break;
	case '2':
		return 2;
		break;
	case '3':
		return 3;
		break;
	case '4':
		return 4;
		break;
	case '5':
		return 5;
		break;
	case '6':
		return 6;
		break;
	case '7':
		return 7;
		break;
	case '8':
		return 8;
		break;
	case '9':
		return 9;
		break;
	case 'A':
		return 10;
		break;
	case 'B':
		return 11;
		break;
	case 'C':
		return 12;
		break;
	case 'D':
		return 13;
		break;
	case 'E':
		return 14;
		break;
	case 'F':
		return 15;
		break;
	default:
		printf("AAAAAAAA");
		break;
	}
}

int bytestring_to_int(char s[3]) {
	return digit_to_int(s[0]) * 16 + digit_to_int(s[1]);
}

int main()
{	
	printf("Starting simulator...\n\n");
	
	char bytecode[300];
	int i = 0;
	scanf("%[^\n]", &bytecode);

	for (int i = 0; i < strlen(bytecode); i += 3)
	{
		Mem[i / 3] = bytestring_to_int(bytecode + i);
	}

	printf("Here's a memory dump: ");
	for (int i = 0; i < 32; i++)
	{
		printf("%d ", Mem[i]);
	}
	printf("\n");
	
	printf("This is where the simulation would go, but we haven't gotten that far yet.");
}
