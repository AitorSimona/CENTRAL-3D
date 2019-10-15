#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "Globals.h"

struct ImportData {};

class Importer
{
public:
	Importer();
	virtual ~Importer();

	virtual bool Import(const char& File_path, const ImportData& IData) const = 0;

};

#endif
