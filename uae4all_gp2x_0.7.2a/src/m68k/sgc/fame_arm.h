/*
 *  i68kcore.h
 *  M68kTests
 *
 *  Created by Stuart Carnie on 7/2/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */


#define u8	uint8_t
#define s8	signed char
#define u16	uint16_t
#define s16	signed short
#define u32	uint32_t
#define s32	signed int

/************************************/
/* General library defines          */
/************************************/

#ifndef M68K_OK
    #define M68K_OK 0
#endif
#ifndef M68K_RUNNING
    #define M68K_RUNNING 1
#endif
#ifndef M68K_NO_SUP_ADDR_SPACE
    #define M68K_NO_SUP_ADDR_SPACE 2
#endif
#ifndef M68K_DOUBLE_BUS_FAULT
    #define M68K_DOUBLE_BUS_FAULT -1
#endif
#ifndef M68K_INV_REG
    #define M68K_INV_REG -1
#endif

/* Hardware interrupt state */

#ifndef M68K_IRQ_LEVEL_ERROR
    #define M68K_IRQ_LEVEL_ERROR -1
#endif
#ifndef M68K_IRQ_INV_PARAMS
    #define M68K_IRQ_INV_PARAMS -2
#endif

/* Defines to specify hardware interrupt type */

#ifndef M68K_AUTOVECTORED_IRQ
    #define M68K_AUTOVECTORED_IRQ -1
#endif
#ifndef M68K_SPURIOUS_IRQ
    #define M68K_SPURIOUS_IRQ -2
#endif

#ifndef M68K_AUTO_LOWER_IRQ
	#define M68K_AUTO_LOWER_IRQ 1
#endif
#ifndef M68K_MANUAL_LOWER_IRQ
	#define M68K_MANUAL_LOWER_IRQ 0
#endif

/* Defines to specify address space */

#ifndef M68K_SUP_ADDR_SPACE
    #define M68K_SUP_ADDR_SPACE 0
#endif
#ifndef M68K_USER_ADDR_SPACE
    #define M68K_USER_ADDR_SPACE 2
#endif
#ifndef M68K_PROG_ADDR_SPACE
    #define M68K_PROG_ADDR_SPACE 0
#endif
#ifndef M68K_DATA_ADDR_SPACE
    #define M68K_DATA_ADDR_SPACE 1
#endif

typedef union {
	u8 B;
	s8 SB;
	u16 W;
	s16 SW;
} famec_union16;

typedef union {
	u8 B;
	s8 SB;
	u16 W;
	s16 SW;
	u32 D;
	s32 SD;
} famec_union32;

typedef struct {
	famec_union32 d[8];    // [r7,#0x00]
	famec_union32 a[8];    // [r7,#0x20]
	uint32_t pc;      // [r7,#0x40] Memory Base (.membase) + 68k PC
	uint8_t srh;    // [r7,#0x44] Status Register high (T_S__III)
	uint8_t unused; // [r7,#0x45] Unused
	uint8_t flags;  // [r7,#0x46] Flags (ARM order: ____NZCV) [68k order is XNZVC]
	uint8_t irq;    // [r7,#0x47] IRQ level
	uint32_t osp;     // [r7,#0x48] Other Stack Pointer (USP/SSP)
	uint32_t xc;      // [r7,#0x4c] Extend flag (bit29: ??X? _)
	uint32_t prev_pc; // [r7,#0x50] Set to start address of currently executed opcode + 2 (if enabled in config.h)
	uint32_t jumptab; // [r7,#0x54] Jump table pointer
	//int execinfo;		  // [r7,#0x58] bit: 0: stopped state, 1: trace state, 2: activity bit, 3: addr error, 4: fatal halt
	int cycles_to_execute;// [r7,#0x5c] Number of cycles to execute
	int cycles;           // [r7,#0x5c] Number of cycles remaining.
	int membase;          // [r7,#0x60] Memory Base (ARM address minus 68000 address)
	uint32_t (*checkpc)(uint32_t pc); // [r7,#0x64] called to recalc Memory Base+pc
	uint32_t (*read8  )(uint32_t a);  // [r7,#0x68]
	uint32_t (*read16 )(uint32_t a);  // [r7,#0x6c]
	uint32_t (*read32 )(uint32_t a);  // [r7,#0x70]
	void (*write8 )(uint32_t a,uint32_t d); // [r7,#0x74]
	void (*write16)(uint32_t a,uint32_t d); // [r7,#0x78]
	void (*write32)(uint32_t a,uint32_t d); // [r7,#0x7c]
	uint32_t (*fetch8 )(uint32_t a);  // [r7,#0x80]
	uint32_t (*fetch16)(uint32_t a);  // [r7,#0x84]
	uint32_t (*fetch32)(uint32_t a);  // [r7,#0x88]
	int  (*IrqCallback)(int int_level);       // [r7,#0x8c] optional irq callback function, see config.h
	void (*ResetCallback)(void);              // [r7,#0x90] if enabled in config.h, calls this whenever RESET opcode is encountered.
	int  (*UnrecognizedCallback)(void);       // [r7,#0x94] if enabled in config.h, calls this whenever unrecognized opcode is encountered.
	uint32_t internal[6];                 // [r7,#0x98] reserved for internal use, do not change.
} m68kcontext_t;

void m68k_release_timeslice(void);
//int  m68k_raise_irq(int level, int vector);
//int  m68k_lower_irq(int level);
void     m68k_init(void);
uint32_t m68k_reset(void);

#ifdef __cplusplus
extern "C"
#endif
void emulate();

extern m68kcontext_t m68k_context;

struct M68KCONTEXT_t {
	uae_u8			interrupts[8];	// only interrupts[0] is used
	uae_u32			cycles_counter;
	uae_u16			execinfo;	// will really only use bit7 (HALTED)
	uint32_t *		exception_handlers;
};

extern struct M68KCONTEXT_t M68KCONTEXT;

#define m68k_emulate(cycles1) \
{ \
	m68k_context.cycles_to_execute = (cycles1);  \
	emulate(); \
}
