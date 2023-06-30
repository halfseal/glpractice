#ifndef OCTREE_H
#define OCTREE_H

#include "../glm/glm/glm.hpp"
#include "../glm/glm/gtc/matrix_transform.hpp"
#include <vector>
#include <cmath>
#include <iostream>

glm::vec3 OffsetTable[8] = {
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(+1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, +1.0f, -1.0f),
    glm::vec3(+1.0f, +1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f, +1.0f),
    glm::vec3(+1.0f, -1.0f, +1.0f),
    glm::vec3(-1.0f, +1.0f, +1.0f),
    glm::vec3(+1.0f, +1.0f, +1.0f),
};

class OctreeNode
{
public:
    OctreeNode(glm::vec3 centre, float boxSize, unsigned int code, unsigned int depth)
    {
        c = centre;
        l = boxSize;
        Code = code;
        Depth = depth;
        for (int i = 0; i < 8; i++)
            Children[i] = nullptr;
    }
    ~OctreeNode()
    {
    }

    glm::vec3 c;
    float l;
    unsigned int Code;
    unsigned int Depth;
    OctreeNode* Children[8];
};

class Octree
{
public:
    // Octree(float boxSize = 64.0f, unsigned int maxDepth = 6)
    Octree(float voxelSize = 1.0f, float maxSize = 256.0f)
    {
        MaxDepth = 0;
        while (voxelSize * 2.0f <= maxSize) {
            voxelSize *= 2.0f;
            MaxDepth++;
        }
        std::cout << voxelSize << std::endl;
        Root = new OctreeNode(
            glm::vec3(0.0f, 0.0f, 0.0f),
            voxelSize,
            0,
            0
        );
    }
    ~Octree()
    {
    }

    void insert(OctreeNode* node, glm::vec3 point)
    {
        float _hl = node->l * 0.5f;
        if (point.x < node->c.x - _hl || point.x > node->c.x + _hl ||
            point.y < node->c.y - _hl || point.y > node->c.y + _hl ||
            point.z < node->c.z - _hl || point.z > node->c.z + _hl)
            return;

        if (node->Depth >= MaxDepth) {
            return;
        }
        // std::cout << "Centre: (" << node->c.x << ", " << node->c.y << ", " << node->c.z << ")" << std::endl;
        unsigned int code = 0;
        if (point.x > node->c.x)
            code |= 1;
        if (point.y > node->c.y)
            code |= 2;
        if (point.z > node->c.z)
            code |= 4;
        if (node->Children[code] == nullptr) {
            float newBoxsize = 0.5f * node->l;
            glm::vec3 offset = OffsetTable[code] * newBoxsize * 0.5f;
            glm::vec3 newCentre = node->c + offset;
            // std::cout << "newCentre: (" << newCentre.x << ", " << newCentre.y << ", " << newCentre.z << ")" << std::endl;

            node->Children[code] = new OctreeNode(
                newCentre, newBoxsize, code, node->Depth + 1
            );
            if (node->Children[code]->Depth >= MaxDepth)
                LeafCount++;
        }
        insert(node->Children[code], point);
    }

    void printAll(OctreeNode* node) {
        if (node == nullptr)
            return;
        if (node->Depth >= MaxDepth) {
            std::cout << "Point: (" << node->c.x << ", "
                << node->c.y << ", "
                << node->c.z << ")" << std::endl;
        }
        else {
            for (int i = 0; i < 8; i++) {
                printAll(node->Children[i]);
            }
        }
    }

    void printAllToFile(OctreeNode* node, const std::string& filename) {
        std::ofstream outputFile(filename);
        if (!outputFile.is_open()) {
            std::cout << "Failed to open file for writing." << std::endl;
            return;
        }

        printNodeToFile(node, outputFile);

        outputFile.close();
    }

    void printNodeToFile(OctreeNode* node, std::ofstream& outputFile) {
        if (node == nullptr)
            return;

        if (node->Depth >= MaxDepth) {
            outputFile << node->c.x << " " << node->c.y << " " << node->c.z << std::endl;
        }
        else {
            for (int i = 0; i < 8; i++) {
                printNodeToFile(node->Children[i], outputFile);
            }
        }
    }

    void octreeToVector(OctreeNode* node, std::vector<glm::vec3>& points) {
        if (node == nullptr)
            return;

        if (node->Depth >= MaxDepth) {
            points.push_back(node->c);
        }
        else {
            for (int i = 0; i < 8; i++) {
                octreeToVector(node->Children[i], points);
            }
        }
    }

    OctreeNode* getRoot() {
        return Root;
    }

    unsigned int getLeafCount() {
        return LeafCount;
    }

private:
    OctreeNode* Root;
    unsigned int MaxDepth;
    unsigned int LeafCount = 0;
};

#endif