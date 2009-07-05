
static __inline__ int LNAME (int spix, int dpix, int stoppos)
{
	
	
	/* CASO DUAL */
	
    unsigned short * __restrict__ buf = ((unsigned short *)xlinebuffer);
	
    if (bpldualpf) {
	    // OCS/ECS Dual playfield 
	    int *lookup = bpldualpfpri ? dblpf_ind2 : dblpf_ind1;
		
#if defined(DREAMCAST)
	    register int resto=(((unsigned)&buf[dpix])&0x1f);
	    if (resto)
			resto=0x20-resto;
	    if (resto>(stoppos-dpix))
			resto=(stoppos-dpix);
	    while (resto>0) {
			register unsigned short d = colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]];
			buf[dpix++]= d;
	    	spix += SRC_INC;
			resto-=2;
	    }
	    if (dpix >= stoppos)
			return spix;
		
	    unsigned int *d = (unsigned int *)(void *)(0xe0000000 | (((unsigned long)&buf[dpix]) & 0x03ffffe0));
	    unsigned short *s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]];
	    unsigned n=((stoppos-dpix)>>4);
	    {
		    register unsigned tmp=n<<4;
		    resto=(stoppos-dpix)-tmp;
		    dpix+=tmp;
	    }
	    QACR0 = ((((unsigned int)&buf[dpix])>>26)<<2)&0x1c;
	    QACR1 = ((((unsigned int)&buf[dpix])>>26)<<2)&0x1c;
	    while(n--) {
			//		    asm("pref @%0" : : "r" (s + 8));
		    register unsigned dato;
			
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]]; dato|=(((unsigned)*s)<<16);
		    d[0] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]];
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]]; dato|=(((unsigned)*s)<<16);
		    d[1] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]];
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]]; dato|=(((unsigned)*s)<<16);
		    d[2] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]];
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]]; dato|=(((unsigned)*s)<<16);
		    d[3] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]];
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]]; dato|=(((unsigned)*s)<<16);
		    d[4] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]];
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]]; dato|=(((unsigned)*s)<<16);
		    d[5] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]];
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]]; dato|=(((unsigned)*s)<<16);
		    d[6] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]];
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]]; dato|=(((unsigned)*s)<<16);
		    d[7] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]];
		    asm("pref @%0" : : "r" (d));
		    d+=8;
	    }
	    d = (unsigned int *)0xe0000000;
	    d[0] = d[8] = 0;
		
	    while (resto>0) {
		    register unsigned short d = colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]];
		    buf[dpix++]= d;
		    spix += SRC_INC;
		    resto--;
	    }
		
		
		
#else
	    while (dpix < stoppos) {
			register unsigned short d = colors_for_drawing.acolors[lookup[pixdata.apixels[spix]]];
			buf[dpix++] = d;
			spix += SRC_INC;
	    }
#endif
		
		
		
    } else {
		
		
		
		/* CASO NORMAL */
		
#if defined(DREAMCAST)
	    register int resto=(((unsigned)&buf[dpix])&0x1f);
	    if (resto)
			resto=0x20-resto;
	    if (resto>(stoppos-dpix))
			resto=(stoppos-dpix);
	    while (resto>0) {
	    	buf[dpix++]= (colors_for_drawing.acolors[pixdata.apixels[spix]]);
	    	spix += SRC_INC;
			resto-=2;
	    }
	    if (dpix >= stoppos)
			return spix;
		
	    unsigned int *d = (unsigned int *)(void *)
		(0xe0000000 | (((unsigned long)&buf[dpix]) & 0x03ffffe0));
	    unsigned short *s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]];
	    unsigned n=((stoppos-dpix)>>4);
	    {
		    register unsigned tmp=n<<4;
		    resto=(stoppos-dpix)-tmp;
		    dpix+=tmp;
	    }
	    QACR0 = ((((unsigned int)&buf[dpix])>>26)<<2)&0x1c;
	    QACR1 = ((((unsigned int)&buf[dpix])>>26)<<2)&0x1c;
	    while(n--) {
			//		    asm("pref @%0" : : "r" (s + 8));
		    register unsigned dato;
			
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]]; dato|=(((unsigned)*s)<<16);
		    d[0] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]];
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]]; dato|=(((unsigned)*s)<<16);
		    d[1] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]];
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]]; dato|=(((unsigned)*s)<<16);
		    d[2] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]];
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]]; dato|=(((unsigned)*s)<<16);
		    d[3] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]];
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]]; dato|=(((unsigned)*s)<<16);
		    d[4] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]];
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]]; dato|=(((unsigned)*s)<<16);
		    d[5] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]];
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]]; dato|=(((unsigned)*s)<<16);
		    d[6] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]];
		    dato=*s; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]]; dato|=(((unsigned)*s)<<16);
		    d[7] = dato; spix += SRC_INC; s = (unsigned short *)&colors_for_drawing.acolors[pixdata.apixels[spix]];
		    asm("pref @%0" : : "r" (d));
		    d+=8;
	    }
	    d = (unsigned int *)0xe0000000;
	    d[0] = d[8] = 0;
		
	    while (resto>0) {
		    buf[dpix++]= (colors_for_drawing.acolors[pixdata.apixels[spix]]);
		    spix += SRC_INC;
		    resto--;
	    }
		
#else
			
		// SGC: optimizations using the __restrict__ keyword
		long int * __restrict__ acolors = (long int *)&colors_for_drawing.acolors;
		uae_u8 * __restrict__ apixels = (uae_u8 *)&pixdata.apixels;
		while (dpix < stoppos) {
			buf[dpix++] = (acolors[apixels[spix]]);
			spix += SRC_INC;
		}
		
		/*
		while (dpix < stoppos) {
			buf[dpix++]= (colors_for_drawing.acolors[pixdata.apixels[spix]]);
			spix += SRC_INC;
		}
		 */
#endif
    }
    return spix;
}

#undef LNAME
#undef HDOUBLE
#undef SRC_INC
