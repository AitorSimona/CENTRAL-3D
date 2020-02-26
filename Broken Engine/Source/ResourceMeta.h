#ifndef __RESOURCE_META_H__
#define __RESOURCE_META_H__

#include "Resource.h"

namespace BrokenEngine {
	class ResourceMeta : public Resource {
	public:
		ResourceMeta(uint UID, std::string source_file);
		~ResourceMeta();

		bool LoadInMemory() override;
		void FreeMemory() override;

		uint Date = 0;

	private:
		void OnOverwrite() override;
		void OnDelete() override;
		void Repath() override;
	};
}
#endif //__RESOURCE_META_H__