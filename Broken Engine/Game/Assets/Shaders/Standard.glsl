#version 440 core
		#define VERTEX_SHADER
		#ifdef VERTEX_SHADER

		layout (location = 0) in vec3 a_Position;
		layout(location = 1) in vec3 a_Normal;
		layout(location = 2) in vec3 a_Color;
		layout (location = 3) in vec2 a_TexCoord;

		uniform vec3 u_Color = vec3(1.0);
		uniform mat4 u_Model;
		uniform mat4 u_View;
		uniform mat4 u_Proj;

#define FRAGMENT_SHADER
#ifdef FRAGMENT_SHADER

#define MAX_SHADER_LIGHTS 20

//Output Variables
out vec4 out_color;

//Input Variables (Varying)
in vec2 v_TexCoord;
in vec3 v_Color;
in vec3 v_Normal;
in vec3 v_FragPos;
in vec3 v_CamPos;
in mat3 v_TBN;

//Uniforms
uniform float u_Shininess = 1.5;
uniform int u_UseTextures = 0;

uniform int u_HasDiffuseTexture = 0;
uniform int u_HasSpecularTexture = 0;
uniform int u_HasNormalMap = 0;

uniform int u_DrawNormalMapping = 0;
uniform int u_DrawNormalMapping_Lit = 0;
uniform int u_DrawNormalMapping_Lit_Adv = 0;

uniform sampler2D u_AlbedoTexture;
uniform sampler2D u_SpecularTexture;
uniform sampler2D u_NormalTexture;

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

uniform int u_LightsNumber = 0;
uniform BrokenLight u_BkLights[MAX_SHADER_LIGHTS];
// uniform BrokenLight u_BkLights[MAX_SHADER_LIGHTS] = BrokenLight[MAX_SHADER_LIGHTS](BrokenLight(vec3(0.0), vec3(0.0), vec3(1.0), 0.5, vec3(1.0, 0.09, 0.032), vec2(12.5, 45.0), 2));


		uniform int u_UseTextures;
		uniform sampler2D u_AlbedoTexture;

		in vec3 v_Color;
		in vec2 v_TexCoord;
		out vec4 color;

		void main()
		{
			color = texture(u_AlbedoTexture, v_TexCoord) * vec4(v_Color, 1);
			if(u_UseTextures == -1)
				color = vec4(v_Color, 1);
		}
		#endif //FRAGMENT_SHADER
