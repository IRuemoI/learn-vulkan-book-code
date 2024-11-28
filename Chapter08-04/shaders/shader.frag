#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) out vec4 outColor;

layout (location = 0) in vec4 inLightQD;

layout (location = 1) in float vFogFactor;
void main()
{
    vec4 objectColor=vec4(0.95, 0.95, 0.95, 1.0);//物体颜色
    vec4 fogColor = vec4(0.97, 0.76, 0.03, 1.0);//雾的颜色
    if (vFogFactor != 0.0)
    { //如果雾因子为0，不必计算光照
        objectColor = objectColor*inLightQD;//计算光照之后物体颜色
        outColor = objectColor*vFogFactor + fogColor*(1.0-vFogFactor);//物体颜色和雾颜色插值计算最终颜色
    } else
    {
        outColor=fogColor;
    }
}