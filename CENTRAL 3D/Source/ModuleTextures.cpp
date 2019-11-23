#include "ModuleTextures.h"
#include "Application.h"
#include "ModuleImporter.h"
#include "OpenGL.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ResourceTexture.h"

#include "DevIL/include/il.h"
#include "DevIL/include/ilu.h"
#include "DevIL/include/ilut.h"

#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")

#include "mmgr/mmgr.h"


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
		CONSOLE_LOG("|[error]: DevIL version is different. Exiting...");
		ret = false;
	}

	CONSOLE_LOG("Initializing DevIL Version: %i",IL_VERSION);
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

inline void ModuleTextures::SetTextureParameters(bool CheckersTexture) const
{
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
}

uint ModuleTextures::CreateTextureFromPixels(int internalFormat, uint width, uint height, uint format, const void* pixels, bool CheckersTexture) const
{
	uint TextureID = 0;

	// --- Generate the texture ID ---
	glGenTextures(1, (GLuint*)&TextureID);
	// --- Bind the texture so we can work with it---
	glBindTexture(GL_TEXTURE_2D, TextureID);

	SetTextureParameters(CheckersTexture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);

	if (!CheckersTexture)
	{
		// --- Generate Mipmap of the recently created texture (Note that we are using it in texture size reduction only)---
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// --- Unbind texture ---
	glBindTexture(GL_TEXTURE_2D, 0);

	CONSOLE_LOG("Loaded Texture: ID: %i , Width: %i , Height: %i ", TextureID, width, height);

	// --- Returning id so a mesh can use it (and destroy buffer when done) ---

	return TextureID;
}

inline void ModuleTextures::CreateTextureFromImage(uint &TextureID, uint &width, uint &height, const char* path, bool load_existing) const
{
	// --- Attention!! If the image is flipped, we flip it back --- 
	ILinfo imageInfo;
	iluGetImageInfo(&imageInfo);

	width = imageInfo.Width;
	height = imageInfo.Height;

	//if (imageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	//	iluFlipImage();

	// --- Convert the image into a suitable format to work with ---
	if (ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
	{
		// --- Create the texture ---
		TextureID = CreateTextureFromPixels(ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_FORMAT), ilGetData());

		if (!load_existing)
		{
			// --- Save to Lib ---
			ILuint size;
			ILubyte *data;
			ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
			size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer

			if (size > 0) {
				data = new ILubyte[size]; // allocate data buffer

				if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
					App->fs->Save(path, data, size);

				delete[] data;
			}

		}
	}
	else
		CONSOLE_LOG("|[error]: Image conversion failed. ERROR: %s", iluErrorString(ilGetError()));
}

uint ModuleTextures::CreateTextureFromFile(const char* path, uint &width, uint &height, uint LibUID) const
{
	// --- In this function we use devil to load an image using the path given, extract pixel data and then create texture using CreateTextureFromImage ---

	uint TextureID = 0;

	if (path == nullptr)
	{
		CONSOLE_LOG("|[error]: Error at loading texture from path. ERROR: Path %s was nullptr",path);
		return TextureID;
	}

	std::string final_path = path;

	// --- Generate the image name (ID for buffer) ---
	uint ImageName = 0;
	ilGenImages(1, (ILuint*)&ImageName);

	// --- Bind the image ---
	ilBindImage(ImageName);

	bool load_existing = false;
	std::string name = TEXTURES_FOLDER;
	name.append(std::to_string(LibUID));
	name.append(".dds");

	// --- Look for meta, if found load image from library ---
	if (App->resources->IsFileImported(path))
	{
		uint uid = App->resources->GetUIDFromMeta(path);

		std::string lib_Tex = TEXTURES_FOLDER;
		lib_Tex.append(std::to_string(uid));
		lib_Tex.append(".dds");
		final_path = lib_Tex;

		load_existing = true;
	}
	else if (final_path.find("Library") != std::string::npos)
		load_existing = true;
	else
	{
		App->resources->CreateMetaFromUID(LibUID, path);
	}

	// --- Load the image into binded buffer and create texture from its pixel data ---
	if (ilLoadImage(final_path.data()))
		CreateTextureFromImage(TextureID, width,height, name.data(), load_existing);
	else
		CONSOLE_LOG("|[error]: DevIL could not load the image. ERROR: %s", iluErrorString(ilGetError()));

	// --- Release Image data (we have already extracted the necessary information) ---
	ilDeleteImages(1, (const ILuint*)&ImageName);

	// --- Returning the Texture ID so a mesh can use it, note that this variable is filled by CreateTextureFromPixels ---

	return TextureID;
}
