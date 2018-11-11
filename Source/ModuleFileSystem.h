#ifndef __MODULEFILESYSTEM_H__
#define __MODULEFILESYSTEM_H__

#include "Module.h"
#include "Panel.h"
#include "Timer.h"

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(bool start_enabled = true);
	~ModuleFileSystem();

	update_status Update(float dt);

	bool addPath(const char* path);
	bool fileExists(const char* path, const char* atDirectory = nullptr, const char* withExtension = nullptr);

	uint readFile(const char* path, char** buffer);
	uint writeFile(const char* path, const void* buffer, uint size, bool overwrite = false);

	bool copyFile(const char* src, const char* dest);
	bool deleteFile(const char* path);
	
	//Check if the file exists and returns filename+(num_version)
	//num_version is equivalent to the amounts of times that a file with given name exists -1
	const char* getAvailablePath(const char* path);

	void splitPath(const char* full_path, std::string* path, std::string* filename, std::string* extension);
	std::string normalizePath(const char* path);
	std::string getFullPath(const char* path, const char* atDirectory = nullptr, const char* withExtension = nullptr);

	void manageDroppedFiles(const char* path);

	void importFilesAt(const char* path);
	
	void getFilesAt(const char* path, std::list<assetsElement*>& elements, const char* exclusiveExtension = nullptr, const char* ignoreExtension = nullptr);

	int getLastTimeChanged(const char* path);
	//Path: path of the asset file (not the .meta)
	int getMetaLastChange(const char* path);

private:

	Timer import_timer;
	float import_delay = 1000.0f;

	bool first_import = false;
};

#endif // !__MODULEFILESYSTEM_H__