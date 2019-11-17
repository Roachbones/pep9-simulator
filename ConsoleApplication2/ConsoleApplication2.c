#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

char garbage;

void pause() {
	printf("press enter to continue.\n");
	scanf("%c", &garbage);
}

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

int get_operand(int aaa, int address_spec) {
	switch (aaa)
	{
	case 0: //immediate
		return address_spec;
		break;
	case 1: //direct
		return Mem[address_spec];
		break;
	default:
		printf("AAAAAAAAAAAAAAAA");
		break;
	}
}

int* r_to_register(int r) { // 0 -> &A, 1 -> &X
	switch (r)
	{
	case 0:
		return &A;
	case 1:
		return &X;
	default:
		printf("AAAAHHHH");
		break;
	}
}

int LDBr(int *R, int aaa, int address_spec) {
	int operand = get_operand(aaa, address_spec);
	*R = operand;
	// todo: set N and Z
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
	
	int PC = 0;
	int inst_spec;
	int naked_inst_spec;
	int inst_spec_clothes;
	int operand_spec;

	int r = 0; //register specifier bit
	int* R; //pointer to the register that the instruction specifies
	int aaa = 0; //addressing mode that the instruction specifies
	do //Von Neumann cycle
	{
		inst_spec = Mem[PC];
		operand_spec = Mem[PC + 1] * 16 + Mem[PC + 2]; //not used for unary instructions

		if (inst_spec < 6) { //XXXX XXXX
			inst_spec_clothes = 0;
		}
		else if (inst_spec < 40) { //XXXX XXX_
			inst_spec_clothes = inst_spec % 2; //last bit
			if (inst_spec < 18) { //XXXX XXXr
				r = inst_spec_clothes;
			}
			else { //XXXX XXXa
				aaa = inst_spec_clothes; //actually only one bit in this case, but acts the same as a three-bit address mode specifier
			}
		}
		else if (inst_spec < 96) { //XXXX Xaaa
			inst_spec_clothes = inst_spec % 8; //last three bits
			aaa = inst_spec_clothes;
		}
		else //XXXX raaa
		{
			inst_spec_clothes = inst_spec % 16;
			aaa = inst_spec % 8;
			r = inst_spec % 16 - inst_spec % 8;
		}
		naked_inst_spec = inst_spec - inst_spec_clothes;
		R = r_to_register(r);

		printf("\ninst_spec: %d\nnaked_inst_spec: %d\nr: %d\naaa: %d\n", inst_spec, naked_inst_spec, r, aaa);

		switch (naked_inst_spec)
		{
		case 0: //STOP
			printf("Executing instruction STOP.\n");
			break; //this is handled by the while condition
		case 208: //LDBr
			PC += 2;
			printf("Executing instruction LDBr.\n");
			LDBr(R, aaa, operand_spec);
			break;
		default:
			printf("Unimplemented instruction specifier: 0d%d / 0x%x\n", inst_spec, inst_spec);
			break;
		}

		PC++;
	} while (inst_spec != 0);

}
