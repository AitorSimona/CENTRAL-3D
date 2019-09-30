#include "JSONLoader.h"
#include <assert.h>
#include <fstream>

#include "Source/mmgr/mmgr.h"


json JSONLoader::Load(const char * File) const 
{
	if (File == nullptr)
	{
		assert(File != nullptr);
	}

	// --- Create JSON object ---
	json jsonfile;

	// --- Load File ---
	std::ifstream ifs(File);
	assert(!ifs.fail());

	// --- Parse File, put data in jsonfile ---
	try
	{
		jsonfile = json::parse(ifs);
	}
	catch (json::parse_error& e)
	{
		LOG("Parse Error in loading file: %c", e.what());
	}
	
	return jsonfile;
}

bool JSONLoader::Save(const char * File, json jsonfile) 
{
	// --- Save to File, create if it does not exist ---
	std::ofstream file(File);
	assert(file.fail());

	file << jsonfile;

	return true;
}
