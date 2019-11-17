/*
Kevin Thomas
9/17/2019
CSCI 220
2.c

Takes two integers a and b as input.
Demonstrates the output of five mathematical operations on them.
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main()
{
	//pretty straightforward
	int a;
	int b;
	printf("a: ");
	scanf("%d", &a);
	printf("b: ");
	scanf("%d", &b);
	printf("%d + %d = %d\n", a, b, a + b);
	printf("%d - %d = %d\n", a, b, a - b);
	printf("%d * %d = %d\n", a, b, a * b);
	if (b == 0) {
		printf("Can't divide by zero.\n");
	}
	else {
		printf("%d / %d = %d\n", a, b, a / b);
		printf("%d %% %d = %d\n", a, b, a % b);
	}
}
