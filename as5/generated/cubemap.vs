R"for_C++_include(#version 330

// Input vertex attributes
in vec3 vertexPosition;

// Input uniform values
uniform mat4 matProjection;
uniform mat4 matView;

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;

void main()
{
    // Calculate fragment position based on model transformations
    fragPosition = vertexPosition;

    // Calculate final vertex position
    gl_Position = matProjection*matView*vec4(vertexPosition, 1.0);
})for_C++_include"