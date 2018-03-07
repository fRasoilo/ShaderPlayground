#version 330

//TEXTURE FRAG SHADER

uniform sampler2D Texture;

//Interpolated Data from the Vertex Shader
in vec4 pos; 
in vec2 Frag_UV;
in float global_time;

//Output Data
out vec4 gl_FragColor;


void main()
{

    gl_FragColor = vec4(pos.x, pos.y, 1.0, global_time);
}
