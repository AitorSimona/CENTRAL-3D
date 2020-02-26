#ifndef __RESOURCE_SHADER_OBJECT_H__
#define __RESOURCE_SHADER_OBJECT_H__

#include "Resource.h"

namespace BrokenEngine {
	class ResourceShaderObject : public Resource {
	public:
		ResourceShaderObject(uint UID, std::string source_file);
		~ResourceShaderObject();

		bool LoadInMemory() override;
		void FreeMemory() override;

	private:
		void OnOverwrite() override;
		void OnDelete() override;
	};
}
#endif //__RESOURCE_SHADER_OBJECT_H__