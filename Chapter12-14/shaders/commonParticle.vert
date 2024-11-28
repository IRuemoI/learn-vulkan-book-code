#version 450
#extension GL_ARB_separate_shader_objects : enable //开启separate_shader_objects
#extension GL_ARB_shading_language_420pack : enable //开启shading_language_420pack
layout (push_constant) uniform constantVals { //推送常量块
    mat4 mvp;//总变换矩阵
} myConstantVals;
layout (std140, set = 0, binding =0) uniform bufferVals { //一致块
    float maxLifeSpan;//最大生命周期
} myBufferVals;
layout (location = 0) in vec2 pos;//输入的顶点位置
layout (location = 1) in float xSpeed;//当前X方向速度
layout (location = 2) in float curLifespan;//当前粒子生命周期
layout (location = 3) in vec2 inTexCoor;//输入的纹理坐标
layout (location = 0) out vec2 outTexCoor;//输出到片元着色器的纹理坐标
layout (location = 1) out float sjFactor;//输出的衰减因子
layout (location = 2) out vec4 vPosition;//输出到片元着色器的顶点位置
out gl_PerVertex { //输出接口块
    vec4 gl_Position;//内建变量gl_Position
};
void main() {
    outTexCoor = inTexCoor;
    gl_Position = myConstantVals.mvp* vec4(pos.x, pos.y, 0.0, 1.0);
    vPosition=vec4(pos.x, pos.y, 0.0, curLifespan);//计算顶点位置属性，并将其传递给片元着色器
    sjFactor=(myBufferVals.maxLifeSpan-curLifespan)/myBufferVals.maxLifeSpan;//计算总衰减因子，并将其传递给片元着色器
}
