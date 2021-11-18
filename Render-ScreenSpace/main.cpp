/*
Ensure that you run the executable from the build/bin directory. Shader paths are not updated dynamically despite being relative.
*/
#include "./include/render.h"
#include "./include/stb_image.h"

#include <config.h>
#include <sph.h>

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
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
static const glm::vec2 blurDirX = glm::vec2(1.0f / SCR_WIDTH, 0.0f);
static const glm::vec2 blurDirY = glm::vec2(0.0f, 1.0f / SCR_HEIGHT);
static float filterRadius = 300;
static const glm::vec4 color = glm::vec4(.275f, 0.65f, 0.85f, 0.9f);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

static bool lbutton_down = false;
double mouseX_init = 0;
double mouseY_init = 0;

//Light
glm::vec3 lightPos = glm::vec3(1.0f, 5.0f, 35.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// SPH simulation
static SPH::Simulation* sph;
float scalingFactor = 1;


int main()
{
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


    //-----------------------SKYBOX Shader Program-------------------
    // Skybox shaders
    Shader skyboxShader("../../Render-ScreenSpace/shader/skybox.vert", "../../Render-ScreenSpace/shader/skybox.frag");

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    // skyboxVAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> faces
    {
        "../../Render-ScreenSpace/Assets/cubemap/right.png",
        "../../Render-ScreenSpace/Assets/cubemap/left.png",
        "../../Render-ScreenSpace/Assets/cubemap/top.png",
        "../../Render-ScreenSpace/Assets/cubemap/bottom.png",
        "../../Render-ScreenSpace/Assets/cubemap/front.png",
        "../../Render-ScreenSpace/Assets/cubemap/back.png"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    
    //--------------Initialise the SPH simulation----------------------------
    sph = new SPH::Simulation(nullptr);
    int particleCount = SPH::Config::ParticleCount;
    float particleRadius = SPH::Config::ParticleRadius;
    scalingFactor = 2; //Ideally set to a value such that the resultant pointsize is more than 4.

    glm::vec3 particlePos[particleCount];

    for(int i=0; i< particleCount; i++)
    {
        particlePos[i] = glm::vec3(sph->particles[i].position.x, sph->particles[i].position.y, sph->particles[i].position.z);
    }


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
    unsigned int blurFBO_v, blurFBO_h, blurVBO, blurVAO, blurEBO;
    glGenBuffers(1, &blurEBO);
    glGenBuffers(1, &blurVBO);
    glGenVertexArrays(1, &blurVAO);
    glGenFramebuffers(1, &blurFBO_v);
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO_v);

    GLuint blurTex_v;
    glGenTextures(1, &blurTex_v);
    glBindTexture(GL_TEXTURE_2D, blurTex_v);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, blurTex_v, 0);

    
    glGenBuffers(1, &blurEBO);
    glGenBuffers(1, &blurVBO);
    glGenVertexArrays(1, &blurVAO);
    glGenFramebuffers(1, &blurFBO_h);
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO_h);

    GLuint blurTex_h;
    glGenTextures(1, &blurTex_h);
    glBindTexture(GL_TEXTURE_2D, blurTex_h);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, blurTex_h, 0);

    //--------------------------------------------------THICKNESS SHADER PROGRAM--------------------------------------
    Shader thicknessShader("../../Render-ScreenSpace/shader/sprite.vert","../../Render-ScreenSpace/shader/thickness.frag");
    unsigned int thickFBO, thickVBO, thickVAO, thickEBO;
    glGenBuffers(1, &thickEBO);
    glGenBuffers(1, &thickVBO);
    glGenVertexArrays(1, &thickVAO);
    glGenFramebuffers(1, &thickFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, thickFBO);

    GLuint thickTex;
    glGenTextures(1, &thickTex);
    glBindTexture(GL_TEXTURE_2D, thickTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, thickTex, 0);

    //------------------------------------------------(Apparently PreFinal) COMPILATION------------------------------------------------
    Shader finalShader("../../Render-ScreenSpace/shader/final.vert", "../../Render-ScreenSpace/shader/final.frag");
    unsigned int finalFBO, finalVBO, finalVAO, finalEBO;
    glGenBuffers(1, &finalEBO);
    glGenBuffers(1, &finalVBO);
    glGenVertexArrays(1, &finalVAO);
    glGenFramebuffers(1, &finalFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);

    GLuint finalTex;
    glGenTextures(1, &finalTex);
    glBindTexture(GL_TEXTURE_2D, finalTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalTex, 0);

    //-----------------------------------------------------------FINAL COMPILATION-------------------------------------
    // Shader fShader("f.vert", "f.frag");
    // GLuint fFBO, fEBO, fVBO, fVAO;
    // initBuffers(fFBO, fEBO, fVBO, fVAO);
    // GLuint fTEX;
    // initTexture(SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_RGBA32F, fTEX);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fTEX, 0);
    //-----------------------------------------------RENDER LOOP-------------------------------------------------------
    // -----------
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


        // Run SPH Simulation and Update positions
        // sph->Run();
        // for(int i=0; i< particleCount; i++)
        // {
        //     particlePos[i] = glm::vec3(sph->particles[i].position.x, sph->particles[i].position.y, sph->particles[i].position.z);
        // }

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

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

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        depthShader.setMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        depthShader.setMat4("view", view);

        // glBindFramebuffer(GL_FRAMEBUFFER, 0);  //THis enables the buffer for drawing
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

        // glBindFramebuffer(GL_FRAMEBUFFER, 0);  //THis enables the buffer for drawing
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
        blurShader.setFloat("blurScale", 0.1f);

        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
        blurShader.setFloat("blurScale", 0.1f);

        glEnable(GL_DEPTH_TEST);
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);  //THis enables the buffer for drawing
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDisable(GL_DEPTH_TEST);

        // //------------------------------THICKNESS OF FLUID PROGRAM-------------------------------------
        // thicknessShader.use();
        // glBindFramebuffer(GL_FRAMEBUFFER, thickFBO);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // glBindVertexArray(depthVAO);
    
        // thicknessShader.setFloat("pointScale", (float)SCR_HEIGHT * (1.0f / tanf(camera.Zoom * 0.5f)) );
        // thicknessShader.setFloat("pointRadius", scalingFactor*particleRadius);

        // depthShader.setMat4("projection", projection);
        // depthShader.setMat4("view", view);

        // // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glEnable(GL_BLEND);
        // glBlendFunc(GL_ONE, GL_ONE);
        // glBlendEquation(GL_FUNC_ADD);
        // glDepthMask(GL_FALSE);
        // glEnable(GL_PROGRAM_POINT_SIZE);

        // for (unsigned int i = 0; i < particleCount; i++)
        // {
        //     // calculate the model matrix for each object and pass it to shader before drawing
        //     glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        //     model = glm::translate(model, particlePos[i]);
        //     // model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));
        //     thicknessShader.setMat4("model", model);

        //     glDrawArrays(GL_POINTS, 0, 1);
        // }
        // glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
        // glDisable(GL_DEPTH_TEST);
        // glDisable(GL_BLEND);

        // // //------------------------------COMPILATION OF ALL STEPS----------------------------------------
        // finalShader.use();
        // glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);

        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// glBindVertexArray(finalVAO);
		
		// glBindBuffer(GL_ARRAY_BUFFER, finalVBO);
		// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, finalEBO);
		// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		// glEnableVertexAttribArray(0);

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, blurTex_h);
        // depthMAP = glGetUniformLocation(finalShader.ID, "depthMap");
        // glUniform1i(depthMAP, 0);

        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, thickTex);
        // GLint thicknessMap = glGetUniformLocation(finalShader.ID, "thicknessMap");
        // glUniform1i(thicknessMap, 1);

        // // glActiveTexture(GL_TEXTURE2);
        // // glBindTexture(GL_TEXTURE_2D, plane.tex);
        // // GLint sceneMap = glGetUniformLocation(fluidFinal.program, "sceneMap");
        // // glUniform1i(sceneMap, 2);

        // finalShader.setMat4("projection", projection);
        // finalShader.setMat4("mView", view);
        // finalShader.setVec2("invTexScale", glm::vec2(1.0f/SCR_WIDTH, 1.0f/SCR_HEIGHT));
        // finalShader.setVec4("color", color);

        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glEnable(GL_DEPTH_TEST);
        // glDepthMask(GL_TRUE);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glDisable(GL_DEPTH_TEST);

        //-----------------------DRAW SKYBOX at last (ENable and Disable the Depth test)----------------
        // glEnable(GL_DEPTH_TEST);
        // glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        // skyboxShader.use();
        // view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        // skyboxShader.setMat4("view", view);
        // skyboxShader.setMat4("projection", projection);
        // // skybox cube
        // glBindVertexArray(skyboxVAO);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // glBindVertexArray(0);
        // glDepthFunc(GL_LESS); // set depth function back to default
        // glDisable(GL_DEPTH_TEST);

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