/*
 *  iphone_main.h
 *  iAmiga
 *
 *  Created by Stuart Carnie on 5/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#define ROM_PATH_PREFIX

extern "C" char* get_rom_path();
extern "C" char* get_key_path();
extern "C" uint32_t get_chipmem_size();
extern "C" uint32_t get_fastmem_size();
extern "C" char* get_df_path(int drive);
extern "C" int getDisplayIsNTSC();
extern "C" void setDisplayIsNTSC(int isNTSC);

#import "sysdeps.h"
#import "gui.h"
