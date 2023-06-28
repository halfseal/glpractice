#ifndef OBJECT_H
#define OBJECT_H

#include "glad.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"

#include <vector>

class Point
{
public:
    // pointcloud coodrinates
    std::vector<glm::vec3> Positions;
    // gl variables
    unsigned int VAO;
    unsigned int VBO;

    Point(std::vector<glm::vec3> &positions)
    {
        Positions = positions;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBufferData(GL_ARRAY_BUFFER, Positions.size() * sizeof(glm::vec3), Positions.data(), GL_STATIC_DRAW);

        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    }

    void Refresh()
    {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, Positions.size() * sizeof(glm::vec3), Positions.data(), GL_STATIC_DRAW);
    }

    void Draw()
    {
        glPointSize(1.0f);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, Positions.size());
    }

    ~Point()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

class Box
{
public:
    std::vector<glm::vec3> Vertices;
    // gl variables
    unsigned int VAO;
    unsigned int VBO;

    Box()
    {
        Vertices = {
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f, 1.0f, -1.0f),
            glm::vec3(1.0f, 1.0f, -1.0f),
            glm::vec3(-1.0f, 1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),

            glm::vec3(-1.0f, -1.0f, 1.0f),
            glm::vec3(1.0f, -1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(-1.0f, 1.0f, 1.0f),
            glm::vec3(-1.0f, -1.0f, 1.0f),

            glm::vec3(-1.0f, 1.0f, 1.0f),
            glm::vec3(-1.0f, 1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f, 1.0f),
            glm::vec3(-1.0f, 1.0f, 1.0f),

            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),

            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, 1.0f),
            glm::vec3(1.0f, -1.0f, 1.0f),
            glm::vec3(-1.0f, -1.0f, 1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),

            glm::vec3(-1.0f, 1.0f, -1.0f),
            glm::vec3(1.0f, 1.0f, -1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(-1.0f, 1.0f, 1.0f),
            glm::vec3(-1.0f, 1.0f, -1.0f),
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(glm::vec3), Vertices.data(), GL_STATIC_DRAW);
    }

    void DrawFill()
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, Vertices.size());
    }

    void DrawLine()
    {
        glLineWidth(1.0f);
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, Vertices.size());
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    ~Box()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

#endif
