#ifndef __JSONLoader_H__
#define __JSONLoader_H__

#include "BrokenCore.h"
#include "json/json.hpp"

namespace BrokenEngine {
	// for convenience
	using json = nlohmann::json;

	class BROKEN_API JSONLoader {
	public:

		JSONLoader() {}
		~JSONLoader() {}

		json Load(const char* File) const;

		bool Save(const char* File, json jsonfile);

		std::string Serialize(json jsonfile);
	};
}
#endif
