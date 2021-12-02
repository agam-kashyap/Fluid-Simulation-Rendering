#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "camera.h"
#include "sphere.h"


// class Renderer{

//     public:
//         Renderer();
//         Renderer(unsigned int width, unsigned int height);
//         int Run();

//     private: 
//         const unsigned int SCR_WIDTH;
//         const unsigned int SCR_HEIGHT;

//         // Camera
//         Camera camera;
//         float lastX;
//         float lastY;
//         bool firstMouse;

//         // timing
//         float deltaTime;
//         float lastFrame;

//         void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//         void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//         void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//         void processInput(GLFWwindow *window);
// };


#endif