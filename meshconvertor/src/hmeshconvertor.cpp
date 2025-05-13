#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"

struct MeshHeader {
    uint32_t vertexCount;
    uint32_t indexCount;
    uint32_t submeshCount;
};

struct SubMeshInfo {
    uint32_t indexOffset;
    uint32_t indexCount;
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: meshconv <input.obj> <output.mesh>" << std::endl;
        return 1;
    }

    std::string inputPath = argv[1];
    std::string outputPath = argv[2];

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputPath.c_str())) {
        std::cerr << "Failed to load OBJ file: " << warn << err << std::endl;
        return 1;
    }

    // Open output file
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open output file" << std::endl;
        return 1;
    }

    // Calculate total counts
    uint32_t totalVertices = 0;
    uint32_t totalIndices = 0;

    for (const auto& shape : shapes) {
        totalIndices += shape.mesh.indices.size();
    }

    // Write header
    MeshHeader header{
        static_cast<uint32_t>(attrib.vertices.size() / 3),
        totalIndices,
        static_cast<uint32_t>(shapes.size())
    };

    outFile.write(reinterpret_cast<char*>(&header), sizeof(header));

    // Write vertex data
    outFile.write(reinterpret_cast<char*>(attrib.vertices.data()),
                 attrib.vertices.size() * sizeof(float));

    if (!attrib.normals.empty()) {
        outFile.write(reinterpret_cast<char*>(attrib.normals.data()),
                     attrib.normals.size() * sizeof(float));
    }

    if (!attrib.texcoords.empty()) {
        outFile.write(reinterpret_cast<char*>(attrib.texcoords.data()),
                     attrib.texcoords.size() * sizeof(float));
    }

    // Write submesh info and indices
    uint32_t currentIndexOffset = 0;
    for (const auto& shape : shapes) {
        SubMeshInfo subMeshInfo{
            currentIndexOffset,
            static_cast<uint32_t>(shape.mesh.indices.size())
        };
        outFile.write(reinterpret_cast<char*>(&subMeshInfo), sizeof(subMeshInfo));

        // Write indices
        for (const auto& index : shape.mesh.indices) {
            uint32_t vertexIndex = index.vertex_index;
            outFile.write(reinterpret_cast<char*>(&vertexIndex), sizeof(vertexIndex));
        }

        currentIndexOffset += shape.mesh.indices.size();
    }

    outFile.close();
    std::cout << "Successfully converted " << inputPath << " to " << outputPath << std::endl;

    return 0;
}