/*
Ensure that you run the executable from the build/bin directory. Shader paths are not updated dynamically despite being relative.
*/
#include "./include/render.h"

#include <config.h>
#include <sph.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

static bool lbutton_down = false;
double mouseX_init = 0;
double mouseY_init = 0;

//Light
glm::vec3 lightPos = glm::vec3(1.0f, 20.0f, 40.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// SPH simulation
static SPH::Simulation* sph;
float scalingFactor = 1;


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback); //This function moves the camera based on the cursor position
    glfwSetMouseButtonCallback(window, mouse_button_callback); //This function moves the camera based on the mouse drag

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader zprogram
    // ------------------------------------
    Shader depthShader("../../Render-ScreenSpace/shader/sprite.vert","../../Render-ScreenSpace/shader/sprite.frag");

    // Initialise the SPH simulation
    sph = new SPH::Simulation(nullptr);
    int particleCount = SPH::Config::ParticleCount;
    float particleRadius = SPH::Config::ParticleRadius;
    scalingFactor = 2; //Ideally set to a value such that the resultant pointsize is more than 4.

    glm::vec3 particlePos[particleCount];

    for(int i=0; i< particleCount; i++)
    {
        particlePos[i] = glm::vec3(sph->particles[i].position.x, sph->particles[i].position.y, sph->particles[i].position.z);
    }

    float vertices[] = {
        0.0f, 0.0f, 0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    
    glEnable(GL_PROGRAM_POINT_SIZE);

    // render loop
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
            std::cout << "LBUTTON DOWN" << std::endl;
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

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        //Sprite Shader to render the spheres
        depthShader.use();

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

        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < particleCount; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, particlePos[i]);
            model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));
            depthShader.setMat4("model", model);

            glDrawArrays(GL_POINTS, 0, 1);
        }


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

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
    std::cout << "In Mouse Callback" << std::endl;
    
}