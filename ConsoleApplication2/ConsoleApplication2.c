#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int VERBOSE = 1; // If it's 1, we'll print lots of debug statements during simulation. Otherwise, just Pep/9 output.

// Pep/9 main memory. mostly initialized to zeroes, but with preset machine vectors
int Mem[65536] = {
	[0xFFF4] = 0xFB, [0xFFF5] = 0x8F, // initial SP
	[0xFFF6] = 0xFC, [0xFFF7] = 0x0F,
	[0xFFF8] = 0xFC, [0xFFF9] = 0x15, // charIn
	[0xFFFA] = 0xFC, [0xFFFB] = 0x15, // charOut
	[0xFFFC] = 0xFC, [0xFFFD] = 0x17,
	[0xFFFE] = 0xFC, [0xFFFF] = 0x52
};
int N, Z, V, C; //treat these like Booleans. they should be 0 or 1. initialized to 0
int A, X; //registers. 1 word each
int PC = 0;
int SP = 0xFB8F;

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
	// address_spec = pep_int_to_c_int(address_spec);
	switch (aaa)
	{
	case 0: //immediate addressing is taken care of in read_byte
		printf("error: immediate addressing shouldn't be resolved");
		break;
	case 1: //direct
		return address_spec;
	case 2: //indirect
		return Mem[address_spec];
	case 3: //stack-relative
		// address specifiers are signed in some contexts
		// so if we do math with them then we need to chop off the extra bits
		return (SP + address_spec) & 0xFFFF;
	case 4: //stack-relative deferred
		return Mem[(SP + address_spec) & 0xFFFF];
	case 5: //indexed
		return (address_spec + X) & 0xFFFF;
	case 6: //stack-indexed
		return (SP + address_spec + X) & 0xFFFF;
	case 7: //stack-indexed deferred
		return (Mem[(SP + address_spec) & 0xFFFF] + X) & 0xFFFF;
	default:
		printf("AAAAAAAAAAAAAAAA");
		break;
	}
}

// I'm pretty sure these can be simplified
int pep_int_to_c_int(int n) { //n should be an int representing a Pep/9 word
	if (n >> 15) { // most significant bit is 1
		return - ((n ^ 0xFFFF) + 1); //convert to positive, then to a negative c int
	} else {
		return n;
	}
}
int c_int_to_pep_int(int n) {
	if (n < 0) {
		return ((-n) ^ 0xFFFF) + 1; //convert to positive, then to a negative pep int
	} else {
		return n;
	}
}

// returns the byte stored at the address specified.
int read_byte(int aaa, int address_spec) {
	if (aaa == 0) { return address_spec; } //immediate addressing
	int address = resolve_address(aaa, address_spec);
	if (address == 0xFC15) { //charIn
		if (VERBOSE) {printf("Enter character: ");}
		// this doesn't work very well due to C's input buffer quirks
		// all the stackoverflow answers beg you to use something besides scanf
		// but Dr. Williamson only showed us scanf so 🤷
		scanf("%c", &Mem[address]); //cast char to int
		if (VERBOSE) {printf("\n");}
		return Mem[address];
	}
	//printf("\t\t\t\treading 0x%X", address);
	return Mem[address];
}
//returns the word stored at the address specified.
int read_word(int aaa, int address_spec) {
	if (aaa == 0) { return address_spec; } //immediate addressing
	int address = resolve_address(aaa, address_spec);
	return (read_byte(1, address) << 8) + read_byte(1, address + 1); //with direct addressing
}
// writes new_byte at the address specified.
void write_byte(int aaa, int address_spec, int new_byte) {
	int address = resolve_address(aaa, address_spec);
	if (address == 0xFC16) { //charOut
		if (VERBOSE) {
			printf("\t\tcharOut output: %c\n", new_byte);
		} else {
			printf("%c", new_byte);
		}
	}
	//printf("\t\t\t\twriting 0x%X", address);
	Mem[address] = new_byte;
}
// writes new_word at the address specified.
void write_word(int aaa, int address_spec, int new_word) {
	int address = resolve_address(aaa, address_spec);
	write_byte(1, address, new_word >> 8);
	write_byte(1, address + 1, new_word % 256);
}

int* r_to_register(int r) { // returns a pointer to the appropriate register. 0 -> &A, 1 -> &X
	switch (r)
	{
	case 0:
		return &A;
	case 1:
		return &X;
	case -1:
		return 0;
	default:
		printf("error: invalid register bit.");
		return 0;
	}
}

void set_NZ_from_word(int w) { //w will usually be a register
	Z = (w == 0);
	N = (w < 0);
}

void LDBr(int *R, int aaa, int address_spec) {
	*R = (*R & (0xFF00)) + read_byte(aaa, address_spec); //most significant byte remains unchanged
	set_NZ_from_word(*R & 0x00FF); // N <- 0, Z <- (*R{8..15} = 0)
}
void LDWr(int *R, int aaa, int address_spec) {
	*R = read_word(aaa, address_spec);
	set_NZ_from_word(*R);
}
void STBr(int *R, int aaa, int address_spec) {
	if (aaa == 0) {
		printf("Pep/9 ERROR: STBr does not accept immediate addressing!\n");
	};
	write_byte(aaa, address_spec, *R & 0x00FF); //only write least significant byte
}
void STWr(int *R, int aaa, int address_spec) {
	if (aaa == 0) {
		printf("Pep/9 ERROR: STWr does not accept immediate addressing!\n");
	};
	write_word(aaa, address_spec, *R);
}

void BR(int a, int address_spec) {
	PC = read_byte(a, address_spec);
}
void BRV(int a, int address_spec) {
	if (V) {
		BR(a, address_spec);
	}
}

void ADDSP(int aaa, int address_spec) {
	SP += pep_int_to_c_int(read_byte(aaa, address_spec));
}
void SUBSP(int aaa, int address_spec) {
	SP -= pep_int_to_c_int(read_byte(aaa, address_spec));
}
void MOVSPA() {
	A = SP;
}

void DECO(int aaa, int address_spec) {
	if (VERBOSE) {
		printf("\t\tDECO output: %d", read_word(aaa, address_spec));
	} else {
		printf("%d", read_word(aaa, address_spec));
	}
}

int main()
{	
	char bytecode[900]; //up to 900 characters / 300 bytecode bytes
	printf("Input bytecode: ");
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
		r = -1; aaa = -1; operand_spec = -1;

		inst_spec = Mem[PC];

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

		if (naked_inst_spec < 18) { //unary instruction
			PC += 1;
		} else { //non-unary instruction
			operand_spec = (Mem[PC + 1] << 8) + Mem[PC + 2]; //not used for unary instructions
			PC += 3;
		}

		if (VERBOSE) {
			printf("\ninst_spec: 0x%X\nnaked_inst_spec: 0x%X\n", inst_spec, naked_inst_spec);
			if (operand_spec != -1) {printf("operand_spec: 0x%04X\n", operand_spec);}
			printf("r: %d\taaa: %d\n", r, aaa);
			printf("A: %d\tX: %d\tSP: 0x%X\n", A, X, SP);
		}

		switch (naked_inst_spec)
		{
		case 0: //STOP
			if (VERBOSE) {printf("Executing instruction STOP.\n");}
			break; //this is handled by the while condition
		case 18: //BR
			if (VERBOSE) {printf("Executing instruction BR.\n");}
			BR(aaa, operand_spec);
			break;
		case 56: // DECO
			if (VERBOSE) {printf("Executing instruction DECO.\n");}
			DECO(aaa, operand_spec);
			break;
		case 80: //ADDSP
			if (VERBOSE) {printf("Executing instruction ADDSP.\n");}
			ADDSP(aaa, operand_spec);
			break;
		case 88: //SUBSP
			if (VERBOSE) {printf("Executing instruction SUBSP.\n");}
			SUBSP(aaa, operand_spec);
			break;
		case 192: // LDWr
			if (VERBOSE) {printf("Executing instruction LDBr.\n");}
			LDWr(R, aaa, operand_spec);
			break;
		case 208: //LDBr
			if (VERBOSE) {printf("Executing instruction LDBr.\n");}
			LDBr(R, aaa, operand_spec);
			break;
		case 224: //STWr
			if (VERBOSE) {printf("Executing instruction STWr.\n");}
			STWr(R, aaa, operand_spec);
			break;
		case 240: //STBr
			if (VERBOSE) {printf("Executing instruction STBr.\n");}
			STBr(R, aaa, operand_spec);
			break;
		default:
			printf("Unimplemented instruction specifier: 0d%d / 0x%x\n", inst_spec, inst_spec);
			break;
		}

		// make sure all our "bytes" only store 1 byte of information
		for (int i=0; i<0xFFFF; i++) {
			if (Mem[i] > 0xFF) {printf("Error: byte 0x%X invalid value (0x%X)", i, Mem[i]);}
		}

	} while (inst_spec != 0);

	printf("\nSimulation over.\n");
}
