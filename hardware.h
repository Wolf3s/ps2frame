// hw.h

#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#ifdef __cplusplus
extern "C" {
#endif


#include	"ps2.h"

extern	u64		DMABuffer[];
extern	PU8		pScreen;
extern	int		g_nScreen_X;	// These can go UP TO 512
extern	int		g_nScreen_Y;
extern	int		g_nFiltered;	// set to 1 to bi-linear filter the screen
extern	int		g_nDisplayWidth;
extern	int		g_nDisplayHeight;
extern	int		g_nFileSize;

extern	u8		g_nPaper;			// PAPER for text draing
extern	u8		g_nInk;				// INK for text drawing
extern	int		g_nFlags;			// 1 = DONT draw background			(Text drawing flags)

extern	int		g_NTSC;				// PAL/NTSC mode?


extern	U8 Palette[];

void	ClearScreen( U32 ARGB );
void	SetupScreen( int nDisplayBuffer );
void	RenderQuad2( void );
void	RenderQuad( int xx );

void	DrawBitmap( int x, int y, int w, int h );

void	UploadPalette( void* Address, U32 Xsize, U32 Ysize, U32 VAdd );
void	UploadScreen( U32 Xsize, U32 Ysize, U32 VAdd );
void	UpdateScreen( void );
void	SetPaletteEntry( U32 ARGB, U32 index );

u8*		LoadInto( char* pszName, u8* pBuffer );
u8*		Load( char* pszName );
void 	LoadSoundModules(void);









#ifdef __cplusplus
}
#endif

#endif
