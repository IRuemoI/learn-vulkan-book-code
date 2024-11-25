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

static const VertexWithColor squareData[] =
{
	{ -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0 },
	{  0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0 },
	{  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0 },
	{ -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0 },
};

uint16_t squareIndices[] = { 0,3,1, 3,2,1 }; // 6 indices
