#include "JSONLoader.h"
#include <assert.h>
#include <fstream>

// for convenience
using json = nlohmann::json;

bool JSONLoader::Load(const char * importFile) 
{
	/*if (importFile == nullptr)
	{
		assert(importFile != nullptr);
		return false;
	}*/

	if (!done)
	{
		done = true;
	/*	json test = {
		  {"pi", 3.141},
		  {"happy", true},
		  {"name", "Niels"},
		  {"nothing", nullptr},
		  {"answer", {
			{"everything", 42}
		  }},
		  {"list", {1, 0, 2}},
		  {"object", {
			{"currency", "USD"},
			{"value", 42.99}
		  }}
		};*/

		std::ifstream ifs("TEST.json");

		json j = json::parse(ifs);

		std::string str = j["HEY"];
	}


	return true;
}

bool JSONLoader::Save(const char * File) 
{
	json jsonfile;

	std::string test = "Hello World";
	jsonfile["HEY"] = test;

	std::ofstream file("TEST.json");
	file << jsonfile;

	return false;
}
