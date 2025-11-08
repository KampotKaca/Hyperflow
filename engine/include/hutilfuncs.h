//
// Created by ratib on 08.11.2025.
//

#ifndef HYPERFLOW_HUTILFUNCS_H
#define HYPERFLOW_HUTILFUNCS_H

#include "henums.h"

namespace hf::utils
{
    ivec3 ConvertVersion(const char* version); //Convert string version to an integer,
    bool ReadFile(const char* path, List<char>& result); //Get the data of the file.
    bool ReadFile(const std::string& path, List<char>& result); //Get the data of the file.
    bool ReadFile(const std::filesystem::path& path, List<char>& result); //Get the data of the file.
    bool ReadFile(const std::string_view path, List<char>& result); //Get the data of the file.
    bool WriteFile(const char* path, const List<char>& data); //Write the data in the file.
    bool WriteFile(const std::string& path, const List<char>& data); //Write the data in the file.
    bool WriteFile(const std::filesystem::path& path, const List<char>& data); //Write the data in the file.
    bool WriteFile(const std::string_view path, const List<char>& data); //Write the data in the file.
    bool FileExists(const char* path); //Check if file exists.
    bool FileExists(const std::filesystem::path& path); //Check if file exists.

    [[nodiscard]] void* Alloc(std::size_t n); //Allocate n amount of memory.
    [[nodiscard]] void* AllocAligned(std::size_t n, std::size_t align); //Allocate n amount of aligned memory.
    void Deallocate(void* p); //Deallocate the memory.
    void DeallocateAligned(void* p, std::size_t align); //Deallocate the aligned memory.
    void* Realloc(void* p, std::size_t n); //Reallocate the memory.
    void* ReallocAligned(void* p, std::size_t n, std::size_t align); //Reallocate the memory with alignment.
    void* Calloc(std::size_t n, std::size_t size); //Allocate and initialize n amount of memory.

    std::size_t AllocUsableSize(void* ptr);
    void CollectThreadMemoryCache(); //Reduces memory fragmentation.
}

#endif //HYPERFLOW_HUTILFUNCS_H
