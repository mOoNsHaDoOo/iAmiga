
// Cyclone 68000 Emulator - Header File

// (c) Copyright 2004 Dave, All rights reserved.
// (c) 2005-2007 notaz
// Cyclone 68000 is free for non-commercial use.

// For commercial use, separate licencing terms must be obtained.


#ifndef __CYCLONE_H__
#define __CYCLONE_H__

#ifdef __cplusplus
extern "C" {
#endif

extern int CycloneVer; // Version number of library

struct Cyclone
{
  uint32_t d[8];    // [r7,#0x00]
  uint32_t a[8];    // [r7,#0x20]
  uint32_t pc;      // [r7,#0x40] Memory Base (.membase) + 68k PC
  uint8_t srh;    // [r7,#0x44] Status Register high (T_S__III)
  uint8_t unused; // [r7,#0x45] Unused
  uint8_t flags;  // [r7,#0x46] Flags (ARM order: ____NZCV) [68k order is XNZVC]
  uint8_t irq;    // [r7,#0x47] IRQ level
  uint32_t osp;     // [r7,#0x48] Other Stack Pointer (USP/SSP)
  uint32_t xc;      // [r7,#0x4c] Extend flag (bit29: ??X? _)
  uint32_t prev_pc; // [r7,#0x50] Set to start address of currently executed opcode + 2 (if enabled in config.h)
  uint32_t jumptab; // [r7,#0x54] Jump table pointer
  int state_flags;      // [r7,#0x58] bit: 0: stopped state, 1: trace state, 2: activity bit, 3: addr error, 4: fatal halt
  int cycles;           // [r7,#0x5c] Number of cycles to execute - 1. Updates to cycles left after CycloneRun()
  int membase;          // [r7,#0x60] Memory Base (ARM address minus 68000 address)
  uint32_t (*checkpc)(uint32_t pc); // [r7,#0x64] called to recalc Memory Base+pc
  uint32_t (*read8  )(uint32_t a);  // [r7,#0x68]
  uint32_t (*read16 )(uint32_t a);  // [r7,#0x6c]
  uint32_t (*read32 )(uint32_t a);  // [r7,#0x70]
  void (*write8 )(uint32_t a,uint8_t  d); // [r7,#0x74]
  void (*write16)(uint32_t a,uint16_t d); // [r7,#0x78]
  void (*write32)(uint32_t a,uint32_t   d); // [r7,#0x7c]
  uint32_t (*fetch8 )(uint32_t a);  // [r7,#0x80]
  uint32_t (*fetch16)(uint32_t a);  // [r7,#0x84]
  uint32_t (*fetch32)(uint32_t a);  // [r7,#0x88]
  int  (*IrqCallback)(int int_level);       // [r7,#0x8c] optional irq callback function, see config.h
  void (*ResetCallback)(void);              // [r7,#0x90] if enabled in config.h, calls this whenever RESET opcode is encountered.
  int  (*UnrecognizedCallback)(void);       // [r7,#0x94] if enabled in config.h, calls this whenever unrecognized opcode is encountered.
  uint32_t internal[6];                 // [r7,#0x98] reserved for internal use, do not change.
};

// Initialize. Used only if Cyclone was compiled with compressed jumptable, see config.h
extern "C" void CycloneInit(void);

// Run cyclone. Cycles should be specified in context (pcy->cycles)
extern "C" void CycloneRun(struct Cyclone *pcy);

// Utility functions to get and set SR
void CycloneSetSr(struct Cyclone *pcy, uint32_t sr);
uint32_t CycloneGetSr(const struct Cyclone *pcy);

// Generates irq exception if needed (if pcy->irq > mask).
// Returns cycles used for exception if it was generated, 0 otherwise.
int CycloneFlushIrq(struct Cyclone *pcy);

// Functions for saving and restoring state.
// CycloneUnpack() uses checkpc(), so it must be initialized.
// save_buffer must point to buffer of 128 (0x80) bytes of size.
void CyclonePack(const struct Cyclone *pcy, void *save_buffer);
void CycloneUnpack(struct Cyclone *pcy, const void *save_buffer);

// genesis: if 1, switch to normal TAS handlers
void CycloneSetRealTAS(int use_real);


// These values are special return values for IrqCallback.

// Causes an interrupt autovector (0x18 + interrupt level) to be taken.
// This happens in a real 68K if VPA or AVEC is asserted during an interrupt
// acknowledge cycle instead of DTACK (the most common situation).
#define CYCLONE_INT_ACK_AUTOVECTOR    -1

// Causes the spurious interrupt vector (0x18) to be taken
// This happens in a real 68K if BERR is asserted during the interrupt
// acknowledge cycle (i.e. no devices responded to the acknowledge).
#define CYCLONE_INT_ACK_SPURIOUS      -2


#ifdef __cplusplus
} // End of extern "C"
#endif

#endif // __CYCLONE_H__

