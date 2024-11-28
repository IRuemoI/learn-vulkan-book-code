#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout (std140, set = 0, binding = 0) uniform bufferVals {
    mat4 mvp;
} myBufferVals;
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 0) out vec3 vcolor;
out gl_PerVertex {
    vec4 gl_Position;
};
void main() {
    gl_Position = myBufferVals.mvp * vec4(pos, 1.0);
    vcolor = color;
}
