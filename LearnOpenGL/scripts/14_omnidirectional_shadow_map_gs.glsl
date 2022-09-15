#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 uLightSpaceMatrices[6];

out vec4 ioFragPos; // FragPos from GS (output per "EmitVertex()" call).

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // Built-in variable that specifies to which face we render.

        for (int i = 0; i < 3; ++i) // For each triangle vertex.
        {
            ioFragPos = gl_in[i].gl_Position;
            gl_Position = uLightSpaceMatrices[face] * ioFragPos;

            EmitVertex();
        }

        EndPrimitive();
    }
}