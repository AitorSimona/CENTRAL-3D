#include "JSONLoader.h"
#include <fstream>
#include <iomanip>

#include "mmgr/mmgr.h"


json JSONLoader::Load(const char * File) const 
{

	bool ret = true;

	// --- Create JSON object ---
	json jsonfile;

	if (File == nullptr)
	{
		ret = false;
		CONSOLE_LOG("|[error]: JSONLoader::Load : %c was nullptr", File);
	}

	else
	{
		
		// --- Load File ---
		std::ifstream ifs;
		ifs.open(File);

		if (!ifs.is_open())
		{
			CONSOLE_LOG("|[error]: JSONLoader::Load could not open File: %c", File);
			ret = false;
		}

		else
		{
			// --- Parse File, put data in jsonfile ---
			try
			{
				jsonfile = json::parse(ifs);
			}
			catch (json::parse_error& e)
			{
				CONSOLE_LOG("|[error]: Parse Error in loading file: %c", e.what());
			}

			ifs.close();

		}

	}
	
	return jsonfile;
}

bool JSONLoader::Save(const char * File, json jsonfile) 
{
	// --- Save to File, overwrite if exists ---
	// Note setw, used to prettify JSON file (adding newlines and spaces)

	bool ret = true;

	std::ofstream file;
	file.open(File);

	if (!file.is_open())
	{
		CONSOLE_LOG("|[error]: JSONLoader::Save could not open File: %c", File);
		ret = false;
	}
	else
	{
		file << std::setw(4) << jsonfile << std::endl;
		file.close();
	}
	
	return ret;
}

std::string JSONLoader::Serialize(json jsonfile)
{
	std::string data;
	data = jsonfile.dump(4);

	return data;
}
