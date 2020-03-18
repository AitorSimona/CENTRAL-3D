#include "ImporterFont.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ResourceFont.h"

#include "mmgr/mmgr.h"

#define FONT_SIZE_DEFAULT 32

using namespace Broken;

ImporterFont::ImporterFont() : Importer(Importer::ImporterType::Font)
{
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
}

ImporterFont::~ImporterFont()
{
	FT_Done_FreeType(ft);
}

Resource* ImporterFont::Import(ImportData& IData) const
{
	ResourceFont* font = (ResourceFont*)App->resources->CreateResource(Resource::ResourceType::FONT, IData.path);

	std::string folder = FONTS_FOLDER;

	FT_Face face;

	if (FT_New_Face(ft, IData.path, 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, FONT_SIZE_DEFAULT);

	// Init of all chars of the font to the map of character textures
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction


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
		glGenTextures(1, &font->texture);
		glBindTexture(GL_TEXTURE_2D, font->texture);
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
			font->texture,
			float2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			float2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		font->characters.insert(std::pair<GLchar, ResourceFont::Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);


	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &font->VAO);
	glGenBuffers(1, &font->VBO);
	glBindVertexArray(font->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, font->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 5, NULL, GL_DYNAMIC_DRAW);
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	// texCoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return font;
}

void ImporterFont::Save(ImporterFont* anim) const
{
}

Resource* ImporterFont::Load(const char* path) const
{

	return nullptr;
}
