#pragma once

#include "Constants.h"

#include <vector>

#include <CHSL/WindowsExt.h>

// DX
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl.h>
namespace dx = DirectX;
namespace wrl = Microsoft::WRL;
using Microsoft::WRL::ComPtr;

#define CHSL_LINEAR
#define CHSL_DX
//#define CHSL_EXCEPT	// We replace CHSL exceptions with our own system
#include <CHSL.h>

#include <memory>

using std::unique_ptr;
using std::shared_ptr;
using std::make_unique;
using std::make_shared;

// Debugging system
#include "Debug.h"
