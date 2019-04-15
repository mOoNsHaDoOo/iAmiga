
// Dave's Disa 68000 Disassembler

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ARM) || defined(GP32) || !defined (__WINS__)
#define CPU_CALL
#else
#define CPU_CALL __fastcall
#endif

#include <stdint.h>
    
extern uint32_t DisaPc;
extern char *DisaText; // Text buffer to write in

extern uint16_t (CPU_CALL *DisaWord)(uint32_t a);
int DisaGetEa(char *t,int ea,int size);

int DisaGet(void);

#ifdef __cplusplus
} // End of extern "C"
#endif
