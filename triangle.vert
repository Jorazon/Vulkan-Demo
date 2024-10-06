#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

//vec2 positions[3] = vec2[3](
//    vec2( 0.0, -0.5), // Bottom vertex
//    vec2( 0.5,  0.5), // Top-right vertex
//    vec2(-0.5,  0.5)  // Top-left vertex
//);
//
//vec3 colors[3] = vec3[3](
//    vec3(1.0, 0.0, 0.0), // Bottom vertex
//    vec3(0.0, 1.0, 0.0), // Top-right vertex
//    vec3(0.0, 0.0, 1.0)  // Top-left vertex
//);

void main() {
    gl_Position = vec4(inPosition, 1.0);
    //gl_Position = vec4(positions[gl_VertexIndex], 1.0);
    fragColor = inColor;
    //fragColor = colors[gl_VertexIndex];
}
