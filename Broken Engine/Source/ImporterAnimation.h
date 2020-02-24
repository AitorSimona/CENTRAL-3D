#ifndef __IMPORTER_ANIMATION_H__
#define __IMPORTER_ANIMATION_H__

#include "Importer.h"

struct aiAnimation;
class ResourceAnimation;
class Resource;

struct ImportAnimationData : public Importer::ImportData
{
	ImportAnimationData(const char* path) : Importer::ImportData(path) {};

	aiAnimation* animation = nullptr;
};

class ImporterAnimation : public Importer
{

public:
	ImporterAnimation();
	virtual ~ImporterAnimation();

	Resource* Import(ImportData& IData) const override;

	void Save(ResourceAnimation* anim) const;
	Resource* Load(const char* path) const override;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Animation; };
};

#endif