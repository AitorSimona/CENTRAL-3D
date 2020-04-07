#version 440 core
		#define VERTEX_SHADER
		#ifdef VERTEX_SHADER

		layout (location = 0) in vec2 aPos;
		layout (location = 1) in vec2 aTexCoords;

		out vec2 TexCoords;

		void main()
		{
			gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
			TexCoords = aTexCoords;
		} 
		#endif //VERTEX_SHADER
#define FRAGMENT_SHADER
		#ifdef FRAGMENT_SHADER

		out vec4 FragColor;
  
		in vec2 TexCoords;

		uniform sampler2D screenTexture;
		
		void main()
		{ 
		    FragColor = texture(screenTexture, TexCoords);
		}
		#endif //FRAGMENT_SHADER