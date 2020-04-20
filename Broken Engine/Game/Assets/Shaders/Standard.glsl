#version 440 core

#define VERTEX_SHADER
#ifdef VERTEX_SHADER

//Layout Daya
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Color;
layout (location = 3) in vec2 a_TexCoord;
layout (location = 4) in vec3 a_Tangent;
layout (location = 5) in vec3 a_Bitangent;

//Uniforms
uniform mat4 u_Model; //model_matrix
uniform mat4 u_View; //view
uniform mat4 u_Proj; //projection

uniform vec4 u_Color = vec4(1.0); //Color
uniform vec3 u_CameraPosition;

//Varyings
out vec2 v_TexCoord;
out vec4 v_Color;
out vec3 v_Normal;
out vec3 v_FragPos;
out vec3 v_CamPos;
out mat3 v_TBN;

void main()
{
	v_Color = u_Color;
	v_TexCoord = a_TexCoord;
	v_CamPos = u_CameraPosition;

	v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
	v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;

	vec3 T = normalize(vec3(u_Model * vec4(a_Tangent, 0.0)));
	vec3 N = normalize(vec3(u_Model * vec4(a_Normal, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	v_TBN = (mat3(T, B, N));

	gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0);
}

#endif //VERTEX_SHADER

#define FRAGMENT_SHADER
#ifdef FRAGMENT_SHADER

#define MAX_SHADER_LIGHTS 20

//Output Variables
out vec4 out_color;

//Input Variables (Varying)
in vec2 v_TexCoord;
in vec4 v_Color;
in vec3 v_Normal;
in vec3 v_FragPos;
in vec3 v_CamPos;
in mat3 v_TBN;

//Uniforms
uniform float u_GammaCorrection = 1.0;

uniform float u_Shininess = 1.5;
uniform int u_UseTextures = 0;

uniform int u_HasDiffuseTexture = 0;
uniform int u_HasSpecularTexture = 0;
uniform int u_HasNormalMap = 0;
uniform int HasTransparencies = 0;

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


//Light Calculations Functions ---------------------------------------------------------------------------------------
vec3 CalculateLightResult(vec3 LColor, vec3 LDir, vec3 normal, vec3 viewDir)
{
	//Normalize light direction
	vec3 lightDir = normalize(LDir);

	//Diffuse Component
	float diffImpact = max(dot(normal, lightDir), 0.0);

	//Specular component
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float specImpact = pow(max(dot(normal, halfwayDir), 0.0), u_Shininess);

	//Calculate light result
	vec3 diffuse = LColor * diffImpact;
	vec3 specular = LColor * specImpact;

	//If we have textures, apply them
	if(u_HasDiffuseTexture == 1)
		diffuse *= texture(u_AlbedoTexture, v_TexCoord).rgb;
	if(u_HasSpecularTexture == 1)
		specular *= texture(u_SpecularTexture, v_TexCoord).rgb;

	return (diffuse + specular);
}

//Dir Light Calculation
vec3 CalculateDirectionalLight(BrokenLight light, vec3 normal, vec3 viewDir)
{
	if(u_HasNormalMap == 1)
		return CalculateLightResult(light.color, /*v_TBN * */normalize(light.dir), normal, viewDir) * light.intensity;
	else
		return CalculateLightResult(light.color, light.dir, normal, viewDir) * light.intensity;
}

//Point Light Calculation
vec3 CalculatePointlight(BrokenLight light, vec3 normal, vec3 viewDir)
{
	//Calculate light direction
	vec3 direction = light.pos - v_FragPos;
	//if(u_HasNormalMap == 1)
	//	direction = v_TBN * normalize(direction);

	//Attenuation Calculation
	float d = length(light.pos - v_FragPos);
	float lightAttenuation = 1.0/(light.attenuationKLQ.x + light.attenuationKLQ.y * d + light.attenuationKLQ.z *(d * d));

	//Result
	return CalculateLightResult(light.color, direction, normal, viewDir) * lightAttenuation * light.intensity;
}

//Spot Light Calculation
vec3 CalculateSpotlight(BrokenLight light, vec3 normal, vec3 viewDir)
{
	//Calculate light direction
	vec3 direction = light.pos - v_FragPos;
	//if(u_HasNormalMap == 1)
	//	direction = v_TBN * normalize(direction);

	//Attenuation Calculation
	float d = length(light.pos - v_FragPos);
	float lightAttenuation = 1.0/ (light.attenuationKLQ.x + light.attenuationKLQ.y * d + light.attenuationKLQ.z *(d * d));

	//Spotlight Calcs for Soft Edges
	float theta = dot(normalize(light.pos - v_FragPos), normalize(-light.dir)); //Light direction and light orientation
	float epsilon = light.InOutCutoff.x - light.InOutCutoff.y;

	float lightIntensity = clamp((theta - light.InOutCutoff.y) / epsilon, 0.0, 1.0) * light.intensity;

	//Result
	return CalculateLightResult(light.color, direction, normal, viewDir) * lightAttenuation * lightIntensity;
}

//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void main()
{
	// Transparency
	float alpha = 1.0;
	if(HasTransparencies == 1)
	{
		if(u_UseTextures == 0)
			alpha = v_Color.a;
		else
			alpha = texture(u_AlbedoTexture, v_TexCoord).a * v_Color.a;
	}

	if(alpha < 0.004)
		discard;

	//Normal Mapping Calculations
	vec3 normalVec = normalize(v_Normal);	
	if(u_DrawNormalMapping == 1)
	{
		out_color = vec4(normalVec, 1.0);
		return;
	}
	
	vec3 viewDirection = normalize(v_CamPos - v_FragPos);
	if(u_HasNormalMap == 1)
	{
		normalVec = texture(u_NormalTexture, v_TexCoord).rgb;
		normalVec = normalize(normalVec * 2.0 - 1.0);
		normalVec = normalize(v_TBN * normalVec);
		//viewDirection = v_TBN * normalize(v_CamPos - v_FragPos);
	}

	//Light Calculations
	int lights_iterator = (u_LightsNumber > MAX_SHADER_LIGHTS ? MAX_SHADER_LIGHTS : u_LightsNumber);
	vec3 colorResult = vec3(0.0);
	for(int i = 0; i < lights_iterator; ++i)
	{
		if(u_DrawNormalMapping_Lit_Adv == 0)
		{
			if(u_BkLights[i].LightType == 0) //Directional
				colorResult += CalculateDirectionalLight(u_BkLights[i], normalVec, viewDirection);

			else if(u_BkLights[i].LightType == 1) //Pointlight
				colorResult += CalculatePointlight(u_BkLights[i], normalVec, viewDirection);

			else if(u_BkLights[i].LightType == 2) //Spotlight
				colorResult += CalculateSpotlight(u_BkLights[i], normalVec, viewDirection);
		}
		else
		{
			if(u_BkLights[i].LightType == 0)
				colorResult += v_TBN * normalize(u_BkLights[i].dir);
			else
				colorResult += v_TBN * normalize(u_BkLights[i].pos);
		}
	}

	if(u_DrawNormalMapping_Lit == 0 && u_DrawNormalMapping_Lit_Adv == 0)
	{
		//Resulting Color
		if(u_UseTextures == 0 || (HasTransparencies == 0 && u_UseTextures == 1 && texture(u_AlbedoTexture, v_TexCoord).a < 0.1))
			out_color = vec4(colorResult + v_Color.rgb, alpha);
		else if(u_UseTextures == 1)
			out_color = vec4(colorResult + v_Color.rgb * texture(u_AlbedoTexture, v_TexCoord).rgb, alpha);

		out_color = pow(out_color, vec4(vec3(1.0/u_GammaCorrection), 1.0));
	}
	else
	{
		//Normal Mapping Debgug
		if(u_DrawNormalMapping_Lit == 1)
			out_color = vec4(colorResult * normalVec, 1.0);
		else if(u_DrawNormalMapping_Lit_Adv == 1)
			out_color = vec4(colorResult, 1.0);
	}
}

#endif //FRAGMENT_SHADER
