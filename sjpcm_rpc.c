/*
    ---------------------------------------------------------------------
    sjpcm_rpc.c - SjPCM EE-side code. (c) Nick Van Veen (aka Sjeep), 2002
	---------------------------------------------------------------------

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. The name of the author(s) may not be used to endorse or promote products
       derived from this software without specific prior written permission.
	4. You do not redistribute this software or variations of this software
	   (in source code OR binary form) commercially.

    THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.

*/

#include <tamtypes.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <sifrpc.h>
#include <stdarg.h>
#include "sjpcm.h"
//#include "printf.h"

static unsigned sbuff[64] __attribute__((__section__(".bss"))) __attribute__((aligned(64)));
//__attribute__((aligned (64)));
static struct t_rpc_client_data cd0;

int sjpcm_inited = 0;
int pcmbufl, pcmbufr;
int bufpos;

/*void SjPCM_Puts(char *format, ...)
{
	static char buff[4096];
    va_list args;
    int rv;

	if(!sjpcm_inited) return;

    va_start(args, format);
    rv = vsnprintf(buff, 4096, format, args);

	memcpy((char*)(&sbuff[0]),buff,252);
	sif_call_rpc(&cd0,SJPCM_PUTS,0,(void*)(&sbuff[0]),252,(void*)(&sbuff[0]),252,0,0);
}*/

void SjPCM_Play()
{
	if(!sjpcm_inited) return;

	sif_call_rpc(&cd0,SJPCM_PLAY,0,(void*)(&sbuff[0]),0,(void*)(&sbuff[0]),0,0,0);
}

void SjPCM_Pause()
{
	if(!sjpcm_inited) return;

	sif_call_rpc(&cd0,SJPCM_PAUSE,0,(void*)(&sbuff[0]),0,(void*)(&sbuff[0]),0,0,0);
}

void SjPCM_Setvol(unsigned int volume)
{
	if(!sjpcm_inited) return;

	sbuff[5] = volume&0x3fff;
	sif_call_rpc(&cd0,SJPCM_SETVOL,0,(void*)(&sbuff[0]),64,(void*)(&sbuff[0]),64,0,0);
}

void SjPCM_Clearbuff()
{
	if(!sjpcm_inited) return;

	sif_call_rpc(&cd0,SJPCM_CLEARBUFF,0,(void*)(&sbuff[0]),0,(void*)(&sbuff[0]),0,0,0);
}

int SjPCM_Init( void )
{
	int i;
/*
	do {
        if (sif_bind_rpc(&cd0, SJPCM_IRX, 0) < 0) {
            return -1;
        }
        nopdelay();
    } while(!cd0.server);
*/
	while(1)
	{
		if (sif_bind_rpc( &cd0, SJPCM_IRX, 0) < 0) return -1; // bind error
 		if (cd0.server != 0) break;
    	i = 0x10000;
    	while(i--);
	}

	sif_call_rpc(&cd0,SJPCM_INIT,0,(void*)(&sbuff[0]),64,(void*)(&sbuff[0]),64,0,0);

	k_FlushCache(0);
	pcmbufl = sbuff[1];
	pcmbufr = sbuff[2];
	bufpos = sbuff[3];

	sjpcm_inited = 1;

	return 0;
}

// size should either be either 800 (NTSC) or 960 (PAL)
void SjPCM_Enqueue(short *left, short *right, int size, int wait)
{
    int i;
    struct t_sif_dma_transfer sdt;

    if (!sjpcm_inited) return;

    sdt.src = (void *)left;
    sdt.dest = (void *)(pcmbufl + bufpos);
    sdt.size = size*2;
    sdt.attr = 0;

	k_FlushCache(0);

    i = k_sceSifSetDma(&sdt, 1); // start dma transfer
    while ((wait != 0) && (k_sceSifDmaStat(i) >= 0)); // wait for completion of dma transfer

    sdt.src = (void *)right;
    sdt.dest = (void *)(pcmbufr + bufpos);
    sdt.size = size*2;
    sdt.attr = 0;

	k_FlushCache(0);

    i = k_sceSifSetDma(&sdt, 1);
    while ((wait != 0) && (k_sceSifDmaStat(i) >= 0));

	sbuff[0] = size;
	sif_call_rpc(&cd0,SJPCM_ENQUEUE,0,(void*)(&sbuff[0]),64,(void*)(&sbuff[0]),64,0,0);
	bufpos = sbuff[3];

}
