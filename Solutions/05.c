#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "elf.h"
#include "arm.h"

#define OUT_GROUP 0
#define IN_GROUP 1

void Disassemble(unsigned int *armI, int count, unsigned int startAddress);
void DecodeInstruction(unsigned int instr, unsigned int instrAddress);
void DecodeDataProcessing(unsigned int instr, int ccode);
int SignExtend(unsigned int x, int bits);
int Rotate(unsigned int rotatee, int amount);

const char *GetConditionString(unsigned int cond);
const char *GetOpCodeString(unsigned int opcode);
const char *GetShiftTypeString(unsigned int type, unsigned int length);
const char *GetBlockDataTransferTypeString(unsigned int l, unsigned int p, unsigned int u, unsigned int baseRegister);
int PrintInstructionShift(unsigned int type, unsigned int length);

void SoftwareInterupt(unsigned int instr);
void Branch(unsigned int instr, unsigned int instrAddress);
void Multiply(unsigned int instr);
void DataProcessing(unsigned int instr);
void LoadStore(unsigned int instr);
void BlockDataTransfer(unsigned int instr);
void BranchExchange(unsigned int instr);

int main(int argc, const char *argv[]) {
	FILE *elfFile;
	ELFHEADER elfHead;
	ELFPROGHDR *prgHdr;
	unsigned int *armInstructions = NULL;
	int i = 0;

	if (argc < 2) {
		fprintf(stderr, "USAGE: DisARM <filename>.\n");
		return 1;
	}

	elfFile = fopen(argv[1], "rb");
	fread(&elfHead, 1, sizeof(ELFHEADER), elfFile);

	if (!((elfHead.magic[0] == 0177) && (elfHead.magic[1] == 'E' && elfHead.magic[2] == 'L' && elfHead.magic[3] == 'F'))) {
		fprintf(stderr, "%s is not an ELF file\n", argv[1]);
		return 2;
	}

	/* Print ELF header information */
	printf("ELF File Information:\n=====================\n");
	printf("File-location:\t%s\n", argv[1]);
	printf("File-type:\t%d\n", elfHead.filetype);
	printf("Arch-type:\t%d\n", elfHead.archtype);
	printf("Entry:\t\t0x%08X\n", elfHead.entry);
	printf("Prog-Header:\t%d bytes\n", elfHead.phdrpos);
	printf("Prog-Headers:\t%d\n", elfHead.phdrcnt);
	printf("Section-Header:\t%d bytes\n", elfHead.shdrpos);

	/* Allocate memory for a program header.
	 * Memory will be re-written in loop for n headers */
	prgHdr = (ELFPROGHDR *)malloc(sizeof(ELFPROGHDR));

	if (!prgHdr) {
		fprintf(elfFile, "Out of Memory\n");
		fclose(elfFile);
		return 3;
	}

	for (i = 0; i < elfHead.phdrcnt; i++) {
		/* Read the ith program header into the prgHdr struct*/
		fseek(elfFile, elfHead.phdrpos + (sizeof(ELFPROGHDR)* i), SEEK_SET);
		fread(prgHdr, 1, sizeof(ELFPROGHDR), elfFile);

		/* Print program header information */
		if (elfHead.phdrcnt != 1) {
			printf("\nProgram Header %d of %d:\n======================\n", i + 1, elfHead.phdrcnt);
		}
		printf("Segment-Offset:\t%d bytes\n", prgHdr->offset);
		printf("File-size:\t%d bytes\n", prgHdr->filesize);
		printf("Alignment:\t%d %s\n\n", prgHdr->align, (prgHdr->align == 1 || 0) ? "(None)" : "");

		/* Allocate memory and read in ARM instructions */
		armInstructions = (unsigned int *)malloc((prgHdr->filesize + 3) & (~3));
		if (armInstructions == NULL) {
			fclose(elfFile);
			free(prgHdr);
			fprintf(stderr, "Out of Memory\n");
			return 3;
		}
		fseek(elfFile, prgHdr->offset, SEEK_SET);
		fread(armInstructions, 1, prgHdr->filesize, elfFile);

		Disassemble(armInstructions, ((prgHdr->filesize + 3 & ~3) / 4), prgHdr->virtaddr);
		printf("\n");
		free(armInstructions);
	}

	free(prgHdr);
	fclose(elfFile);
	return 0;
}

void Disassemble(unsigned int *armInstruction, int count, unsigned int startAddress) {
	int instrAddress, i;

	for (i = 0; i < count; i++) {
		instrAddress = startAddress + i * 4;
		printf("0x%08X: 0x%08X\t", instrAddress, armInstruction[i]);
		DecodeInstruction(armInstruction[i], instrAddress);
		printf("\n");
	}
}

void DecodeInstruction(unsigned int instr, unsigned int instrAddress) {
	if (((instr & 0x0F000000) >> 24) == 0xF)
		SoftwareInterupt(instr);
	else if (((instr & 0x0F000000) >> 25) == 0x5)
		Branch(instr, instrAddress);
	else if ((((instr & 0x0FF00000) >> 22) == 0x0) && (((instr & 0x000000F0) >> 4) == 0x9))
		Multiply(instr);
	else if (((instr & 0x0F000000) >> 26) == 0x0)
		DataProcessing(instr);
	else if (((instr & 0x0F000000) >> 26) == 0x1)
		LoadStore(instr);
	else if (((instr & 0x0F000000) >> 25) == 0x4)
		BlockDataTransfer(instr);
	else if (((instr & 0x0FFFFFF0) >> 4) == 0x12FFF1)
		BranchExchange(instr);
}

void SoftwareInterupt(unsigned int instr) {
	SOFTWARE_INTERUPT swi = {instr};

	printf("SWI%s\t%d",
		GetConditionString(swi.cond),
		swi.immediate
		);
}

void Branch(unsigned int instr, unsigned int instrAddress) {
	BRANCH b = {instr};

	printf("B%s%2s\t0x%08X",
		b.link ? "L" : "",
		GetConditionString(b.cond),
		instrAddress + ((SignExtend(b.immediate, 24) + 2) << 2)
		);
}

void BranchExchange(unsigned int instr) {
	BRANCH_EXCHANGE be = {instr};

	printf("BX%2s\tR%d",
		GetConditionString(be.cond),
		be.operandRegister
		);
}

void Multiply(unsigned int instr) {
	MULTIPLY mul = {instr};

	printf("%3s%2s%s\tR%d, R%d, R%d",
		mul.accumulate ? "MLA" : "MUL",
		GetConditionString(mul.cond),
		mul.setConditionCodes ? "S" : "",
		mul.destinationRegister,
		mul.secondOperandRegister,
		mul.shiftLength
		);

	if (mul.accumulate)
		printf(", R%d", mul.firstOperandRegister);
}

void DataProcessing(unsigned int instr) {
	DATA_PROCESSING dProc = {instr};

	switch (dProc.opcode) {
	case CMP: case CMN: case TST: case TEQ:
		/* [op]{cond} Rn, Op2
		   This case deals with comparisons setting the CPSR */
		printf("%3s%2s\tR%d, ",
			GetOpCodeString(dProc.opcode),
			GetConditionString(dProc.cond),
			dProc.firstOperandRegister
			);
		break;
	case AND: case EOR: case SUB: case RSB: case ADD: case ADC: case SBC: case RSC: case ORR: case BIC:
		/* [op]{cond} Rd, Rn, Op2
		   This case deals with all three-operand instructions in the Data Processing family */
		printf("%3s%2s%s\tR%d, R%d, ",
			GetOpCodeString(dProc.opcode),
			GetConditionString(dProc.cond),
			dProc.setConditionCodes ? "S" : "",
			dProc.destinationRegister,
			dProc.firstOperandRegister
			);
		break;
	case MOV: case MVN:
		/* [op]{cond} Rd, Op2
		   This case deals with the move and move negated instructions */
		printf("%3s%2s%s\tR%d, ",
			GetOpCodeString(dProc.opcode),
			GetConditionString(dProc.cond),
			dProc.setConditionCodes ? "S" : "",
			dProc.destinationRegister
			);
		break;
	}

	if (dProc.b25 == 1) {
		DPROC_IMMEDIATE dImm = {instr};
		printf("#%d", Rotate(dImm.immediate, 2 * dImm.rotation));
	} else {
		DPROC_SHIFT dSft = {instr};
		printf("R%d", dSft.secondOperandRegister);

		if ((dSft.b4 && !dSft.shiftLength) || (!dSft.shiftType && dSft.shiftLength) || dSft.shiftType) {
			if (dSft.b4)
				printf(", %3s #%d", GetShiftTypeString(dSft.shiftType, dSft.shiftLength >> 1), dSft.shiftLength >> 1);
			else
				printf(", %3s #%d", GetShiftTypeString(dSft.shiftType, dSft.shiftLength), dSft.shiftLength);
		}
	}
}

void LoadStore(unsigned int instr) {
	LOAD_STORE ls = {instr};

	printf("%3s%s%s\tR%d, [R%d%s",
		ls.isLoad ? "LDR" : "STR",
		GetConditionString(ls.cond),
		ls.isByte ? "B" : "",
		ls.destinationRegister,
		ls.baseRegister,
		ls.isPreIndex ? "" : "]"
		);

	if (!ls.b25 && ls.immediate) {
		printf(", #%d", ls.immediate);
	} else if (ls.b25) {
		LOAD_STORE_REGISTER lsr = {instr};
		printf(", %sR%d", ls.isAddition ? "" : "-", lsr.offsetRegister);
		if ((!lsr.shiftType && lsr.shiftLength) || lsr.shiftType)
			printf(", %3s #%d", GetShiftTypeString(lsr.shiftType, lsr.shiftLength), lsr.shiftLength);
	}

	printf("%s%s",
		ls.isPreIndex ? "]" : "",
		(ls.b25 || ls.immediate) && (ls.autoIndex && ls.isPreIndex) ? "!" : ""
		);
}

void BlockDataTransfer(unsigned int instr) {
	BLOCK_DATA_TRANSFER bdt = {instr};
	unsigned int currentBit, remainingBits, nextBit;
	int state = OUT_GROUP;
	int i = 0;

	printf("%3s%s%2s\tR%d%s, {",
		bdt.isLoad ? "LDM" : "STM",
		GetConditionString(bdt.cond),
		GetBlockDataTransferTypeString(bdt.isLoad, bdt.isPreIndex, bdt.isAddition, bdt.baseRegister),
		bdt.baseRegister,
		bdt.autoIndex ? "!" : ""
		);

	for (i = 0; i <= 15; i++) {
		currentBit = ((bdt.registerList << (32 - (i + 1))) >> 31);
		remainingBits = bdt.registerList >> (i + 1);
		nextBit = (bdt.registerList << (32 - (i + 2))) >> 31;

		if (state == OUT_GROUP && currentBit) {
			printf("R%d", i);

			if (!remainingBits) {
				break;
			} else if (remainingBits && !nextBit) {
				printf(", ");
			} else if (remainingBits && nextBit) {
				state = IN_GROUP;
				printf("-");
			}
		} else if (state == IN_GROUP && !currentBit) {
			state = OUT_GROUP;
			printf("R%d", i - 1);

			if (remainingBits)
				printf(", ");
		}
	}

	printf("}%s", bdt.loadPSR ? "^" : "");
}

const char *GetConditionString(unsigned int cond) {
	switch (cond) {
	case EQ: return "EQ";
	case NE: return "NE";
	case CS: return "CS";
	case CC: return "CC";
	case MI: return "MI";
	case PL: return "PL";
	case VS: return "VS";
	case VC: return "VC";
	case HI: return "HI";
	case LS: return "LS";
	case GE: return "GE";
	case LT: return "LT";
	case GT: return "GT";
	case LE: return "LE";
	default: return "";
	}
}

const char *GetOpCodeString(unsigned int opcode) {
	switch (opcode) {
	case AND: return "AND";
	case EOR: return "EOR";
	case SUB: return "SUB";
	case RSB: return "RSB";
	case ADD: return "ADD";
	case ADC: return "ADC";
	case SBC: return "SBC";
	case RSC: return "RSC";
	case TST: return "TST";
	case TEQ: return "TEQ";
	case CMP: return "CMP";
	case CMN: return "CMN";
	case ORR: return "ORR";
	case MOV: return "MOV";
	case BIC: return "BIC";
	case MVN: return "MVN";
	default: return "";
	}
}

const char *GetShiftTypeString(unsigned int type, unsigned int length) {
	switch (type) {
	case LSL: return "LSL";
	case LSR: return "LSR";
	case ASR: return "ASR";
	case ROR_RRX: return ((!length) ? "ROR" : "RRX");
	default: return "";
	}
}

const char *GetBlockDataTransferTypeString(unsigned int l, unsigned int p, unsigned int u, unsigned int baseRegister) {
	int group = (u)+(p * 2) + (l * 4);

	switch (group) {
	case ED_DA: return baseRegister ? "ED" : "DA";
	case EA_IA: return baseRegister ? "EA" : "IA";
	case FD_DB: return baseRegister ? "FD" : "DB";
	case FA_IB: return baseRegister ? "FA" : "IB";
	case FA_DA: return baseRegister ? "FA" : "DA";
	case FD_IA: return baseRegister ? "FD" : "IA";
	case EA_DB: return baseRegister ? "EA" : "DB";
	case ED_IB: return baseRegister ? "ED" : "IB";
	default: return "";
	}
}

int SignExtend(unsigned int x, int bits) {
	int r;
	int m = 1U << (bits - 1);

	x = x & ((1U << bits) - 1);
	r = (x ^ m) - m;
	return r;
}

int Rotate(unsigned int rotatee, int amount) {
	unsigned int mask, low, high;

	mask = (1 << amount) - 1;
	low = rotatee & mask;
	high = rotatee >> amount;

	rotatee = (low << (32 - amount)) | high;

	return rotatee;
}