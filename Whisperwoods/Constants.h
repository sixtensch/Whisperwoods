#ifndef H_CONSTANTS
#define H_CONSTANTS

#define CATCH_PROGRAM	1
//#define WW_NODXGI

#define MAJOR_VERSION	0
#define MINOR_VERSION	1

#define DIR_ASSETS		L"./Assets/"
#define DIR_SHADERS		DIR_ASSETS L"Shaders/"
#define DIR_TEXTURES	DIR_ASSETS L"Textures/"
#define DIR_MODELS		DIR_ASSETS L"Models/"

#define WINDOW_NAME		"Whisperwoods"
#define WINDOW_WIDTH	1280
#define WINDOW_HEIGHT	720

#define LIGHT_CAPACITY_DIR		1
#define LIGHT_CAPACITY_POINT	8
#define LIGHT_CAPACITY_SPOT		8

#define LIGHT_NEAR		0.05f
#define LIGHT_FAR		100.0f

#define BM_PIXELS_PER_UNIT	5
#define BM_PIXEL_SIZE		(1.0f / BM_PIXELS_PER_UNIT)
#define BM_MIN_SIZE			BM_PIXELS_PER_UNIT * 5

#define BM_C_EMPTY					0xffffff
#define BM_C_TERRAIN_OUT			0x00ff00
#define BM_C_TERRAIN_IN				0x007f00
#define BM_C_EXIT					0x000000

#endif
