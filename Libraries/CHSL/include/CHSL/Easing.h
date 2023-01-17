#pragma once

namespace cs
{
	typedef float (*EaseFunction)(float);


	// Easing function types.
	// Easing in = function connects smoothly to the start, accelerating
	// Easing out = smooths out towards the end, decelerating
	enum EaseType
	{	
		EaseTypeLinear,
		EaseTypeIn,
		EaseTypeOut,
		EaseTypeInOut,
		EaseTypeInOutResponsive,

		EaseTypeInSin,
		EaseTypeOutSin,
		EaseTypeInOutSin,

		EaseTypeInCubic,
		EaseTypeOutCubic,

		EaseTypeInQuad,
		EaseTypeOutQuad,

		EaseTypeInCircular,
		EaseTypeOutCircular,

		EaseTypeInBounce,
		EaseTypeOutBounce,

		EaseTypeInElastic,
		EaseTypeOutElastic,

		EaseTypeCount // <-- Keep last
	};

	// Easing functions
	// Range from 0.0f-1.0f, not clamped
	namespace Ease
	{
		
		EaseFunction getFunction(EaseType easeType);

		float linear(float x);
		float in(float x);
		float out(float x);
		float inOut(float x);
		float inOutResponsive(float x);
		float inSin(float x);
		float outSin(float x);
		float inOutSin(float x);
		float inCubic(float x);
		float outCubic(float x);
		float inQuad(float x);
		float outQuad(float x);
		float inCircular(float x);
		float outCircular(float x);
		float inBounce(float x);
		float outBounce(float x);
		float inElastic(float x);
		float outElastic(float x);

	}
}