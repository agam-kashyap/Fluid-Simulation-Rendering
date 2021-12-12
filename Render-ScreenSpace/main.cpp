/*
Ensure that you run the executable from the build/bin directory. Shader paths are not updated dynamically despite being relative.
*/
#include "./include/render.h"
#include "./include/stb_image.h"

#include <config.h>
#include <sph.h>
#include <string>
#include <fstream>
#include <iostream>
#include <unistd.h>

glm::vec3 parseParticlePositions(char *line);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow *window);
void initBuffers(GLuint &fbo, GLuint &ebo, GLuint &vbo, GLuint &vao);
void initTexture(int width, int height, GLenum format, GLenum internalFormat, GLuint &tex);


// SKybox helper functions
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(std::vector<std::string> faces);


// settings
const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 900;
static const glm::vec2 blurDirX = glm::vec2(1.0f / SCR_WIDTH, 0.0f);
static const glm::vec2 blurDirY = glm::vec2(0.0f, 1.0f / SCR_HEIGHT);
static float filterRadius = 10;
static const glm::vec4 color = glm::vec4(.275f, 0.65f, 0.85f, 0.9f);
float const blurScale = 0.1;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

static bool lbutton_down = false;
double mouseX_init = 0;
double mouseY_init = 0;

//Light
glm::vec3 lightPos = glm::vec3(0.0f, 6.0f, 30.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// SPH simulation
static SPH::Simulation* sph;
float scalingFactor = 1; //Ideally set to a value such that the resultant pointsize is more than 4.
int particleCount;
float particleRadius;

int main(int argc, char *argv[])
{
    std::string filename = argv[1];
    std::ifstream posFile;
    try
    {
        posFile.open(filename);
        // let the first line of the file contain all the necessary parameters
        if(posFile.is_open())
        {
            std::string line;
            getline(posFile, line);
            char *lineStr = new char[line.length() + 1];
            strcpy(lineStr, line.c_str());
            // particle Count
            const char *ptr;
            ptr = strtok(lineStr, " ");
            particleCount = std::atoi(ptr);
            //particle Radius
            ptr = strtok(NULL, " ");
            particleRadius = std::atof(ptr);
            std::cout << particleCount << " " << particleRadius << std::endl;

            // //---------------------OpenGL code begins-------------------
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            #ifdef __APPLE__
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            #endif

            GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Screen Space Rendering of SPH Fluid", NULL, NULL);
            if (window == NULL)
            {
                std::cout << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                return -1;
            }
            glfwMakeContextCurrent(window);
            glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
            glfwSetMouseButtonCallback(window, mouse_button_callback); //This function moves the camera based on the mouse drag

            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                std::cout << "Failed to initialize GLAD" << std::endl;
                return -1;
            }
            
            //--------------Initialise the SPH simulation----------------------------
            sph = new SPH::Simulation(nullptr);

            glm::vec3 particlePos[particleCount];
            
            float progress = 0.0;
            int barWidth = 70;
            for(int i=0; i< particleCount; i++)
            {
                getline(posFile, line);
                lineStr = new char[line.length() + 1];
                strcpy(lineStr, line.c_str());
                particlePos[i] = parseParticlePositions(lineStr);

                std::cout << "[";
                int pos = barWidth * progress;
                for (int i = 0; i < barWidth; ++i) {
                        if (i < pos) std::cout << "=";
                        else if (i == pos) std::cout << ">";
                        else std::cout << " ";
                }
                std::cout << "] " << int(progress * 100.0) << " %\r";
                std::cout.flush();

                progress += 1.0/particleCount; 
            }

            //---------------Infinite Plane program----------------------------------
            Shader planeShader("../../Render-ScreenSpace/shader/plane.vert", "../../Render-ScreenSpace/shader/plane.frag");
            GLuint planeFBO, planeEBO, planeVAO, planeVBO, planeTex;
            initBuffers(planeFBO, planeEBO, planeVBO, planeVAO);
            initTexture(SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_RGBA32F, planeTex);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, planeTex, 0);
            GLfloat planeVertices[] = {
                100.0f, -100.0f, 100.0f,
                100.0f, -100.0f, -100.0f,
                -100.0f, -100.0f, -100.0f,
                -100.0f, -100.0f, 100.0f
            };
            GLuint planeIndices[] = {
                0, 1, 3,
                1, 2, 3
            };
            
            //-----------------------DEPTH SHADER PROGRAM-----------------------------
            Shader depthShader("../../Render-ScreenSpace/shader/sprite.vert","../../Render-ScreenSpace/shader/sprite.frag");

            float vertices[] = {
                0.0f, 0.0f, 0.0f
            };
            GLuint depthFBO, depthEBO, depthVBO, depthVAO;
            initBuffers(depthFBO, depthEBO, depthVBO, depthVAO);
            GLuint depthTEX;
            initTexture(SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, depthTEX);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTEX, 0);
            glBindVertexArray(depthVAO);
            glBindBuffer(GL_ARRAY_BUFFER, depthVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, depthVBO);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(0);
            
            // //--------------------------BLUR SHADER PROGRAM-------------------------------
            Shader blurShader("../../Render-ScreenSpace/shader/blur.vert","../../Render-ScreenSpace/shader/blur.frag");
            GLuint blurFBO_v, blurFBO_h, blurVBO, blurVAO, blurEBO;
            initBuffers(blurFBO_v, blurEBO, blurVBO, blurVAO);

            GLuint blurTex_v;
            initTexture(SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, blurTex_v);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, blurTex_v, 0);

            initBuffers(blurFBO_h, blurEBO, blurVBO, blurVAO);
            GLuint blurTex_h;
            initTexture(SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, blurTex_h);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, blurTex_h, 0);

            //--------------------------------------------------THICKNESS SHADER PROGRAM--------------------------------------
            Shader thicknessShader("../../Render-ScreenSpace/shader/sprite.vert","../../Render-ScreenSpace/shader/thickness.frag");
            GLuint thickFBO, thickVBO, thickVAO, thickEBO;
            initBuffers(thickFBO, thickEBO, thickVBO, thickVAO);
            GLuint thickTex;
            initTexture(SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_RGBA32F, thickTex);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, thickTex, 0);

            //------------------------------------------------COMPILATION------------------------------------------------
            Shader finalShader("../../Render-ScreenSpace/shader/final.vert", "../../Render-ScreenSpace/shader/final.frag");
            GLuint finalFBO, finalVBO, finalVAO, finalEBO;
            initBuffers(finalFBO, finalEBO, finalVBO, finalVAO);
            GLuint finalTex;
            initTexture(SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_RGBA32F, finalTex);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalTex, 0);

            //-----------------------------------------------RENDER LOOP-------------------------------------------------------
            while (!glfwWindowShouldClose(window))
            {
                // per-frame time logic
                // --------------------
                float currentFrame = glfwGetTime();
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;

                // input
                // -----
                processInput(window);
                if(lbutton_down)
                {
                    // Grab the current location of the mouse and find the delta between it and the last
                    double m_x, m_y;
                    glfwGetCursorPos(window, &m_x, &m_y);
                    double mouseX_move = m_x - mouseX_init;
                    double mouseY_move = m_y - mouseY_init;
                    
                    mouseX_init = m_x;
                    mouseY_init = m_y;

                    camera.ProcessMouseButtonMovement(mouseX_move, mouseY_move, SCR_WIDTH, SCR_HEIGHT);
                }
                else
                {
                    glfwGetCursorPos(window, &mouseX_init, &mouseY_init);
                }


                //---------------------Read file and Update positions-------------------------------------------
                for(int i=0; i< particleCount; i++)
                {
                    getline(posFile, line);
                    lineStr = new char[line.length() + 1];
                    strcpy(lineStr, line.c_str());
                    particlePos[i] = parseParticlePositions(lineStr);
                }

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                //-----------------Infinite Plane Shader-------------------------------------------------------------------------
                planeShader.use();
                glBindFramebuffer(GL_FRAMEBUFFER, planeFBO);
                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
                planeShader.setMat4("projection", projection);
                glm::mat4 view = camera.GetViewMatrix();
                planeShader.setMat4("mView", view);

                glBindVertexArray(planeVAO);
                glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
                glEnableVertexAttribArray(0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                
                //-----------------Depth(Sprite) Shader to render the spheres----------------------------------------------------
                depthShader.use();
                glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);

                glClear(GL_DEPTH_BUFFER_BIT);

                depthShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
                depthShader.setVec3("lightPos", lightPos);
                depthShader.setVec3("particleColor", glm::vec3(0.0, 0.3, 1.0));
                depthShader.setVec3("cameraViewPos", camera.Position);

                depthShader.setFloat("heightOfNearPlane", (float)SCR_HEIGHT * (1.0f / tanf(camera.Zoom * 0.5f)) );
                depthShader.setFloat("particleRadius", scalingFactor*particleRadius);

                depthShader.setMat4("projection", projection);
                depthShader.setMat4("view", view);

                glBindVertexArray(depthVAO);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_PROGRAM_POINT_SIZE);
                for (unsigned int i = 0; i < particleCount; i++)
                {
                    // calculate the model matrix for each object and pass it to shader before drawing
                    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
                    model = glm::translate(model, particlePos[i]);
                    // model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));
                    depthShader.setMat4("model", model);

                    glDrawArrays(GL_POINTS, 0, 1);
                }
                glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
                
                //-----------------------Add Shader program for Blur----------------------
                blurShader.use();
                // // vertical blur
                glBindFramebuffer(GL_FRAMEBUFFER, blurFBO_v);
                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);

                glClear(GL_DEPTH_BUFFER_BIT);
                // // vertical quad
                GLfloat vertices[] = {
                        1.0f,  1.0f,
                        1.0f, -1.0f,
                    -1.0f, -1.0f,
                    -1.0f,  1.0f
                };
                GLuint indices[] = {
                    0, 1, 3,
                    1, 2, 3
                };

                glBindVertexArray(blurVAO);
                glBindBuffer(GL_ARRAY_BUFFER, blurVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blurEBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
                glEnableVertexAttribArray(0);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, depthTEX);
                GLint depthMAP = glGetUniformLocation(blurShader.ID, "depthMap");
                glUniform1i(depthMAP, 0);

                blurShader.setMat4("projection", projection);
                blurShader.setVec2("screenSize", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
                blurShader.setVec2("blurDir", blurDirY);
                blurShader.setFloat("filterRadius", filterRadius);
                blurShader.setFloat("blurScale", blurScale);

                glEnable(GL_DEPTH_TEST);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            
                // // Horizontal Blur
                glBindFramebuffer(GL_FRAMEBUFFER, blurFBO_h);
                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);

                glClear(GL_DEPTH_BUFFER_BIT);

                glBindVertexArray(blurVAO);
                glBindBuffer(GL_ARRAY_BUFFER, blurVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blurEBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
                glEnableVertexAttribArray(0);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, blurTex_v);
                depthMAP = glGetUniformLocation(blurShader.ID, "depthMap");
                glUniform1i(depthMAP, 0);

                blurShader.setMat4("projection", projection);
                blurShader.setVec2("screenSize", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
                blurShader.setVec2("blurDir", blurDirX);
                blurShader.setFloat("filterRadius", filterRadius);
                blurShader.setFloat("blurScale", blurScale);

                glEnable(GL_DEPTH_TEST);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glDisable(GL_DEPTH_TEST);

                //------------------------------THICKNESS OF FLUID PROGRAM-------------------------------------
                thicknessShader.use();
                glBindFramebuffer(GL_FRAMEBUFFER, thickFBO);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glBindVertexArray(depthVAO);
            
                thicknessShader.setFloat("pointScale", (float)SCR_HEIGHT * (1.0f / tanf(camera.Zoom * 0.5f)) );
                thicknessShader.setFloat("pointRadius", scalingFactor*particleRadius);

                depthShader.setMat4("projection", projection);
                depthShader.setMat4("view", view);

                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);
                glBlendEquation(GL_FUNC_ADD);
                glDepthMask(GL_FALSE);
                glEnable(GL_PROGRAM_POINT_SIZE);

                for (unsigned int i = 0; i < particleCount; i++)
                {
                    // calculate the model matrix for each object and pass it to shader before drawing
                    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
                    model = glm::translate(model, particlePos[i]);
                    // model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));
                    thicknessShader.setMat4("model", model);

                    glDrawArrays(GL_POINTS, 0, 1);
                }
                glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_BLEND);

                // //------------------------------COMPILATION OF ALL STEPS----------------------------------------
                finalShader.use();
                // glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glBindVertexArray(finalVAO);
                
                glBindBuffer(GL_ARRAY_BUFFER, finalVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, finalEBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
                glEnableVertexAttribArray(0);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, blurTex_h);
                depthMAP = glGetUniformLocation(finalShader.ID, "depthMap");
                glUniform1i(depthMAP, 0);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, thickTex);
                GLint thicknessMap = glGetUniformLocation(finalShader.ID, "thicknessMap");
                glUniform1i(thicknessMap, 1);

                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, planeTex);
                GLint sceneMap = glGetUniformLocation(finalShader.ID, "sceneMap");
                glUniform1i(sceneMap, 2);

                finalShader.setMat4("projection", projection);
                finalShader.setMat4("mView", view);
                finalShader.setVec2("invTexScale", glm::vec2(1.0f/SCR_WIDTH, 1.0f/SCR_HEIGHT));
                finalShader.setVec4("color", color);

                
                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_TRUE);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glDisable(GL_DEPTH_TEST);

                // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
                // -------------------------------------------------------------------------------
                glfwSwapBuffers(window);
                glfwPollEvents();
            }

            // optional: de-allocate all resources once they've outlived their purpose:
            // ------------------------------------------------------------------------
            glDeleteVertexArrays(1, &depthVAO);
            glDeleteBuffers(1, &depthVBO);

            // glfw: terminate, clearing all previously allocated GLFW resources.
            // ------------------------------------------------------------------
            glfwTerminate();
            return 0;
                   
        }
        posFile.close();
    }
    catch(std::ifstream::failure& e)
    {
        std::cout << filename << ": FILE NOT READ PROPERLY" << '\n';
    }
}
glm::vec3 parseParticlePositions(char *line)
{
    float posX, posY, posZ;
    const char *ptr;
    ptr = strtok(line, " ");
    if(ptr == "nan" || ptr == "-nan") posX = 0.0;
    else posX = std::stof(ptr);
    ptr = strtok(NULL, " ");
    if(ptr == "nan" || ptr == "-nan") posY = 0.0;
    else posY = std::stof(ptr);
    ptr = strtok(NULL, " ");
    if(ptr == "nan" || ptr == "-nan") posY = 0.0;
    else posZ = std::stof(ptr);

    return glm::vec3(posY, posZ, posX);
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
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
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if(action == GLFW_PRESS)
        {
            lbutton_down = true;
        }
        else if (action == GLFW_RELEASE)
        {
            lbutton_down = false;
        }
    }
}


// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


void initBuffers(GLuint &fbo, GLuint &ebo, GLuint &vbo, GLuint &vao)
{
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void initTexture(int width, int height, GLenum format, GLenum internalFormat, GLuint &tex) 
{
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
}