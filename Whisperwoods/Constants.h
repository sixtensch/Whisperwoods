#ifndef H_CONSTANTS
#define H_CONSTANTS

#define CATCH_PROGRAM	1
#define WW_NODXGI

#define MAJOR_VERSION	0
#define MINOR_VERSION	1

#define DIR_ASSETS		L"./Assets/"
#define DIR_SHADERS		DIR_ASSETS L"Shaders/"
#define DIR_TEXTURES	DIR_ASSETS L"Textures/"
#define DIR_MODELS		DIR_ASSETS L"Models/"

#define WINDOW_NAME		"Whisperwoods"
#define WINDOW_WIDTH	1920
#define WINDOW_HEIGHT	1080

#define LIGHT_CAPACITY_DIR		1
#define LIGHT_CAPACITY_POINT	8
#define LIGHT_CAPACITY_SPOT		8

#define LIGHT_NEAR		0.05f
#define LIGHT_FAR		100.0f

// Values used by both render core .hlsli file for compute shaders.
#define BLOOM_MIP_LEVELS 8u
#define COMPUTE_GROUP_COUNT_X 80u
#define COMPUTE_GROUP_COUNT_Y 90u
	
#endif
