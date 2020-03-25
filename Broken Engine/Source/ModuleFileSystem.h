#ifndef __MODULEFILESYSTEM_H__
#define __MODULEFILESYSTEM_H__

#include "Module.h"
#include <vector>
#define NOMINMAX
#include <Windows.h>
#include "Timer.h"

struct SDL_RWops;
int close_sdl_rwops(SDL_RWops *rw);

struct aiFileIO;

BE_BEGIN_NAMESPACE


class BROKEN_API ModuleFileSystem : public Module {
public:

	ModuleFileSystem(bool start_enabled = true, const char* game_path = nullptr);

	// Destructor
	~ModuleFileSystem();

	// Called before render is available
	bool Init(json& file) override;

	update_status PreUpdate(float dt) override;

	// Called before quitting
	bool CleanUp() override;

	// Utility functions
	bool AddPath(const char* path_or_zip);
	bool Exists(const char* file) const;
	bool IsDirectory(const char* file) const;
	void CreateDirectory(const char* directory);
	std::string GetDirectoryFromPath(std::string& path);
	std::string GetNameFromPath(std::string path, bool withExtension = false);
	void DiscoverFiles(const char* directory, std::vector<std::string>& files) const;
	const std::vector<std::string>* ExDiscoverFiles(const char* directory);
	void DiscoverDirectories(const char* directory, std::vector<std::string>& dirs) const;
	const std::vector<std::string>* ExDiscoverDirectories(const char* directory);
	void DiscoverFilesAndDirectories(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) const;
	bool CopyFromOutsideFS(const char* full_path, const char* destination);
	bool Copy(const char* source, const char* destination);
	void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) const;
	void NormalizePath(char* full_path, bool lowercase = false) const;
	void NormalizePath(std::string& full_path, bool lowercase = false) const;
	uint GetLastModificationTime(const char* file);
	void WatchDirectory(const char* directory);
	void RemoveFileExtension(std::string& file);

	// Open for Read/Write
	unsigned int Load(const char* path, const char* file, char** buffer) const;
	unsigned int Load(const char* file, char** buffer) const;
	SDL_RWops* Load(const char* file) const;


	void DeleteArray(const std::vector<std::string>* to_delete);


	// IO interfaces for other libs to handle files via PHYSfs
	aiFileIO* GetAssimpIO();

	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;
	//bool SaveUnique(std::string& output, const void* buffer, uint size, const char* path, const char* prefix, const char* extension);
	bool Remove(const char* file);

	const char* GetBasePath() const;
	const char* GetWritePath() const;
	const char* GetReadPaths() const;

private:
	// --- FS Windows Watcher ---
	ulong dwWaitStatus;
	HANDLE dwChangeHandles[1]; // void*

	bool started_wait = false;
	Timer wait_timer;
	uint32 wait_time = 1000; // ms

	// --- Vector of returned char** ---
	std::vector<std::vector<std::string>*> returned_arrays;


	void CreateAssimpIO();

private:

	aiFileIO* AssimpIO = nullptr;
};

BE_END_NAMESPACE
#endif // __MODULEFILESYSTEM_H__