#include "ModuleTextures.h"
#include "Application.h"
#include "OpenGL.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"


#include "DevIL/include/il.h"
#include "DevIL/include/ilu.h"
#include "DevIL/include/ilut.h"

#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")

#include "mmgr/mmgr.h"


using namespace Broken;
ModuleTextures::ModuleTextures(bool start_enabled) : Module(start_enabled) {
	name = "Textures";
}

ModuleTextures::~ModuleTextures() {}

bool ModuleTextures::Init(json& file) {
	bool ret = true;

	// Check versions
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
		iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION ||
		ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION) {
		ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: DevIL version is different. Exiting...");
		ret = false;
	}

	ENGINE_AND_SYSTEM_CONSOLE_LOG("Initializing DevIL Version: %i", IL_VERSION);
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

bool ModuleTextures::Start() {
	// --- Load Checkers Texture ---
	CheckerTexID = LoadCheckImage();
	DefaultTexture = LoadDefaultTexture();

	return true;
}

bool ModuleTextures::CleanUp() {

	return true;
}

uint ModuleTextures::LoadCheckImage() const {
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

uint ModuleTextures::LoadDefaultTexture() const {
	// --- Creating pixel data ---

	GLubyte default_tex[1][1][4];

	int c = 255;
	default_tex[0][0][0] = (GLubyte)c;
	default_tex[0][0][1] = (GLubyte)c;
	default_tex[0][0][2] = (GLubyte)c;
	default_tex[0][0][3] = (GLubyte)c;

	// --- Create the texture ---
	return CreateTextureFromPixels(GL_RGBA, 1, 1, GL_RGBA, default_tex, true);
}

uint ModuleTextures::CreateAndSaveTextureFromPixels(uint UID, int internalFormat, uint width, uint height, uint format, const void* pixels, std::string& out_path)
{
	out_path = TEXTURES_FOLDER;
	out_path.append(std::to_string(UID));
	out_path.append(".dds");

	ILuint img;
	ilGenImages(1, &img);
	ilBindImage(img);
	ilTexImage(width, height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, (void*)pixels);

	// --- Save to Lib ---
	ILuint size;
	ILubyte* data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer

	if (size > 0)
	{
		data = new ILubyte[size]; // allocate data buffer

		if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
			App->fs->Save(out_path.c_str(), data, size);

		delete[] data;
	}

	uint texID = ilutGLBindTexImage();

	ilDeleteImages(1, &img);

	return 	texID;
}

void ModuleTextures::SetTextureParameters(bool CheckersTexture) const {
	// --- Set texture clamping method ---
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// --- Set texture interpolation method ---
	if (CheckersTexture) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else {
		// --- Mipmap for the highest visual quality when resizing ---
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


		// --- Enabling anisotropic filtering for highest quality at oblique angles---
#ifdef  GL_EXT_texture_filter_anisotropic
		GLfloat largest_supported_anisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);
#endif
	}
}

uint ModuleTextures::CreateTextureFromPixels(int internalFormat, uint width, uint height, uint format, const void* pixels, bool CheckersTexture) const {
	uint TextureID = 0;

	// --- Generate the texture ID ---
	glGenTextures(1, (GLuint*)&TextureID);
	// --- Bind the texture so we can work with it---
	glBindTexture(GL_TEXTURE_2D, TextureID);

	SetTextureParameters(CheckersTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);

	if (!CheckersTexture) {
		// --- Generate Mipmap of the recently created texture (Note that we are using it in texture size reduction only)---
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// --- Unbind texture ---
	glBindTexture(GL_TEXTURE_2D, 0);

	ENGINE_CONSOLE_LOG("Loaded Texture: ID: %i , Width: %i , Height: %i ", TextureID, width, height);

	// --- Returning id so a mesh can use it (and destroy buffer when done) ---

	return TextureID;
}

void ModuleTextures::CreateTextureFromImage(uint& TextureID, uint& width, uint& height, std::string& path) const {
	// --- Attention!! If the image is flipped, we flip it back --- 
	ILinfo imageInfo;
	iluGetImageInfo(&imageInfo);

	width = imageInfo.Width;
	height = imageInfo.Height;

	if (imageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		iluFlipImage();

	// --- Convert the image into a suitable format to work with ---
	if (ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
	{
		// --- Create the texture ---
		uint originalFormat = ilGetInteger(IL_IMAGE_FORMAT);
		TextureID = CreateTextureFromPixels(originalFormat, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), originalFormat, ilGetData());

		if (path != "") {
			iluFlipImage();

			// --- Save to Lib ---
			ILuint size;
			ILubyte* data;
			ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
			size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer

			if (size > 0) {
				data = new ILubyte[size]; // allocate data buffer

				if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
					App->fs->Save(path.c_str(), data, size);

				delete[] data;
			}

		}
	}
	else
		ENGINE_CONSOLE_LOG("|[error]: Image conversion failed. ERROR: %s", iluErrorString(ilGetError()));
}

uint ModuleTextures::CreateCubemap(std::vector<uint>& cubemap_textures)
{
	uint texID = 0;

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	for (uint i = 0; i < 6; i++)
	{
		if (cubemap_textures[i] == 0)
			continue;

		glBindTexture(GL_TEXTURE_2D, cubemap_textures[i]);
		int w, h;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
		GLubyte* pixels = new GLubyte[w * h * 3];

		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

		glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels
		);

		delete[] pixels;
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return texID;
}

uint ModuleTextures::CreateTextureFromFile(const char* path, uint& width, uint& height, int UID) const {
	// --- In this function we use devil to load an image using the path given, extract pixel data and then create texture using CreateTextureFromImage ---

	uint TextureID = 0;

	if (path == nullptr) {
		ENGINE_CONSOLE_LOG("|[error]: Error at loading texture from path. ERROR: Path %s was nullptr", path);
		return TextureID;
	}

	// --- Generate the image name (ID for buffer) ---
	uint ImageName = 0;
	ilGenImages(1, (ILuint*)&ImageName);

	// --- Bind the image ---
	ilBindImage(ImageName);

	std::string lib_path = "";

	// --- Build path to library if asked ---
	if (UID >= 0) {
		lib_path = TEXTURES_FOLDER;
		lib_path.append(std::to_string(UID));
		lib_path.append(".dds");
	}

	// --- Load the image into binded buffer and create texture from its pixel data ---
	if (ilLoadImage(path))
		CreateTextureFromImage(TextureID, width, height, lib_path);
	else
		ENGINE_CONSOLE_LOG("|[error]: DevIL could not load the image. ERROR: %s", iluErrorString(ilGetError()));

	// --- Release Image data (we have already extracted the necessary information) ---
	ilDeleteImages(1, (const ILuint*)&ImageName);

	// --- Returning the Texture ID so a mesh can use it, note that this variable is filled by CreateTextureFromPixels ---
	return TextureID;
}

//void* ModuleTextures::GetTextureDataFromFile(const char* path) const {
//	if (path == nullptr) {
//		ENGINE_CONSOLE_LOG("|[error]: Error at loading texture from path. ERROR: Path %s was nullptr", path);
//		return nullptr;
//	}
//
//	uint ImageName = 0;
//	ilGenImages(1, (ILuint*)&ImageName);
//
//	// --- Bind the image ---
//	ilBindImage(ImageName);
//	void* ret = nullptr;
//
//	// --- Load the image into binded buffer and create texture from its pixel data ---
//	if (ilLoadImage(path)) {
//		ILinfo imageInfo;
//		iluGetImageInfo(&imageInfo);
//
//		ret = new char[imageInfo.SizeOfData];
//		memcpy(ret, ilGetData(), imageInfo.SizeOfData);
//	}
//	else {
//		ENGINE_CONSOLE_LOG("|[error]: DevIL could not load the image. ERROR: %s", iluErrorString(ilGetError()));
//	}
//
//	// --- Release Image data (we have already extracted the necessary information) ---
//	ilDeleteImages(1, &ImageName);
//
//	return ret;
//}
