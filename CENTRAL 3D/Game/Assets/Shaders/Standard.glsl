#version 440 core 
#define VERTEX_SHADER 
#ifdef VERTEX_SHADER 
layout (location = 0) in vec3 position; 
layout(location = 1) in vec3 normal; 
layout(location = 2) in vec3 color; 
layout (location = 3) in vec2 texCoord; 
uniform vec3 Color = vec3(1.0); 
out vec3 ourColor; 
out vec2 TexCoord; 
uniform mat4 model_matrix; 
uniform mat4 view; 
uniform mat4 projection; 
uniform int testint = 0;
uniform float testfloat = 0.0f;
uniform vec2 testvec2 = vec2(0,0);
uniform vec3 testvec3 = vec3(0,0,0);
uniform vec4 testvec4 = vec4(0,0,0,0);

void main(){ 
gl_Position = projection * view * model_matrix * vec4 (position, 1.0f); 
ourColor = Color * testint * testfloat; 

ourColor += vec3(testvec2, 0);
ourColor += testvec4.xyz;
ourColor += testvec3;

TexCoord = texCoord; 
}
#endif //VERTEX_SHADER

#define FRAGMENT_SHADER 
#ifdef FRAGMENT_SHADER 
uniform int Texture;
in vec3 ourColor; 
in vec2 TexCoord; 
out vec4 color; 
uniform sampler2D ourTexture; 
void main(){ 
color = texture(ourTexture, TexCoord); 
if(Texture == -1)
color = vec4(ourColor, 1);
} 
#endif //FRAGMENT_SHADER
