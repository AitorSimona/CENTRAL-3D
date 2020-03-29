#version 440 core 
#define VERTEX_SHADER 
#ifdef VERTEX_SHADER 
layout (location = 0) in vec3 position; 
uniform vec2 nearfar; 
uniform mat4 model_matrix; 
uniform mat4 view; 
uniform mat4 projection; 
out mat4 _projection; 
out vec2 nearfarfrag; 
void main(){ 
nearfarfrag = nearfar; 
_projection = projection; 
gl_Position = projection * view * model_matrix * vec4(position, 1.0f); 
}
#endif //VERTEX_SHADER

#define FRAGMENT_SHADER 
#ifdef FRAGMENT_SHADER 
out vec4 FragColor; 
in vec2 nearfarfrag; 
in mat4 _projection; 
float LinearizeDepth(float depth){ 
float z =  2.0*depth - 1.0; // back to NDC 
return 2.0* nearfarfrag.x * nearfarfrag.y / (nearfarfrag.y + nearfarfrag.x - z * (nearfarfrag.y - nearfarfrag.x)); }
void main(){ 
float depth = LinearizeDepth(gl_FragCoord.z) / nearfarfrag.y;  
FragColor = vec4(vec3(gl_FragCoord.z*nearfarfrag.y*nearfarfrag.x), 1.0); } 
#endif //FRAGMENT_SHADER













































































































