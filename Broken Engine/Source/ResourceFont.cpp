#include "ResourceFont.h"
#include "Application.h"
#include "ModuleGui.h"

#include "mmgr/mmgr.h"

#include "Freetype/include/ft2build.h"

#include FT_FREETYPE_H

using namespace Broken;

// un cop tens una resource importada al seu map(he creat una nova), com accedeixes a ella ? veig que hi ha el GetResource() pero necessites el ID, mhaig de guardar el ID amb el importer ?
// 


ResourceFont::ResourceFont(uint UID, const char* source_file) : Resource(Resource::ResourceType::FONT, UID, source_file)
{
	//extension = ".ttf";
	//resource_file = FONTS_FOLDER + std::to_string(UID) + extension;

	previewTexID = App->gui->defaultfileTexID;
}

ResourceFont::~ResourceFont()
{
}

bool ResourceFont::LoadInMemory()
{
	return false;
}

void ResourceFont::FreeMemory()
{
	// delete vbo vao texture
	
}

void ResourceFont::Init()
{
	FT_Library ft;
	this;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;

	if (FT_New_Face(ft, GetOriginalFile(), 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, size);
	//FT_Set_Char_Size(face, size << 6, size << 6, 96, 96);
	//FT_Activate_Size();
	// Init of all chars of the font to the map of character textures
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	characters.clear();
	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use


		ResourceFont::Character character = {
			texture,
			float2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			float2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		characters.insert(std::pair<GLchar, ResourceFont::Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 5, NULL, GL_DYNAMIC_DRAW);
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	// texCoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ResourceFont::OnOverwrite()
{
}

void ResourceFont::OnDelete()
{
}

