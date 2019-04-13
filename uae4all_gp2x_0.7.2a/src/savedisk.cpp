#ifdef DREAMCAST
#include <kos.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "savedisk.h"

size_t savedisk_get_checksum(void *mem, size_t size)
{
	size_t i,ret=0;
	uint8_t *p=(uint8_t *)mem;
	for(i=0;i<size;i++)
		ret+=(i+1)*(((uint32_t)p[i])+1);
	return ret;
}



void savedisk_apply_changes(void *mem, void *patch, size_t patch_size)
{
	uint32_t *src=(uint32_t *)patch;
	uint8_t *dst=(uint8_t *)mem;
	uint32_t pos=0;
	patch_size/=sizeof(uint32_t);
	while(pos<patch_size)
	{
		uint32_t n=(src[pos++])*SAVEDISK_SLOT;
		memcpy((void *)&dst[n],(void *)&src[pos],SAVEDISK_SLOT);
		pos+=(SAVEDISK_SLOT/sizeof(uint32_t));
	}
}


size_t savedisk_get_changes_file(void *mem, size_t size, void *patch, char *filename)
{
	uint32_t ret=0;
	if (size%SAVEDISK_SLOT)
		size++;
	size/=SAVEDISK_SLOT;
	FILE *f=fopen(filename,"rb");
	if (f)
	{
		uint32_t pos=0;
		uint8_t *src=(uint8_t *)mem;
		uint32_t *dest=(uint32_t *)patch;
		while(size--)
		{
			uint32_t i=(ret/sizeof(uint32_t));
			uint32_t o=pos*SAVEDISK_SLOT;
			dest[i++]=pos;
			size_t n=fread((void *)&dest[i],1,SAVEDISK_SLOT,f);
			if (!n)
				break;
			if (memcmp((void *)&src[o],(void *)&dest[i],n))
			{
				memcpy((void *)&dest[i],(void *)&src[o],SAVEDISK_SLOT);
				ret+=sizeof(uint32_t)+SAVEDISK_SLOT;
			}
			pos++;
		}

		fclose(f);
	}
	return ret;
}

size_t savedisk_get_changes(void *mem, size_t size, void *patch, void *orig)
{
	uint32_t ret=0;
	if (size%SAVEDISK_SLOT)
		size++;
	size/=SAVEDISK_SLOT;
	if (orig)
	{
		uint32_t pos=0;
		uint8_t *src=(uint8_t *)mem;
		uint32_t *dest=(uint32_t *)patch;
		uint8_t *orig_p=(uint8_t *)orig;
		while(size --)
		{
			uint32_t i=(ret/sizeof(uint32_t));
			uint32_t o=pos*SAVEDISK_SLOT;
			dest[i++]=pos;
			if (memcmp((void *)&src[o],(void *)&orig_p[o],SAVEDISK_SLOT))
			{
				memcpy((void *)&dest[i],(void *)&src[o],SAVEDISK_SLOT);
				ret+=sizeof(uint32_t)+SAVEDISK_SLOT;
			}
			pos++;
		}
	}
	return ret;
}
