/* 
 * Minimalistic sound.c implementation for gp2x
 * (c) notaz, 2007
 */

// #define DEBUG_SOUND

#import <sys/types.h>
#import <sys/stat.h>
#import <fcntl.h>
#import <unistd.h>
#import <errno.h>

#import "sysconfig.h"
#import "sysdeps.h"
#import "config.h"
#import "uae.h"
#import "options.h"
#import "memory.h"
#import "debug_uae4all.h"
#import "audio.h"
#import "gensound.h"
#import "sound.h"
#import "custom.h"

#import "MMStopWatch.h"


extern uint32_t next_sample_evtime;

int produce_sound=0;
int changed_produce_sound=0;

//const uint32_t sound_rate=DEFAULT_SOUND_FREQ;

static uae_u16 sndbuffer[4][SNDBUFFER_LEN+32] UAE4ALL_ALIGN;
uint32_t n_callback_sndbuff, n_render_sndbuff;
uae_u16 *sndbufpt = sndbuffer[0];
uae_u16 *render_sndbuff = sndbuffer[0];

#ifndef PROFILER_UAE4ALL

int tablas_ajuste[8][9]=
{
	{ 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 },	// 0
	{ 3 , 4 , 3 , 4 , 3 , 4 , 3 , 4 , 3 },	// 1 
	{ 1 , 2 , 1 , 2 , 1 , 2 , 1 , 2 , 1 },	// 2 
	{ 1 , 0 , 1 , 0 , 1 , 0 , 1 , 0 , 1 },	// 3 
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },	// 4 
	{ 0 ,-1 , 0 ,-1 , 0 ,-1 , 0 ,-1 , 0 },	// 5 
	{-2 ,-1 ,-2 ,-1 ,-2 ,-1 ,-2 ,-1 ,-2 },	// 6 
	{-4 ,-3 ,-4 ,-3 ,-4 ,-3 ,-4 ,-3 ,-4 },	// 7
};

#else

int tablas_ajuste[8][9]=
{
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
	{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
};

#endif


int *tabla_ajuste=(int *)&tablas_ajuste[4];

#ifdef NO_SOUND


void finish_sound_buffer (void) {  }

int setup_sound (void) { sound_available = 0; return 0; }

void close_sound (void) { }

int init_sound (void) { return 0; }

void pause_sound (void) { }

void resume_sound (void) { }

void uae4all_init_sound(void) { }

void uae4all_play_click(void) { }

void uae4all_pause_music(void) { }

void uae4all_resume_music(void) { }

#else

#import "AudioQueueManager.h"

static CAudioQueueManager *g_audioQueue;

static int have_sound = 0;

#define SOUND_EVTIME_NORMAL	(MAXHPOS_PAL*312 *VBLANK_HZ_PAL*CYCLE_UNIT/DEFAULT_SOUND_FREQ)

void sound_default_evtime(void)
{
	int pal = beamcon0 & 0x20;
	switch(m68k_speed)
	{
		case 6:
			scaled_sample_evtime=(uint32_t)(MAXHPOS_PAL*MAXVPOS_PAL*VBLANK_HZ_PAL*CYCLE_UNIT/1.86)/DEFAULT_SOUND_FREQ;
			break;
			
		case 5:
		case 4: // ~4/3 234
			if (pal)
				scaled_sample_evtime=(MAXHPOS_PAL*244*VBLANK_HZ_PAL*CYCLE_UNIT)/DEFAULT_SOUND_FREQ; // ???
			else
				scaled_sample_evtime=(MAXHPOS_NTSC*255*VBLANK_HZ_NTSC*CYCLE_UNIT)/DEFAULT_SOUND_FREQ;
			break;
			
		case 3:
		case 2: // ~8/7 273
			if (pal)
				scaled_sample_evtime=(MAXHPOS_PAL*270*VBLANK_HZ_PAL*CYCLE_UNIT)/DEFAULT_SOUND_FREQ;
			else
				scaled_sample_evtime=(MAXHPOS_NTSC*255*VBLANK_HZ_NTSC*CYCLE_UNIT)/DEFAULT_SOUND_FREQ;
			break;
			
		case 1:
		default: // MAXVPOS_PAL?
			if (pal)
				//scaled_sample_evtime=(MAXHPOS_PAL*313*VBLANK_HZ_PAL*CYCLE_UNIT)/DEFAULT_SOUND_FREQ;
				scaled_sample_evtime = SOUND_EVTIME_NORMAL;
			else
				scaled_sample_evtime=(MAXHPOS_NTSC*MAXVPOS_NTSC*VBLANK_HZ_NTSC*CYCLE_UNIT)/DEFAULT_SOUND_FREQ + 1;
			break;
	}
	
	schedule_audio();
}


void finish_sound_buffer (void)
{
	static int usingTemp = 0;
#ifdef DEBUG_SOUND
	//dbg("sound.c : finish_sound_buffer");
	static Uint32 timer = GetTicks();
	static Uint32 counter = 0;
#endif
	
	if (!usingTemp)
		g_audioQueue->queueBuffer((short int*)render_sndbuff);
	
	sndbufpt = render_sndbuff = (uae_u16*)g_audioQueue->getNextBuffer();
	if (sndbufpt)
		usingTemp = 0;
	else {
		usingTemp = 1;
		sndbufpt = render_sndbuff = sndbuffer[0];
	}
		
#ifdef DEBUG_SOUND
	counter++;
	Uint32 now = GetTicks();
	if (now - timer > 1000) {
		printf("called finish_sound_buffer %dx / sec\r\n", counter);
		timer = now;
		counter=0;
	}
	//dbg(" sound.c : ! finish_sound_buffer");
#endif
}


/* Try to determine whether sound is available.  This is only for GUI purposes.  */
int setup_sound (void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : setup_sound");
#endif
	
    //if (gp2x_start_sound(DEFAULT_SOUND_FREQ, 16, 0) != 0)
	//    return 0;
	
#ifdef DEBUG_SOUND
    dbg(" sound.c : ! setup_sound");
#endif
    return 1;
}

static int open_sound (void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : open_sound");
#endif
	
	g_audioQueue = new CAudioQueueManager(DEFAULT_SOUND_FREQ, SNDBUFFER_LEN>>1, MonoSound);
	g_audioQueue->start();
	sndbufpt = render_sndbuff = (uae_u16*)g_audioQueue->getNextBuffer();
    //if (gp2x_start_sound(DEFAULT_SOUND_FREQ, 16, 0) != 0)
	//    return 0;
	
    sound_default_evtime();
	
    have_sound = 1;
    sound_available = 1;
	
#ifdef DEBUG_SOUND
    dbg(" sound.c : ! open_sound");
#endif
    return 1;
}

void close_sound (void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : close_sound");
#endif
    if (!have_sound)
		return;
	
    // testing shows that reopenning sound device is not a good idea on gp2x (causes random sound driver crashes)
    // we will close it on real exit instead
    //gp2x_stop_sound();
	g_audioQueue->stop();
	
    have_sound = 0;
	
#ifdef DEBUG_SOUND
    dbg(" sound.c : ! close_sound");
#endif
}

int init_sound (void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : init_sound");
#endif
	
    have_sound=open_sound();
	
#ifdef DEBUG_SOUND
    dbg(" sound.c : ! init_sound");
#endif
    return have_sound;
}

void pause_sound (void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : pause_sound");
#endif
	
	g_audioQueue->pause();
	
#ifdef DEBUG_SOUND
    dbg(" sound.c : ! pause_sound");
#endif
}

void resume_sound (void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : resume_sound");
#endif
	
    g_audioQueue->resume();
	
#ifdef DEBUG_SOUND
    dbg(" sound.c : ! resume_sound");
#endif
}

void set_sound_volume(float volume)
{
    if (have_sound) {
        g_audioQueue->setVolume(volume);
    }
}

float get_sound_volume(void)
{
    return g_audioQueue->getVolume();
}

void uae4all_init_sound(void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : uae4all_init_sound");
#endif
#ifdef DEBUG_SOUND
    dbg(" sound.c : ! uae4all_init_sound");
#endif
}

void uae4all_pause_music(void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : pause_music");
#endif
#ifdef DEBUG_SOUND
    dbg(" sound.c : ! pause_music");
#endif
}

void uae4all_resume_music(void)
{
#ifdef DEBUG_SOUND
    dbg("sound.c : resume_music");
#endif
#ifdef DEBUG_SOUND
    dbg(" sound.c : ! resume_music");
#endif
}

void uae4all_play_click(void)
{
}
#endif

