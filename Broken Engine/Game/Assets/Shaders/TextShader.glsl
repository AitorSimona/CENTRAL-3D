#version 440 core
#define VERTEX_SHADER
#ifdef VERTEX_SHADER

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

out vec2 v_TexCoords; 

void main()
{
	gl_Position = u_Proj * u_View * u_Model * vec4 (a_Position, 1.0f);
	v_TexCoords = a_TexCoords;
}

#endif //VERTEX_SHADER

#define FRAGMENT_SHADER
		#ifdef FRAGMENT_SHADER

		in vec2 v_TexCoords;

		uniform sampler2D text;
		uniform vec3 textColor;

		out vec4 color;

		void main()
		{
			vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, v_TexCoords).r);
			color = vec4(textColor, 1.0) * sampled;
		}

		#endif //FRAGMENT_SHADER
