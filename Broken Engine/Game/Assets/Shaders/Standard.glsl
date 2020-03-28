#version 440 core 

#define VERTEX_SHADER 
#ifdef VERTEX_SHADER 

//Layout Daya
layout (location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout (location = 3) in vec2 texCoord;

//Uniforms
uniform vec3 Color = vec3(1.0);
uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

//Varyings
out vec2 v_TexCoord;
out vec3 v_Color;

void main()
{ 
	gl_Position = projection * view * model_matrix * vec4 (position, 1.0);
	v_Color = Color; 
	v_TexCoord = texCoord; 
}

#endif //VERTEX_SHADER


#define FRAGMENT_SHADER 
#ifdef FRAGMENT_SHADER 

#define MAX_SHADER_LIGHTS 50

//Uniforms
uniform int Texture;
uniform float u_Shininess;
uniform sampler2D ourTexture;
uniform sampler2D SpecText;

//Light Uniforms
struct BrokenLight
{
	vec3 dir;
	vec3 pos;
	vec3 color;

	float intensity;

	vec3 attenuationKLQ;
	vec2 InOutCutoff;

	int LightType;
};

uniform BrokenLight u_BkLights[MAX_SHADER_LIGHTS] = BrokenLight[MAX_SHADER_LIGHTS](BrokenLight(vec3(0.0), vec3(0.0), vec3(1.0), 0.5, vec3(1.0, 0.09, 0.032), vec2(12.5, 45.0), 2));

//Input Variables (Varying)
in vec3 v_Color;
in vec2 v_TexCoord; 

//Output Variables
out vec4 out_color; 

void main()
{ 	 
	if(Texture == 0)
		out_color = vec4(v_Color, 1.0);
	else
	{
		vec4 textureRes = texture(ourTexture, v_TexCoord) + texture(SpecText, v_TexCoord);
		out_color = textureRes + vec4(v_Color, 1.0)*texture(ourTexture, v_TexCoord);

		//out_color = texture(ourTexture, v_TexCoord) * vec4(v_Color, 1.0) + texture(SpecText, v_TexCoord);
	}
} 

#endif //FRAGMENT_SHADER
























































































