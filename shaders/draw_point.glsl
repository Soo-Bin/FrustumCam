#version 430 core

#if defined(VERTEX_SHADER)

layout(location = 0)in vec3 pos;
layout(location = 1)in vec3 col;

uniform mat4 mvp;

out vec3 in_color;

void main()
{
    gl_Position = mvp * vec4(pos, 1.0);
    in_color = col;
}

#elif defined(FRAGMENT_SHADER)

out vec4 FragColor;
in vec3 in_color;

float near = 0.5;
float far = 20.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    FragColor = vec4(in_color, 1.0);
    
    // float depth = LinearizeDepth(gl_FragCoord.z) / far;
    // FragColor = vec4(vec3(depth), 1.0);
}
#endif
