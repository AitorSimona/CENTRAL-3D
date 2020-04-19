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

		out vec3 v_Color;
		out vec2 v_TexCoord;

		void main()
		{
			gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0f);
			v_Color = u_Color;
			v_TexCoord = a_TexCoord;
		}
		#endif //VERTEX_SHADER
#define FRAGMENT_SHADER
		#ifdef FRAGMENT_SHADER

		out vec4 FragColor;

		in vec2 v_TexCoord;

		uniform sampler2D u_AlbedoTexture;

		void main()
		{
			vec4 texColor = texture(u_AlbedoTexture, v_TexCoord);

			if(texColor.a < 0.1)
				discard;

		    FragColor = texColor;
		}
		#endif //FRAGMENT_SHADER