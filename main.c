//********************************************************************************
//            Emulator Framework  By Bigboy                
//********************************************************************************
//                                                         
// This is a VERY simple framework based on the cool Star Simulation sample by 
// Sjeep (thanks!).It allows for a display of UP TO 512x512 with 256 colours. The 
// display is palattised for ease of use.
//                                                         
// This code is free to use, do whatever you like with it but if you do something 
// cool, please let me see it!!  Also, if you add a cool function, or fix a bug,
// please either add it via CVS or send it on to me.
//                                                         
// This framework also uses Sjeep's sound IRX to allow and simple way to do 
// sound emulation.                   
//                                                         
//                                                         	Enjoy!!
//                                                             Bigboy                   
//                                                         
//********************************************************************************
//  e-mail: dreamspec@hotmail.com                          
//                                                         
//********************************************************************************
// Credits: Bigboy - main construction, pulling it together
//                   Screen/GS stuff.                      
//          Sjeep  - Example code, Makefiles, Sound LIB    
//          pukko  - PAD stuff, Load module etc.                             
//                                                         
//          NAPLINK- Everyone that was invlolved in this   
//                   Great tool!!!                         
//                                                         
//                                                         
//  If your not on this list, and think you should be , or if I've made a mistake, 
//  then please let me know!!     
//                                                         
//********************************************************************************

// Ver		By			Comments
// 1.0		Bigboy		First usable version. Starts up, uploads textures and runs!
//                                                         
//                                                         
//
//*********************************************************************************

#include <tamtypes.h>
#include <string.h>
#include <kernel.h>
#include <sifrpc.h>
#include <stdlib.h>
#include "ps2.h"
#include "hw.h"
#include "hardware.h"
#include "pad.h"


#define FRAMERATE		(1)				 //control's frame rate.


	char	temparray[16];

	int		g_nWhichBuffer = 0;



// ******************************************************************************
//
// Init the PS2 machine
//
// ******************************************************************************
void	InitPS2( void )
{
	memset(&temparray[0],0,16);						// one of the libs requires memset


	DmaReset();


#ifdef	MASTER
	sif_reset_iop();
#endif
	sif_rpc_init(0);


	// detect system, and reset GS into that mode
	if(pal_ntsc() == 3) {
		printf("\nPAL detected.\n");
		initGraph(3);
	} else {
		printf("\nNTSC detected.\n");
		initGraph(2);
	}

	g_nWhichBuffer = 0;
	SetupScreen( g_nWhichBuffer );										// set up MY screen!!!

	printf("\nBigboys PS2 framework, based on Star Simulation by Sjeep\n");

    install_VRstart_handler();
	pad_initialize();


#ifdef	USESOUND
	LoadSoundModules();						// Load Sjeep's sound IRX - Thanks Sjeep!
#endif
}


// ******************************************************************************
//
// PS2 main startup
//
// ******************************************************************************

int main()
{

	InitPS2();


	//
	// Render something into the screen so we can see it **** TEST ****
	//               *********** REMOVE ME ***********
	//
	{
		int index = 0;
		int	x,y;
		int	col = 0;

		for(y=0;y<256;y++){
			for(x=0;x<320;x++){
				pScreen[index++]=col;
			}
				col++;
				col &=0x7;
		}
	}


	//
	// Main Emulator LOOP
	//
	//Load( "test", NULL );
	while(1) 
	{
		static	volatile	int	test;
		u64		iii;
		for(iii=0;iii<0x40000;iii++){				// Delay so I can see the timing bars
			test+=1;
		}
		TBAR(0x0000ff);
		UpdatePad();								// Read Joystick
		TBAR(0xffffff);

		//printf("joy=%d\n", g_nJoystick);
		// ------------------------
		// Emu code in here!!!
		// ------------------------

		//ProcessMain();


		// ------------------------
		// Finish Emucode
		// ------------------------
		TBAR(0xffff00);
		UpdateScreen();									// uploads+and renders new screen. (palette is also updated every frame for effects)
		TBAR(0);




		while (TestVRstart() < FRAMERATE); 					// wait for FRD number of vblanks
		ClearVRcount();
        g_nWhichBuffer ^= 1;
		SetupScreen( g_nWhichBuffer );					// FLIP!!!

	}
}
