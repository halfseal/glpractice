#include "glad.h"
#include "glfw3.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

#include "source/shader.h"
#include "source/camera.h"
#include "source/object.h"
#include "source/octree.h"

#include <iostream>
#include <unordered_map>
#include <tuple>

void showInstructions();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void motion_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void readVerticesFromFile(const std::string& filename, std::vector<glm::vec3>& vertices);
void downsample(std::vector<glm::vec3>& vertices, const float gridSize);

unsigned long long makeUniqueNumber(glm::vec3 vertex, const float boxSize);

// settings
const unsigned int SCR_WIDTH = 1280; // 800;
const unsigned int SCR_HEIGHT = 780; // 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
// Camera camera(glm::vec3(0.0f, 60.0f, 25.0f), glm::vec3(0.0f, 1.0f, 0.0f), 00.0f, -89.0f);

Point* pointcloud = nullptr;
Box* box = nullptr;

const float VOXELSIZE = 0.25f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Octree octree(VOXELSIZE, 512.0f);

int main()
{
    showInstructions();

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "simulating", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, motion_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader shader("shaders/main_vert.glsl", "shaders/main_frag.glsl");

    std::vector<glm::vec3> vertices;
    readVerticesFromFile("assests/pointcloud1.txt", vertices);
    pointcloud = new Point(vertices);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // printf("camerapos : %f, %f, %f,\tyaw: %f,\tpitch: %f\n", camera.Position.x, camera.Position.y, camera.Position.z, camera.Yaw, camera.Pitch);
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        shader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);

        // model transformation
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);

        if (box != nullptr) {
            std::vector<glm::vec3> cboxvec;
            octree.findByY(octree.getRoot(), camera.Position.y, cboxvec);
            std::unordered_map<unsigned long long, bool> cboxmap;

            for (const auto& pos : cboxvec) {
                cboxmap[makeUniqueNumber(pos, VOXELSIZE)] = true;

                model = glm::mat4(1.0f);
                model = glm::translate(model, pos);
                model = glm::scale(model, glm::vec3(VOXELSIZE / 2.0f));
                shader.setMat4("model", model);

                shader.setVec4("color", glm::vec4(229.0f / 255.0f, 83.0f / 255.0f, 0.0f, 1.0f));
                box->DrawFill();
                shader.setVec4("color", glm::vec4(0.0f));
                box->DrawLine();
            }
            for (const auto& pos : pointcloud->Positions) {
                if (cboxmap.find(makeUniqueNumber(pos, VOXELSIZE)) != cboxmap.end()) {
                    continue;
                }

                model = glm::mat4(1.0f);
                model = glm::translate(model, pos);
                model = glm::scale(model, glm::vec3(VOXELSIZE / 2.0f));
                shader.setMat4("model", model);

                shader.setVec4("color", glm::vec4(1.0f));
                box->DrawFill();
                shader.setVec4("color", glm::vec4(0.0f));
                box->DrawLine();
            }

        }
        else {
            shader.setVec4("color", glm::vec4(1.0f));
            pointcloud->Draw();
        }


        // printf("z : %f\n", camera.Position.y);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void showInstructions()
{
    printf("Click and Drag to Loock Around.\n");
    printf("Press 'W/A/S/D/Up/Down' to Move.\n");
    printf("Press 'Q/Esc' to quit.\n");
    printf("Press 'P' to Downsample.\n");
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        downsample(pointcloud->Positions, VOXELSIZE);
        pointcloud->Refresh();
        box = new Box();
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        glm::dvec2 pos;
        glfwGetCursorPos(window, &pos.x, &pos.y);
        if (action == GLFW_PRESS) {
            camera.IsClicked = true;
            camera.FirstMouse = true;
        }
        else if (action == GLFW_RELEASE) {
            camera.IsClicked = false;
        }
    }
}

void motion_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera.ProcessMotion(static_cast<float>(xpos), static_cast<float>(ypos));
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// Put vertices in vertices array from file
void readVerticesFromFile(const std::string& filename, std::vector<glm::vec3>& vertices)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        float x, y, z;
        if (!(iss >> x >> y >> z)) {
            std::cout << "Failed to read line: " << line << std::endl;
            continue;
        }

        vertices.emplace_back(x, y, z);
    }

    file.close();
}

bool isDownsapled = false;
void downsample(std::vector<glm::vec3>& vertices, const float gridSize)
{
    if (isDownsapled)
        return;
    isDownsapled = true;

    /*
        glm::vec3 min = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
        glm::vec3 max = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);
        for (const auto &vertex : vertices)
        {
            if (vertex.x < min.x)
                min.x = vertex.x;
            if (vertex.y < min.y)
                min.y = vertex.y;
            if (vertex.z < min.z)
                min.z = vertex.z;

            if (vertex.x > max.x)
                max.x = vertex.x;
            if (vertex.y > max.y)
                max.y = vertex.y;
            if (vertex.z > max.z)
                max.z = vertex.z;
        }
    */

    // auto combinedData = [](int _x, int _y, int _z) -> unsigned long long
    //     {
    //         unsigned long long x = static_cast<unsigned long long>(_x);
    //         unsigned long long y = static_cast<unsigned long long>(_y);
    //         unsigned long long z = static_cast<unsigned long long>(_z);
    //         unsigned long long combined = (x << 42) | (y << 21) | z;
    //         return combined;
    //     };

    // std::unordered_map<unsigned long long, glm::vec3> map;

    for (const auto& vertex : vertices) {
        // if (vertex.z >= 299.99) continue;
        // float fx = floor(vertex.x / gridSize);
        // float fy = floor(vertex.y / gridSize);
        // float fz = floor(vertex.z / gridSize);

        // int x = static_cast<int>(fx);
        // int y = static_cast<int>(fy);
        // int z = static_cast<int>(fz);

        // auto combined = combinedData(x, y, z);

        // auto it = map.find(combined);
        // if (it == map.end())
        //     map[combined] = std::move(glm::vec3(
        //         fx * gridSize + gridSize / 2.0f,
        //         fy * gridSize + gridSize / 2.0f,
        //         fz * gridSize + gridSize / 2.0f));

        octree.insert(octree.getRoot(), std::move(vertex));
    }
    // printf("number of leaves :\t%ld\n", octree.getLeafCount());
    // octree.printAllToFile(octree.getRoot(), "octreelog.txt");

    std::vector<glm::vec3> ocvec;
    octree.octreeToVector(octree.getRoot(), ocvec);

    printf("number of points :\t%ld\n", vertices.size());

    printf("number of ocvec :\t%ld\n", ocvec.size());

    // vertices.clear();
    // vertices.reserve(map.size());
    // for (const auto& entry : map)
    //     vertices.push_back(entry.second);
    // printf("number of voxel :\t%ld\n", vertices.size());


    vertices.clear();
    vertices.reserve(ocvec.size());
    for (const auto& o : ocvec)
        vertices.push_back(o);
}

unsigned long long makeUniqueNumber(glm::vec3 vertex, const float boxSize) {
    float fx = floor(vertex.x / boxSize);
    float fy = floor(vertex.y / boxSize);
    float fz = floor(vertex.z / boxSize);

    int _x = static_cast<int>(fx);
    int _y = static_cast<int>(fy);
    int _z = static_cast<int>(fz);

    unsigned long long x = static_cast<unsigned long long>(_x);
    unsigned long long y = static_cast<unsigned long long>(_y);
    unsigned long long z = static_cast<unsigned long long>(_z);
    unsigned long long combined = (x << 42) | (y << 21) | z;

    return combined;
}