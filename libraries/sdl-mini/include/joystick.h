 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Joystick emulation prototypes
  *
  * Copyright 1995 Bernd Schmidt
  */

extern void read_joystick (int nr, uint32_t *dir, int *button);
extern void close_joystick (void);
extern void set_joystickactive(void);

extern int nr_joysticks;
