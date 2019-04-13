 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Sound emulation stuff
  *
  * Copyright 1995, 1996, 1997 Bernd Schmidt
  */

#define PERIOD_MAX UINT32_MAX

#ifndef UAE4ALL_ALIGN
#error UAE4ALL_ALIGN NO DEFINIDO
#endif

#ifdef NO_THREADS
#define PRE_SNDBUFFER_LEN (256)
#else
#define PRE_SNDBUFFER_LEN (1024)
#endif

#ifdef WIN32
#define SNDBUFFER_LEN (PRE_SNDBUFFER_LEN*2)
#else
#define SNDBUFFER_LEN (PRE_SNDBUFFER_LEN)
#endif

#define NO_AHI_CHANNELS

extern uint32_t sound_quality;

struct audio_channel_data{
    uint32_t per;
    uae_u8 dmaen, intreq2, data_written;
    uaecptr lc, pt;
    int wper;
    uint32_t wlen;
    int last_sample;
    uae_u16 dat, nextdat, len;
} UAE4ALL_ALIGN;

extern int audio_channel_current_sample[];
extern int audio_channel_vol[];
extern uint32_t audio_channel_adk_mask[];
extern int audio_channel_state[];
extern uint32_t audio_channel_evtime[];

extern struct audio_channel_data audio_channel[] UAE4ALL_ALIGN;

extern void aud0_handler (void);
extern void aud1_handler (void);
extern void aud2_handler (void);
extern void aud3_handler (void);

extern void AUDxDAT (int nr, uae_u16 value);
extern void AUDxVOL (int nr, uae_u16 value);
extern void AUDxPER (int nr, uae_u16 value);
extern void AUDxLCH (int nr, uae_u16 value);
extern void AUDxLCL (int nr, uae_u16 value);
extern void AUDxLEN (int nr, uae_u16 value);

extern int init_audio (void);
extern void ahi_install (void);
extern void audio_reset (void);
extern void update_audio (void);
extern void schedule_audio (void);
extern "C" void audio_evhandler (void);
extern void audio_channel_enable_dma (int n_channel);
extern void audio_channel_disable_dma (int n_channel);
extern void check_dma_audio(void);
extern void fetch_audio(void);
extern void update_adkmasks (void);
extern void set_audio_volume(float volume);
extern float get_audio_volume(void);
