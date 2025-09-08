#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include "hmeshconvertor.h"
#include "hstrconversion.h"
#include <lz4.h>

#include "hyperflow.h"

namespace ml
{
    bool WriteMesh(const MeshInfo& meshInfo, const std::string& outputFilePath)
    {
        uint32_t submeshCount = meshInfo.headers.size();
        uint32_t headerDataSize = meshInfo.headers.size() * sizeof(SubMeshHeader);
        uint32_t fullHeaderSize = sizeof(uint32_t) + headerDataSize + 1;
        uint32_t offset = 0;
        std::vector<char> headerData = std::vector<char>(fullHeaderSize);

        memcpy(headerData.data() + offset, &submeshCount, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(headerData.data() + offset, meshInfo.headers.data(), headerDataSize);
        offset += headerDataSize;
        headerData[offset] = '\0';

        offset = 0;
        std::vector<char> meshData{};
        for (uint32_t i = 0; i < submeshCount; i++)
        {
            auto& subMesh = meshInfo.subMeshes[i];
            offset = 0;

            uint32_t submeshDataSize = 1;
            uint32_t positionSize = 0;
            uint32_t normalSize = 0;
            uint32_t colorSize = 0;
            uint32_t texcoordSize = 0;
            uint32_t indexSize = subMesh.indices.size();

            if (!subMesh.positions.empty())
            {
                positionSize = subMesh.positions.size() * sizeof(float);
                submeshDataSize++;
            }
            if (!subMesh.normals.empty())
            {
                submeshDataSize++;
                normalSize = subMesh.normals.size() * sizeof(float);
            }
            if (!subMesh.colors.empty())
            {
                submeshDataSize++;
                colorSize = subMesh.colors.size() * sizeof(float);
            }
            if (!subMesh.texCoords.empty())
            {
                submeshDataSize++;
                texcoordSize = subMesh.texCoords.size() * sizeof(float);
            }

            submeshDataSize += positionSize + normalSize + colorSize + texcoordSize + indexSize;
            uint64_t oldSize = meshData.size();
            meshData.resize(meshData.size() + submeshDataSize);

            if (positionSize > 0)
            {
                memcpy(meshData.data() + oldSize + offset, subMesh.positions.data(), positionSize);
                offset += positionSize;
                meshData[oldSize + offset] = '\0';
                offset++;
            }

            if (normalSize > 0)
            {
                memcpy(meshData.data() + oldSize + offset, subMesh.normals.data(), normalSize);
                offset += normalSize;
                meshData[offset + oldSize] = '\0';
                offset++;
            }

            if (colorSize > 0)
            {
                memcpy(meshData.data() + oldSize + offset, subMesh.colors.data(), colorSize);
                offset += colorSize;
                meshData[offset + oldSize] = '\0';
                offset++;
            }

            if (texcoordSize > 0)
            {
                memcpy(meshData.data() + oldSize + offset, subMesh.texCoords.data(), texcoordSize);
                offset += texcoordSize;
                meshData[offset + oldSize] = '\0';
                offset++;
            }

            memcpy(meshData.data() + oldSize + offset, subMesh.indices.data(), indexSize);
            offset += indexSize;
            meshData[offset + oldSize] = '\0';
        }

        const int maxCompressedSize = LZ4_compressBound((int)meshData.size());
        auto compressedBuffer = (char*)malloc(maxCompressedSize);
        if (!compressedBuffer) std::cerr << "Failed to allocate memory" << std::endl;

        auto dataSize = LZ4_compress_default(meshData.data(), compressedBuffer, (int)meshData.size(), maxCompressedSize);

        if (dataSize <= 0)
        {
            std::cerr << "Failed to compress file" << std::endl;
            free(compressedBuffer);
            return false;
        }

        std::ofstream outFile(outputFilePath, std::ios::binary);
        if (!outFile)
        {
            std::cerr << "Failed to open output file" << std::endl;
            free(compressedBuffer);
            return false;
        }

        outFile.write(headerData.data(), fullHeaderSize);
        outFile.write(compressedBuffer, dataSize);
        outFile.close();

        free(compressedBuffer);
        return true;
    }

    bool LoadModel(const char* path, MeshInfo* meshInfo)
    {
        std::string_view sv(path);
        auto dotPos = sv.find_last_of('.');
        if (dotPos == std::string_view::npos)
        {
            std::cerr << "Unknown model type! cannot extract file extension!";
            return false;
        }

        const std::string_view ext = sv.substr(dotPos + 1);
        auto type = hf::STRING_TO_MODEL_TYPE(ext);

        switch (type)
        {
        case hf::ModelType::Unknown: return false;
        case hf::ModelType::Obj: LoadObj(path, meshInfo); break;
        case hf::ModelType::Fbx: LoadFbx(path, meshInfo); break;
        case hf::ModelType::Gltf:
        case hf::ModelType::Glb:  LoadGltf(path, meshInfo); break;
        }

        return true;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 3 || (argc - 1) % 2 != 0)
    {
        std::cerr << "Usage: " << argv[0] << " <input1> <output1> [<input2> <output2> ...]\n";
        return 1;
    }

    std::ios_base::sync_with_stdio(true);

    for (uint32_t i = 1; i < argc; i += 2)
    {
        std::string inputPath = argv[i];
        std::string outputPath = argv[i + 1];

        try
        {
            ml::MeshInfo meshInfo{};
            ml::LoadModel(inputPath.c_str(), &meshInfo);

            if (ml::WriteMesh(meshInfo, outputPath))
                std::cout << "Successfully converted " << inputPath << " to " << outputPath << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }

    return 0;
}