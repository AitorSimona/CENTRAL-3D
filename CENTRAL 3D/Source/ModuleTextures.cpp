#include "ModuleTextures.h"
#include "Application.h"
#include "ModuleMeshImporter.h"
#include "OpenGL.h"

#include "DevIL/include/il.h"
#include "DevIL/include/ilu.h"
#include "DevIL/include/ilut.h"

#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")

#include "mmgr/mmgr.h"

#define CHECKERS_HEIGHT 64
#define CHECKERS_WIDTH 64

ModuleTextures::ModuleTextures(bool start_enabled) : Module(start_enabled)
{
	name = "Textures";
}

ModuleTextures::~ModuleTextures() {}

bool ModuleTextures::Init(json file)
{
	bool ret = true;

	// Check versions
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
		iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION ||
		ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION)
	{
		LOG("|[error]: DevIL version is different. Exiting...");
		ret = false;
	}

	LOG("Initializing DevIL");
	// --- Initializing DevIL

	// Initialize IL
	ilInit();

	// Initialize ILU
	iluInit();

	// Initialize ILUT with OpenGL support.
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return ret;
}

bool ModuleTextures::Start()
{
	// --- Load Checkers Texture ---
	CheckerTexID = LoadCheckImage();

	return true;
}

bool ModuleTextures::CleanUp()
{
	// --- Delete sample checkers texture ---
	if (CheckerTexID > 0)
		glDeleteTextures(1, (GLuint*)&CheckerTexID);

	return true;
}

uint ModuleTextures::LoadCheckImage() const
{
	// --- Creating pixel data for checkers texture ---

	GLubyte checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	// --- Create the texture ---
	return CreateTextureFromPixels(GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, GL_RGBA, checkImage, true);
}

uint ModuleTextures::GetCheckerTextureID() const
{
	return CheckerTexID;
}

uint ModuleTextures::CreateTextureFromPixels(int internalFormat, uint width, uint height, uint format, const void* pixels, bool CheckersTexture) const
{
	uint TextureID = 0;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// --- Generate the texture ID ---
	glGenTextures(1, (GLuint*)&TextureID);
	// --- Bind the texture so we can work with it---
	glBindTexture(GL_TEXTURE_2D, TextureID);
	// --- Set texture clamping method ---
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// --- Set texture interpolation method ---
	if (CheckersTexture)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else
	{
		// --- Mipmap for the highest visual quality when resizing ---
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		// --- Enabling anisotropic filtering for highest quality at oblique angles---
		if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
		{
			GLfloat largest_supported_anisotropy;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);

	if (!CheckersTexture)
	{
		// --- Generate Mipmap and enable 2D Textures ---
		glEnable(GL_TEXTURE_2D);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// --- Unbind texture ---
	glBindTexture(GL_TEXTURE_2D, 0);

	LOG("Loaded Texture: ID: %i , Width: %i , Height: %i ", TextureID, width, height);

	// --- Returning id so a mesh can use it (and destroy buffer when done) ---

	return TextureID;
}

uint ModuleTextures::CreateTextureFromFile(const char* path) const
{
	// --- In this function I use devil to load an image using the path given, extract pixel data and then create texture using CreateTextureFromPixels ---

	uint texName = 0;

	if (path == nullptr)
	{
		LOG("|[error]: Error at loading texture from path. ERROR: Path %s was nullptr",path);
		return texName;
	}

	// --- Generate the image name ---
	uint ImageName = 0;
	ilGenImages(1, (ILuint*)&ImageName);

	// --- Bind the image ---
	ilBindImage(ImageName);

	// --- Load the image ---
	if (ilLoadImage(path))
	{
		// --- Attention!! If the image is flipped, we flip it back --- 
		ILinfo imageInfo;
		iluGetImageInfo(&imageInfo);

		if (imageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
			iluFlipImage();

		// --- Convert the image into a suitable format to work with ---
		if (ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
		{
			// --- Create the texture ---
			texName = CreateTextureFromPixels(ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_FORMAT), ilGetData());		
		}
		else
			LOG("|[error]: Image conversion failed. ERROR: %s", iluErrorString(ilGetError()));
	}
	else
		LOG("|[error]: DevIL could not load the image. ERROR: %s", iluErrorString(ilGetError()));

	ilDeleteImages(1, (const ILuint*)&ImageName);

	// --- Returning the Texture ID so a mesh can use it, note that this variable is filled by CreateTextureFromPixels ---

	return texName;
}
