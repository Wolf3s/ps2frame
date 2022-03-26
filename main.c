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
// 1.1      Bigboy      Added the palette here, and the debug text support example
//                                                         
//
//*********************************************************************************

#include <tamtypes.h>
#include <string.h>
#include <kernel.h>
#include <sifrpc.h>
#include <stdlib.h>
#include <stdio.h>
#include "ps2.h"
#include "hw.h"
#include "hardware.h"
#include "pad.h"

#include <ctype.h>


//FILE *fopen (filename, mode);


#define FRAMERATE		(1)				 //control's frame rate.


	char	temparray[16];
	int		g_nWhichBuffer = 0;

	unsigned char NewPalette[]=
	{
			0x00,0x00,0x00,0x70,			// black
			0x00,0x00,0x70,0x70,			// blue
			0x70,0x00,0x00,0x70,			// red
			0x70,0x00,0x70,0x70,			// magenta
			0x00,0x70,0x00,0x70,			// green
			0x00,0x70,0x70,0x70,			// cyan
			0x70,0x70,0x00,0x70,			// yellow
			0x70,0x70,0x70,0x70,			// white

			0x00,0x00,0x00,0x80,			// black			*BRIGHT*
			0x00,0x00,0xa0,0x80,			// blue
			0xa0,0x00,0x00,0x80,			// red
			0xa0,0x00,0xa0,0x80,			// magenta
			0x00,0xa0,0x00,0x80,			// green
			0x00,0xa0,0xa0,0x80,			// cyan
			0xa0,0xa0,0x00,0x80,			// yellow
			0xa0,0xa0,0xa0,0x80,			// white
	};

	int		g_nSound=0;
	u8		SoundBuffer[ 2049 ] __attribute__((__section__(".bss"))) __attribute__((aligned(16)));



// ******************************************************************************
//
// PS2 main startup
//
// ******************************************************************************
void UpdateSound( void )
{
#ifdef	SOUND
		// ******************************************************************************
		//
		//		If sjpcm.irx was loaded successfully, you may then call SjPCM_Init() to
		//		initialise the module on the iop. SjPCM is designed to be sent pcm data
		//		every frame. On a pal system, send 960 samples per frame. On an ntsc system,
		//		send 800 samples per frame. The format of the PCM is: 16-bit signed pcm, at
		//		48000Hz. PCM is sent to SjPCM with the SjPCM_Enqueue() function.
		//	
		//		void SjPCM_Enqueue(short *left, short *right, int size, int wait);
		//	
		//		- left/right are pointers to the left/right pcm data.
		//		- size is the amount of *samples* to be sent.
		//		- set wait to 1 if you want the function to wait for each dma transfer to end
		//		  (seperate transfer for left & right buffers).
		//
		// ******************************************************************************

		if( g_nSound==1 ) SjPCM_Enqueue( (void*) &SoundBuffer[0], (void*) &SoundBuffer[0], 960, 1);
#endif
}

// ******************************************************************************
//
// Init the PS2 machine
//
// ******************************************************************************
void	InitPS2( void )
{
	memset(&temparray[0],0,16);						// one of the libs requires memset

	DmaReset();
	sif_rpc_init(0);


#ifdef	MASTER
	sif_reset_iop();
#endif
	sif_rpc_init(0);


	// detect system, and reset GS into that mode
	if(pal_ntsc() == 3) {
		//printf("\nPAL detected.\n");
		initGraph(3);
		g_NTSC=0;
	} else {
		//printf("\nNTSC detected.\n");
		initGraph(2);
		g_NTSC=1;
	}

	pad_initialize();

#ifdef	SOUND
	LoadSoundModules();
#endif

//	initGraph(2);
//	g_NTSC=1;


	printf("\nBigboys PS2 Framework V1.1\n");

//	DisplayTxt( "install_VRstart_handler()" );
//    install_VRstart_handler();

#ifdef	SOUND
	if(SjPCM_Init() < 0) {
		g_nSound = 0;
		printf("SjPCM Bind failed!!\n" );
	}
	else{
		g_nSound = 1;
	}
#endif

}


// ******************************************************************************
//
// PS2 main startup
//
// ******************************************************************************

int main()
{
	int		i;
	u32		col;
	u32		index;

	InitPS2();

#ifdef	SOUND
	if( g_nSound == 1 ) {
		SjPCM_Clearbuff();
		SjPCM_Play();
		SjPCM_Setvol(0x1fff);		// up to $3fff
	}
#endif


	index=0;
	for(i=0;i<16;i++)
	{
		col = NewPalette[index++]<<16;
		col |= NewPalette[index++]<<8;
		col |= NewPalette[index++];
		SetPaletteEntry(col,i);
	}
	SetPaletteEntry(0x80ffffff,255);
	


	//
	// Main Emulator LOOP
	//
	//pFileBuffer = Load( "test.dat" );					// example load file
	i=0;
	while(1) 
	{
		static	volatile	int	test;
		u64		iii;
		for(iii=0;iii<0x40000;iii++){					// Delay so I can see the timing bars
			test+=1;
		}
		TBAR(0x0000ff);
		UpdatePad();									// Read Joystick
		TBAR(0xffffff);

		//printf("joy=%d\n", g_nJoystick);
		// ------------------------
		// Emu code in here!!!
		// ------------------------

		//ProcessMain();


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

		g_nFlags = 0;
		g_nInk = 0;										// set ink to BLACK
		Print( i-1,i, "Hello World!!");					// draw debug text around main text in BLACK
		Print( i,i-1, "Hello World!!");
		Print( i+1,i, "Hello World!!");
		Print( i,i+1, "Hello World!!");
		g_nInk = 255;									// set INK to WHITE
		Print( i,i, "Hello World!!");						// draw debug text in WHITE
		i++;
		i&=0x7f;


		UpdateSound();

		// ------------------------
		// Finish Emucode
		// ------------------------
		TBAR(0xffff00);
		UpdateScreen();									// uploads+and renders new screen. (palette is also updated every frame for effects)
		TBAR(0);




		while (TestVRstart() < FRAMERATE); 				// wait for FRD number of vblanks
		ClearVRcount();
        g_nWhichBuffer ^= 1;
		SetupScreen( g_nWhichBuffer );					// FLIP!!!

	}
}
