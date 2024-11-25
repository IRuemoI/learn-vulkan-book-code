#pragma once

/*--------------------------------------------------------------------------------------
Mesh data strucure and  and VertexFormat Data
--------------------------------------------------------------------------------------*/
struct VertexWithColor
{
    float x, y, z, w;   // Vertex Position
    float r, g, b, a;   // Color format Red, Green, Blue, Alpha
};

struct VertexWithUV
{
    float x, y, z, w;   // Vertex Position
    float u, v;         // Texture format U,V
};

static const VertexWithColor geometryData[] =
{
	{  1, -1, -1, 1.0f,		0.f, 0.f, 0.f, 1.0f },
	{ -1, -1, -1, 1.0f,		1.f, 0.f, 0.f, 1.0f },
	{  1,  1, -1, 1.0f,		0.f, 1.f, 0.f, 1.0f },
	{  1,  1, -1, 1.0f,		0.f, 1.f, 0.f, 1.0f },
	{ -1, -1, -1, 1.0f,		1.f, 0.f, 0.f, 1.0f },
	{ -1,  1, -1, 1.0f,		1.f, 1.f, 0.f, 1.0f },

	{  1, -1, 1, 1.0f,		0.f, 0.f, 1.f, 1.0f },
	{  1,  1, 1, 1.0f,		0.f, 1.f, 1.f, 1.0f },
	{ -1, -1, 1, 1.0f,		1.f, 0.f, 1.f, 1.0f },
	{ -1, -1, 1, 1.0f,		1.f, 0.f, 1.f, 1.0f },
	{  1,  1, 1, 1.0f,		0.f, 1.f, 1.f, 1.0f },
	{ -1,  1, 1, 1.0f,		1.f, 1.f, 1.f, 1.0f },

	{ 1, -1,  1, 1.0f,		1.f, 1.f, 1.f, 1.0f },
	{ 1, -1, -1, 1.0f,		1.f, 1.f, 0.f, 1.0f },
	{ 1,  1,  1, 1.0f,		1.f, 0.f, 1.f, 1.0f },
	{ 1,  1,  1, 1.0f,		1.f, 0.f, 1.f, 1.0f },
	{ 1, -1, -1, 1.0f,		1.f, 1.f, 0.f, 1.0f },
	{ 1,  1, -1, 1.0f,		1.f, 0.f, 0.f, 1.0f },

	{ -1, -1,  1, 1.0f,		0.f, 1.f, 1.f, 1.0f },
	{ -1,  1,  1, 1.0f,		0.f, 0.f, 1.f, 1.0f },
	{ -1, -1, -1, 1.0f,		0.f, 1.f, 0.f, 1.0f },
	{ -1, -1, -1, 1.0f,		0.f, 1.f, 0.f, 1.0f },
	{ -1,  1,  1, 1.0f,		0.f, 0.f, 1.f, 1.0f },
	{ -1,  1, -1, 1.0f,		0.f, 0.f, 0.f, 1.0f },

	{  1, 1, -1, 1.0f,		1.f, 1.f, 1.f, 1.0f },
	{ -1, 1, -1, 1.0f,		0.f, 1.f, 1.f, 1.0f },
	{  1, 1,  1, 1.0f,		1.f, 1.f, 0.f, 1.0f },
	{  1, 1,  1, 1.0f,		1.f, 1.f, 0.f, 1.0f },
	{ -1, 1, -1, 1.0f,		0.f, 1.f, 1.f, 1.0f },
	{ -1, 1,  1, 1.0f,		0.f, 1.f, 0.f, 1.0f },

	{  1, -1, -1, 1.0f,		1.f, 0.f, 1.f, 1.0f },
	{  1, -1,  1, 1.0f,		1.f, 0.f, 0.f, 1.0f },
	{ -1, -1, -1, 1.0f,		0.f, 0.f, 1.f, 1.0f },
	{ -1, -1, -1, 1.0f,		0.f, 0.f, 1.f, 1.0f },
	{  1, -1,  1, 1.0f,		1.f, 0.f, 0.f, 1.0f },
	{ -1, -1,  1, 1.0f,		0.f, 0.f, 0.f, 1.0f },
};
