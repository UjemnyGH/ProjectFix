#version 450 core

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Transpose;

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec4 i_Color;
layout(location = 2) in vec2 i_TexCoords;
layout(location = 3) in vec3 i_Normals;
layout(location = 4) in float i_TexID;

out vec4 v_Color;
out vec2 v_TexCoords;
out vec3 v_Normals;
out float v_TexID;
out vec4 v_Position;
out mat4 v_Model;

void main() 
{
    gl_Position = u_Projection * u_View * u_Transpose * i_Position;
    
    v_Color = i_Color;
    v_TexCoords = i_TexCoords;
    v_Normals = i_Normals;
    v_TexID = i_TexID;
    v_Position = i_Position;
    v_Model = u_Transpose;
}