#pragma once
#include <CHSL/Vector.h>

struct Vertex
{
	Vec3 pos;
	Vec3 nor;
	Vec3 tan;
	Vec3 bitan;
	Vertex() = default;
	Vertex(Vec3 p_pos, Vec3 p_nor, Vec3 p_tan, Vec3 p_bitan) :
		pos(p_pos),
		nor(p_nor),
		tan(p_tan),
		bitan(p_bitan) {};
};

struct VertexTextured : Vertex
{
	Vec4 uv;
	VertexTextured() = default;
	VertexTextured(Vec3 p_pos, Vec3 p_nor, Vec3 p_tan, Vec3 p_bitan, Vec2 p_uv, Vec2 p_pad) :
		Vertex(p_pos, p_nor, p_tan, p_bitan),
		uv(p_uv) {};
	bool operator==(const VertexTextured& other);
};

struct VertexRigged : VertexTextured
{
	int bones[4];
	float weights[4];
	VertexRigged() = default;
	VertexRigged(Vec3 p_pos, Vec3 p_nor, Vec3 p_tan, Vec3 p_bitan, Vec2 p_uv, Vec2 p_pad, Point4 p_bones, Vec4 p_weights) :
		VertexTextured(p_pos, p_nor, p_tan, p_bitan, p_uv, p_pad)
	{
		bones[0] = p_bones.x;
		bones[1] = p_bones.y;
		bones[2] = p_bones.z;
		bones[3] = p_bones.w;
		weights[0] = p_weights.x;
		weights[1] = p_weights.y;
		weights[2] = p_weights.z;
		weights[3] = p_weights.w;
	};

	bool operator==(const VertexRigged& other);
};