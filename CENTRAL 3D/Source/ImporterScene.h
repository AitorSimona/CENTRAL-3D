#ifndef __IMPORTER_SCENE_H__
#define __IMPORTER_SCENE_H__

#include "Importer.h"

struct ImportSceneData : public ImportData
{

};

class ImporterScene : public Importer
{

public:
	ImporterScene();
	virtual ~ImporterScene();

	bool Import(const char& File_path, const ImportData& IData) const override;
};

#endif
