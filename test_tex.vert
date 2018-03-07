#version 330                                                     
//Default.VERT                                                   
layout (location = 0) in vec3 vertexPosition_modelspace;
layout (location = 1) in vec2 uv;

out vec4 pos;         
out vec2 Frag_UV;
out float global_time;

uniform float time_in;



void main()                                                     
{                                                               
    gl_Position.xyz = vertexPosition_modelspace;                 
    gl_Position.w = 1.0;
    pos = gl_Position;  
    Frag_UV = uv;                                       
    global_time = time_in;
}