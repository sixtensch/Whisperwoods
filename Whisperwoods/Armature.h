#pragma once

// Stuff required later for skeletal anim.
struct BoneWeightPair
{
	int bone;
	float weight;
	bool operator < (const BoneWeightPair& rhs)
	{
		return (weight < rhs.weight);
	}
};


struct Armature
{
	// TODO: Implement
};