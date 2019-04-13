#include "fade.h"


void fade16(SDL_Surface *screen, uint16_t n)
{
	int i,total=screen->w*screen->h;
	SDL_LockSurface(screen);
	uint16_t rs=screen->format->Rshift;
	uint16_t gs=screen->format->Gshift;
	uint16_t bs=screen->format->Bshift;
	uint16_t rm=screen->format->Rmask;
	uint16_t gm=screen->format->Gmask;
	uint16_t bm=screen->format->Bmask;
	uint16_t rM=rm>>rs;
	uint16_t gM=gm>>gs;
	uint16_t bM=bm>>bs;
	uint16_t * buff=(uint16_t*)screen->pixels;
	for(i=0;i<total;i++)
	{
		register uint16_t r=(buff[i]&rm)>>rs;
		register uint16_t g=(buff[i]&gm)>>gs;
		register uint16_t b=(buff[i]&bm)>>bs;
//		if (n>r)
		if (n+r<rM)
			r+=n;
		else
			r=rM;
//		if (n>g)
		if (n+g<gM)
			g+=n;
		else
			g=gM;
//		if (n>b)
		if (n+b<bM)
			b+=n;
		else
			b=bM;
		buff[i]=(((r<<rs)&rm) | ((g<<gs)&gm) | ((b<<bs)&bm));
	}
	SDL_UnlockSurface(screen);
}
