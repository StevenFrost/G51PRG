#pragma once

#define	EQ	0x0
#define NE	0x1
#define	CS	0x2
#define	CC	0x3
#define	MI	0x4
#define	PL	0x5
#define	VS	0x6
#define VC	0x7
#define	HI	0x8
#define	LS	0x9
#define	GE	0xA
#define LT	0xB
#define	GT	0xC
#define	LE	0xD
#define	AL	0xE

#define	AND	0x0
#define EOR	0x1
#define	SUB	0x2
#define	RSB	0x3
#define	ADD	0x4
#define	ADC	0x5
#define	SBC	0x6
#define RSC	0x7
#define	TST	0x8
#define	TEQ	0x9
#define	CMP	0xA
#define CMN	0xB
#define	ORR	0xC
#define	MOV	0xD
#define	BIC	0xE
#define	MVN	0xF

#define LSL 0x0
#define LSR 0x1
#define ASR	0x2
#define ROR_RRX	0x3

#define ED_DA 0x0
#define EA_IA 0x1
#define FD_DB 0x2
#define FA_IB 0x3
#define FA_DA 0x4
#define FD_IA 0x5
#define EA_DB 0x6
#define ED_IB 0x7

typedef union {
	uint32_t i;
	struct {
		unsigned int immediate : 24;
		unsigned int : 4;
		unsigned int cond : 4;
	};
} SOFTWARE_INTERUPT;

typedef union {
	uint32_t i;
	struct {
		unsigned int immediate : 24;
		unsigned int link : 1;
		unsigned int : 3;
		unsigned int cond : 4;
	};
} BRANCH;

typedef union {
	uint32_t i;
	struct {
		unsigned int operandRegister : 4;
		unsigned int : 24;
		unsigned int cond : 4;
	};
} BRANCH_EXCHANGE;

typedef union {
	uint32_t i;
	struct {
		unsigned int secondOperandRegister : 4;
		unsigned int : 4;
		unsigned int shiftLength : 4;
		unsigned int firstOperandRegister : 4;
		unsigned int destinationRegister : 4;
		unsigned int setConditionCodes : 1;
		unsigned int accumulate : 1;
		unsigned int : 6;
		unsigned int cond : 4;
	};
} MULTIPLY;

typedef union {
	uint32_t i;
	struct {
		unsigned int : 4;
		unsigned int shiftType : 1;
		unsigned int : 7;
		unsigned int destinationRegister : 4;
		unsigned int firstOperandRegister : 4;
		unsigned int setConditionCodes : 1;
		unsigned int opcode : 4;
		unsigned int b25 : 1;
		unsigned int : 2;
		unsigned int cond : 4;
	};
} DATA_PROCESSING;

typedef union {
	uint16_t i;
	struct {
		unsigned int immediate : 8;
		unsigned int rotation : 4;
	};
} DPROC_IMMEDIATE;

typedef union {
	uint16_t i;
	struct {
		unsigned int secondOperandRegister : 4;
		unsigned int b4 : 1;
		unsigned int shiftType : 2;
		unsigned int shiftLength : 5;
	};
} DPROC_SHIFT;

typedef union {
	uint32_t i;
	struct {
		unsigned int immediate : 12;
		unsigned int destinationRegister : 4;
		unsigned int baseRegister : 4;
		unsigned int isLoad : 1;
		unsigned int autoIndex : 1;
		unsigned int isByte : 1;
		unsigned int isAddition : 1;
		unsigned int isPreIndex : 1;
		unsigned int b25 : 1;
		unsigned int : 2;
		unsigned int cond : 4;
	};
} LOAD_STORE;

typedef union {
	uint16_t i;
	struct {
		unsigned int offsetRegister : 4;
		unsigned int : 1;
		unsigned int shiftType : 2;
		unsigned int shiftLength : 5;
	};
} LOAD_STORE_REGISTER;

typedef union {
	uint32_t i;
	struct {
		unsigned int registerList : 16;
		unsigned int baseRegister : 4;
		unsigned int isLoad : 1;
		unsigned int autoIndex : 1;
		unsigned int loadPSR : 1;
		unsigned int isAddition : 1;
		unsigned int isPreIndex : 1;
		unsigned int : 3;
		unsigned int cond : 4;
	};
} BLOCK_DATA_TRANSFER;