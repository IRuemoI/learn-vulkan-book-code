#pragma once

/*--------------------------------------------------------------------------------------
网格数据结构体和定点数据
--------------------------------------------------------------------------------------*/
struct VertexWithColor
{
    float x, y, z, w;// Vertex Position
    float r, g, b, a;// Color format Red, Green, Blue, Alpha
};

struct VertexWithUV
{
    float x, y, z, w;// Vertex Position
    float u, v;// Texture format U,V
};

//通过交替排列的数据来保存位置和颜色因袭
static const VertexWithColor triangleData[] = {
        {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0},
        {1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0},
        {-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0},
};
