#ifndef __RESOURCESCRIPT_H__
#define __RESOURCESCRIPT_H__

#include "Resource.h"

namespace BrokenEngine {
	class ResourceScript : public Resource {
	public:
		ResourceScript(uint uid, std::string source_file);
		~ResourceScript();

	public:
		bool LoadInMemory() override;
		void OnOverwrite() override;
		void OnDelete() override;
		void FreeMemory() override;
	public:
		std::string absolute_path;
		std::string relative_path;
		std::string script_name;

	};
}
#endif // !__RESOURCESCRIPT_H__

