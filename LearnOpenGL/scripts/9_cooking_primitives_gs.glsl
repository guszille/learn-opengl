#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
    vec2 ioTexCoords;
} gs_in[];

/* Built-in interface-block.
 *
 *  in gl_Vertex
 *  {
 *      vec4  gl_Position;
 *      float gl_PointSize;
 *      float gl_ClipDistance[];
 *  } gl_in[];
 */
uniform float uTime;
uniform float uMagnitude = 2.0;

out GS_OUT
{
    vec2 ioTexCoords;
} gs_out;

vec3 getNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);

   return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal)
{
    vec3 direction = normal * ((sin(uTime) + 1.0) / 2.0) * uMagnitude;

    // WARNING: "position + vec4(direction, 0.0)" causes issues at certain distances...

    return position + vec4(direction.xy, 0.0, 0.0);
} 

void main()
{
    vec3 normal = getNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    gs_out.ioTexCoords = gs_in[0].ioTexCoords;
    EmitVertex();

    gl_Position = explode(gl_in[1].gl_Position, normal);
    gs_out.ioTexCoords = gs_in[1].ioTexCoords;
    EmitVertex();

    gl_Position = explode(gl_in[2].gl_Position, normal);
    gs_out.ioTexCoords = gs_in[2].ioTexCoords;
    EmitVertex();

    EndPrimitive();
}