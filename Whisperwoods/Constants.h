#ifndef H_CONSTANTS
#define H_CONSTANTS

//#define CATCH_PROGRAM	0
#define WW_NODXGI

#define MAJOR_VERSION	0
#define MINOR_VERSION	1

#define DIR_ASSETS		L"./Assets/"
#define DIR_SHADERS		DIR_ASSETS L"Shaders/"
#define DIR_TEXTURES	DIR_ASSETS L"Textures/"
#define DIR_MODELS		DIR_ASSETS L"Models/"
#define DIR_LEVELS		DIR_TEXTURES L"Maps/"

#define WINDOW_NAME		"Whisperwoods"
#define WINDOW_WIDTH	1920
#define WINDOW_HEIGHT	1080

#define LIGHT_CAPACITY_DIR		1
#define LIGHT_CAPACITY_POINT	8
#define LIGHT_CAPACITY_SPOT		8

#define LIGHT_NEAR		0.05f
#define LIGHT_FAR		100.0f

#define ENEMY_CONE_INFO_CAPACITY 8

#define BM_PIXELS_PER_UNIT	5
#define BM_PIXEL_SIZE		(1.0f / BM_PIXELS_PER_UNIT)
#define BM_MIN_SIZE			BM_PIXELS_PER_UNIT * 5

#define BM_C_EMPTY					0xffffff
#define BM_C_TERRAIN_OUT			0x00ff00
#define BM_C_TERRAIN_IN				0x007f00
#define BM_C_EXIT					0x000000

// Values used by both render core and .hlsli file for compute shaders.
#define BLOOM_MIP_LEVELS 4u
#define COMPUTE_GROUP_COUNT_X 80u
#define COMPUTE_GROUP_COUNT_Y 90u
	
#define DEG2RAD (DirectX::XM_PI / 180)
#define RAD2DEG (180 / DirectX::XM_PI)

#endif
