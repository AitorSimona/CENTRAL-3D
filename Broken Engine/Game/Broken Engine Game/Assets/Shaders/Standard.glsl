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
uniform vec3 u_CameraPosition;

//Varyings
out vec2 v_TexCoord;
out vec3 v_Color;
out vec3 v_Normal;
out vec3 v_FragPos;
out vec3 v_CamPos;

void main()
{ 
	gl_Position = projection * view * model_matrix * vec4(position.xyz, 1.0);
	
	v_TexCoord = texCoord; 
	v_Color = Color;
	v_FragPos = vec3(model_matrix * vec4(position.xyz, 1.0));
	v_Normal = mat3(transpose(inverse(model_matrix))) * normal;
	v_CamPos = u_CameraPosition;
}

#endif //VERTEX_SHADER


#define FRAGMENT_SHADER 
#ifdef FRAGMENT_SHADER 

#define MAX_SHADER_LIGHTS 2

//Output Variables
out vec4 out_color;

//Input Variables (Varying)
in vec2 v_TexCoord;
in vec3 v_Color;
in vec3 v_Normal;
in vec3 v_FragPos;
in vec3 v_CamPos;

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
uniform int u_LightsNumber = 0;


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
	if(Texture == 1)
	{
		diffuse *= texture(ourTexture, v_TexCoord).rgb;
		specular *= texture(SpecText, v_TexCoord).rgb;
	}	

	return (diffuse + specular);
}

//Dir Light Calculation
vec3 CalculateDirectionalLight(BrokenLight light, vec3 normal, vec3 viewDir)
{
	return CalculateLightResult(light.color, light.dir, normal, viewDir) * light.intensity;
}

//Point Light Calculation
vec3 CalculatePointlight(BrokenLight light, vec3 normal, vec3 viewDir)
{
	//Calculate light direction
	vec3 direction = light.pos - v_FragPos;

	//Attenuation Calculation
	float d = length(light.pos - v_FragPos);
	float lightAttenuation = 1.0/ (light.attenuationKLQ.x + light.attenuationKLQ.y * d + light.attenuationKLQ.z *(d * d));

	//Result
	return CalculateLightResult(light.color, direction, normal, viewDir) * lightAttenuation * light.intensity;
}

//Spot Light Calculation
vec3 CalculateSpotlight(BrokenLight light, vec3 normal, vec3 viewDir)
{
	//Calculate light direction
	vec3 direction = light.pos - v_FragPos;

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
	int lights_iterator = 0;
	if(u_LightsNumber > MAX_SHADER_LIGHTS)
		lights_iterator = MAX_SHADER_LIGHTS;
	else if(u_LightsNumber > 0)
		lights_iterator = u_LightsNumber;

	//Light Calculations
	vec3 normalVec = normalize(v_Normal);
	vec3 viewDirection = normalize(v_CamPos - v_FragPos);

	vec3 colorResult = vec3(0.0);

	for(int i = 0; i < lights_iterator; ++i)
	{
		if(u_BkLights[i].LightType == 0) //Directional
			colorResult += CalculateDirectionalLight(u_BkLights[i], normalVec, viewDirection);

		else if(u_BkLights[i].LightType == 1) //Pointlight
			colorResult += CalculatePointlight(u_BkLights[i], normalVec, viewDirection);

		else if(u_BkLights[i].LightType == 2) //Spotlight
			colorResult += CalculateSpotlight(u_BkLights[i], normalVec, viewDirection);
	}
	
	//Resulting Color
	if(Texture == 0)
		out_color = vec4(colorResult + v_Color, 1.0);
	else
		out_color = vec4(colorResult + v_Color * texture(ourTexture, v_TexCoord).rgb, 1.0);
} 

#endif //FRAGMENT_SHADER











































































































































































