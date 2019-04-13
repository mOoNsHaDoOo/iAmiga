#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"

/*
 * UAE - The Un*x Amiga Emulator
 * 
 * Joystick emulation for Linux and BSD. They share too much code to
 * split this file.
 * 
 * Copyright 1997 Bernd Schmidt
 * Copyright 1998 Krister Walfridsson
 */
//  Changed by Emufr3ak on 17.11.2014
//
//  iUAE is free software: you may copy, redistribute
//  and/or modify it under the terms of the GNU General Public License as
//  published by the Free Software Foundation, either version 2 of the
//  License, or (at your option) any later version.
//
//  This file is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.



#include "sysconfig.h"
#include "sysdeps.h"

#include "uae.h"
#include "joystick.h"
#include "touchstick.h"

#include "vkbd.h"

#import <GameController/GameController.h>

int nr_joysticks;
int joystickselected;

CJoyStick g_touchStick;

void read_joystick(int nr, uint32_t *dir, int *button)
{
#ifndef MAX_AUTOEVENTS
    int left = 0, right = 0, top = 0, bot = 0;
    
    nr = !nr;
    
    *dir = 0;
    *button = 0;
    
    TouchStickDPadState dpadstate = nr == 0 ? g_touchStick.dPadStateP0() : g_touchStick.dPadStateP1();
    
    switch (dpadstate) {
		case DPadUp:
            top = 1; bot = 0; right = 0; left = 0;
			break;
		case DPadUpRight:
            top = 1; right = 1; bot = 0; left = 0;
			break;
		case DPadRight:
            right = 1; left = 0; top = 0; bot = 0;
			break;
		case DPadDownRight:
            bot = 1; right = 1; top = 0; left = 0;
			break;
		case DPadDown:
            bot = 1; top = 0; left = 0; right = 0;
			break;
		case DPadDownLeft:
            bot = 1; left = 1; top = 0; right = 0;
			break;
		case DPadLeft:
            left = 1; right = 0; bot = 0; top = 0;
			break;
		case DPadUpLeft:
            top = 1; left = 1; right = 0; bot = 0;
			break;
        case DPadCenter:
            top = 0; left = 0; bot = 0; right = 0;
	}
    
    if (left) top = !top;
    if (right) bot = !bot;
    *dir = bot | (right << 1) | (top << 8) | (left << 9);
    
    if(nr==0)
    {
        *button = g_touchStick.buttonOneStateP0();
    }
    else
    {
        *button = g_touchStick.buttonOneStateP1();
    }
    
#endif
}

#pragma clang diagnostic pop

