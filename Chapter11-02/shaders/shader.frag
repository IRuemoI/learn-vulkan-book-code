#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
//目前的实验其表明 必须是4的整数倍 否则奇怪现象
layout (binding = 1) uniform sampler2D tex;
layout (std140, set = 0, binding =2) uniform bufferVals { //输入的一致块
    float lightFactor;//亮度
} myFragBuffer;
layout (location = 0) in vec2 inTexCoor;
layout (location = 0) out vec4 outColor;
void main() {
    outColor=textureLod(tex, inTexCoor, 0.0);
}