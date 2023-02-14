#ifndef H_HLSL_CONSTANTS
#define H_HLSL_CONSTANTS



#include "Constants.h"

// Sampler states

#define REGISTER_SAMPLER_SYSTEM_0       register(s6)
#define REGISTER_SAMPLER_SYSTEM_1       register(s7)
#define REGISTER_SAMPLER_SYSTEM_2       register(s8)
#define REGISTER_SAMPLER_SYSTEM_3       register(s9)

#define REGISTER_SAMPLER_STANDARD       register(s10)
#define REGISTER_SAMPLER_POINT          register(s11)
#define REGISTER_SAMPLER_SHADOW         register(s12)

// Constant buffer views

#define REGISTER_CBV_USER_0             register(b1)
#define REGISTER_CBV_USER_1             register(b2)
#define REGISTER_CBV_USER_2             register(b3)
#define REGISTER_CBV_USER_3             register(b4)

#define REGISTER_CBV_SYSTEM_0           register(b5)
#define REGISTER_CBV_SYSTEM_1           register(b6)
#define REGISTER_CBV_SYSTEM_2           register(b7)

#define REGISTER_CBV_TESSELATION_INFO   register(b8)
#define REGISTER_CBV_VIEW_INFO          register(b9)
#define REGISTER_CBV_OBJECT_INFO        register(b10)
#define REGISTER_CBV_MATERIAL_INFO      register(b11)
#define REGISTER_CBV_CLIENT_INFO        register(b12)
#define REGISTER_CBV_SHADING_INFO       register(b13)



// Unordered access views

#define REGISTER_UAV_DEFAULT            register(u0)

#define REGISTER_UAV_USER_0             register(u1)
#define REGISTER_UAV_USER_1             register(u2)
#define REGISTER_UAV_USER_2             register(u3)

#define REGISTER_UAV_SYSTEM_0           register(u4)
#define REGISTER_UAV_SYSTEM_1           register(u5)

#define REGISTER_UAV_RENDER_TARGET      register(u6)

// Armature shader resource view

#define REGISTER_SRV_ARMATURE_MATRIX    register(t6)

// Shader resource views

#define REGISTER_SRV_TEX_DIFFUSE        register(t7)
#define REGISTER_SRV_TEX_SPECULAR       register(t8)
#define REGISTER_SRV_TEX_EMISSIVE       register(t9)
#define REGISTER_SRV_TEX_NORMAL         register(t10)

#define REGISTER_SRV_LIGHTS_DIRECTIONAL register(t11)
#define REGISTER_SRV_LIGHTS_POINT       register(t12)
#define REGISTER_SRV_LIGHTS_SPOT        register(t13)

#define REGISTER_SRV_COPY_SOURCE        register(t20)
#define REGISTER_SRV_PARTICLE_DATA      register(t21)

#define REGISTER_SRV_CUBE_MAP           register(t22)
#define REGISTER_SRV_SHADOW_DEPTH       register(t24)




#endif