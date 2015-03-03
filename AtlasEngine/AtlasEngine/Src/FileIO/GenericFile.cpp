#include "FileIO\GenericFile.h"

uint32_t IFileSystem::ProgramDirectorySize = 0;
wchar_t IFileSystem::ProgramDirectory[PROGRAM_DIRECTORY_CAP];