#ifndef __IMPORTER_ANIMATOR_H__
#define __IMPORTER_ANIMATOR_H__

#include "Importer.h"

#include <map>

BE_BEGIN_NAMESPACE
class ResourceAnimator;
class Resource;
class GameObject;

struct BROKEN_API ImportAnimatorData : public Importer::ImportData
{
	ImportAnimatorData(const char* path) : Importer::ImportData(path) {};
};

class BROKEN_API ImporterAnimator : public Importer
{

public:
	ImporterAnimator();
	virtual ~ImporterAnimator();

	Resource* Import(ImportData& IData) const override;

	void Save(ResourceAnimator* animator) const;
	Resource* Load(const char* path) const override;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Animator; };
	
};

BE_END_NAMESPACE
#endif // !__IMPORTER_ANIMATOR_H__