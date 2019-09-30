#include "JSONLoader.h"
#include <assert.h>
#include <fstream>
#include <iomanip>

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
	std::ifstream ifs;
	ifs.open(File);
	assert(ifs.is_open());

	// --- Parse File, put data in jsonfile ---
	try
	{
		jsonfile = json::parse(ifs);
	}
	catch (json::parse_error& e)
	{
		LOG("Parse Error in loading file: %c", e.what());
	}

	ifs.close();
	
	return jsonfile;
}

bool JSONLoader::Save(const char * File, json jsonfile) 
{
	// --- Save to File, overwrite if exists ---
	// Note setw, used to prettify JSON file (adding newlines and spaces)

	std::ofstream file;
	file.open(File);
	assert(file.is_open());
	file << std::setw(4) << jsonfile << std::endl;
	file.close();
	
	return true;
}
