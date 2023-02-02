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
	Vec2 uv;
	Vec2 padding;
	VertexTextured() = default;
	VertexTextured(Vec3 p_pos, Vec3 p_nor, Vec3 p_tan, Vec3 p_bitan, Vec2 p_uv, Vec2 p_pad) :
		Vertex(p_pos, p_nor, p_tan, p_bitan),
		uv(p_uv),
		padding(p_pad) {};
};

struct VertexSkinned : VertexTextured
{
	Point4 bones;
	Vec4 weights;
	VertexSkinned() = default;
	VertexSkinned(Vec3 p_pos, Vec3 p_nor, Vec3 p_tan, Vec3 p_bitan, Vec2 p_uv, Vec2 p_pad, Point4 p_bones, Vec4 p_weights) :
		VertexTextured(p_pos, p_nor, p_tan, p_bitan, p_uv, p_pad),
		bones(p_bones),
		weights(p_weights) {};
};