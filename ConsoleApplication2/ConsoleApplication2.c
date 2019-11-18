#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int VERBOSE = 1; // If it's 1, we'll print lots of debug statements during simulation. Otherwise, just Pep/9 output.

char garbage;

void pause() {
	printf("press enter to continue.\n");
	scanf("%c", &garbage);
}

int Mem[65536]; // Pep/9 main memory. initialized to zeroes
int N, Z, V, C; //treat these like Booleans. they should be 0 or 1. initialized to 0
int A, X; //registers. 1 word each...?

int digit_to_int(char d) {
	//example input: 'B'
	//example output: 11
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


int resolve_address(int aaa, int address_spec) {
	switch (aaa)
	{
	case 0: //immediate addressing is taken care of in read_byte
		printf("AAAA");
		break;
	case 1: //direct
		return address_spec;
		break;
	case 2: //indirect
		return Mem[address_spec];
		break;
	default:
		printf("AAAAAAAAAAAAAAAA");
		break;
	}
}
int read_byte(int aaa, int address_spec) {
	if (aaa == 0) { //immediate addressing
		return address_spec;
	}
	int address = resolve_address(aaa, address_spec);
	if (address == 0xFC15) { //charIn
		scanf("%c", &Mem[address]); //cast char to int
		return Mem[address];
	}
	return Mem[address];
}
void write_byte(int aaa, int address_spec, int new_byte) {
	int address = resolve_address(aaa, address_spec);
	if (address == 0xFC16) { //charOut
		if (VERBOSE) {
			printf("charOut output: %c\n", new_byte);
		} else {
			printf("%c", new_byte);
		}
	}
	Mem[address] = new_byte;
}

int* r_to_register(int r) { // returns a pointer to the appropriate register. 0 -> &A, 1 -> &X
	switch (r)
	{
	case 0:
		return &A;
	case 1:
		return &X;
	default:
		return 0;
		break;
	}
}

void set_NZ_from_word(int w) { //w will usually be a register
	Z = (w == 0);
	N = (w < 0);
}
int LDBr(int *R, int aaa, int address_spec) {
	*R = read_byte(aaa, address_spec);
	set_NZ_from_word(*R);
}
int STBr(int *R, int aaa, int address_spec) {
	if (aaa == 0) {
		printf("Pep/9 ERROR: STBr does not accept immediate addressing!\n");
	};
	write_byte(aaa, address_spec, *R);
	set_NZ_from_word(*R);
}

int main()
{	
	printf("Input bytecode: ");
	
	char bytecode[300]; //up to 300 characters / 100 bytecode bytes
	scanf("%[^\n]", &bytecode); //example: D1 00 0D F1 FC 16 D1 00 0E F1 FC 16 00 48 69
	
	// we should eventually change this so it takes in bytecode until it reaches the zz
	// but for now let's just not use zz and save ourselves some typing
	for (int i = 0; i < strlen(bytecode); i += 3)
	{
		Mem[i / 3] = bytestring_to_int(bytecode + i);
	}

	printf("Here's a memory dump: ");
	for (int i = 0; i < 32; i++)
	{
		printf("%d ", Mem[i]);
	}
	printf("\nStarting simulation...\n");
	
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
		// these should be set before they're checked, every cycle
		// let's reset them to an invalid value now so we notice if we try to read them before we set them
		r = -1; aaa = -1;

		inst_spec = Mem[PC];
		operand_spec = Mem[PC + 1] * 16 * 16 + Mem[PC + 2]; //not used for unary instructions

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

		if (VERBOSE) {
			printf("\ninst_spec: %d\nnaked_inst_spec: %d\nr: %d    aaa: %d\n", inst_spec, naked_inst_spec, r, aaa);
			printf("A: %d    X: %d\n", A, X);
		}

		switch (naked_inst_spec)
		{
		case 0: //STOP
			if (VERBOSE) {printf("Executing instruction STOP.\n");}
			break; //this is handled by the while condition
		case 208: //LDBr
			PC += 2;
			if (VERBOSE) {printf("Executing instruction LDBr.\n");}
			LDBr(R, aaa, operand_spec);
			break;
		case 240: //STBr
			PC += 2;
			if (VERBOSE) {printf("Executing instruction STBr.\n");}
			STBr(R, aaa, operand_spec);
			break;
		default:
			printf("Unimplemented instruction specifier: 0d%d / 0x%x\n", inst_spec, inst_spec);
			break;
		}

		PC++;
	} while (inst_spec != 0);
	printf("\nSimulation over.\n");
}
