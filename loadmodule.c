/*********************************************************************
 * Sif functions for module (.irx) loading
 * Quite easy rev engineered from util demos..
 * These functions are not in any way complete or really tested!
 *                      -pukko
 *********************************************************************/

#include <tamtypes.h>
#include <sifrpc.h>
#include <sifcmd.h>
#include <string.h>
#include "loadmodule.h"

static struct t_rpc_client_data _lf __attribute__((aligned(16)));
static int _lf_initialized = 0;
// sifLoadModule rpc call var's
static char _senddata[512] __attribute__((aligned(16)));
static char _recvdata[512] __attribute__((aligned(16)));

extern	int	sif_bind_rpc( struct t_rpc_client_data *client, int rpc_number, int mode);
extern	int	sif_call_rpc( struct t_rpc_client_data *client, int rpc_number, int mode, void *send, int ssize, void *receive, int rsize, void (*end_function)( void *), void *end_param);


int
_lf_bind(int arg0)
{
    if(_lf_initialized) {
        return 0;
    }

    bzero(&_lf, sizeof(_lf));

    if (sif_bind_rpc(&_lf, 0x80000006, 0) < 0) {
        return -1;
    }

    if(_lf.server == NULL) {
        return -1;
    }

    _lf_initialized = 1;

    return 0;
}

int
_sifLoadModule(char *moduleName, int a, void *b, int c)
{

    strncpy(&_senddata[8], moduleName, 252);
    _senddata[259] = 0x00;
    _senddata[260] = 0x00;
    *(u32*)(&_senddata[0]) = 0x00000000; // (store word)

    if (sif_call_rpc(&_lf, c, 0, &_senddata, 512, &_recvdata, 4, 0, 0) < 0) {
        return -1;
    }

    /* TODO: Finish this rev.. b is always 0, but...
     *    if(b==0) {
     *    }
     *    else {
     *
     *    }
     */
    return *(int *)(&_recvdata[0]);
}

/*
int sif_get_iop_addr(u32 src, void *dst)
{
	int i=0,len=0,ret=0;
  u8  *pkt;

  if(!_lf_initialized)
		return -1;

  bzero(_senddata,sizeof(_senddata));
	bzero(_recvdata,sizeof(_recvdata)); 
  
  // build packet
  pkt = _senddata; 
  PUSHDATA( u32, pkt, (u32)src, i);
  pkt += i; len += i;
  PUSHDATA( u32, pkt, 2, i);
  pkt += i; len += i;
  	
	if ((ret = sif_call_rpc( &_lf, 3, 0, _senddata, len, _recvdata, 0x20, 0, 0)) < 0)
	  return -1;

	POPDATA( u32, UNCACHED_SEG(_recvdata), *((u32*)dst), i);  

	return 0;
}
*/
