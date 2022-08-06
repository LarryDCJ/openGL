//-----------------------------------------------------------------------------
//  Larry D. Cawley Jr.
//
//  CS330
//
//-----------------------------------------------------------------------------


#include "ShapeBuilder."
#include "mesh.h"
#include "SceneBuilder.h"

// image/texture loading
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>      // Image loading Utility functions

#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>
#include <GLFW/glfw3.h>     // GLFW library

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/camera.h>


using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif


const char* const WINDOW_TITLE = "Week 5 Milestone"; // Macro for window title

// Variables for window width and height
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

ShapeBuilder builder;

// // Triangle mesh data
// GLMesh gMesh;

// Main GLFW window
GLFWwindow* gWindow = nullptr;

// Shader program
// GLuint gProgramId;
GLuint gShaderProgram;

// vector of GLMeshes with each GLMesh type being a different object
std::vector<GLMesh> scene;

// Toggle perspective view
bool perspective = false;

// Movement - Camera
Camera gCamera(glm::vec3(0.0f, 2.5f, 4.0f)); // (horizontal vertical depth)( up xyz ) yaw pitch
float gLastX = WINDOW_WIDTH / 2.0f;
float gLastY = WINDOW_HEIGHT / 2.0f;
bool gFirstMouse = true;

// Frame Timing
float gDeltaTime = 0.0f; // time between current frame and last frame
float gLastFrame = 0.0f;

// Texture
GLuint gTextureId;
glm::vec2 gUVScale(1.0f, 1.0f); // scale of the texture
GLint gTexWrapMode = GL_REPEAT;


/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */

// Initialize the program
bool UInitialize(int, char*[], GLFWwindow** window);

// Set the window size
void UResizeWindow(GLFWwindow* window, int width, int height);

// Processes inputs by the user
void UProcessInput(GLFWwindow* window);

// Reners the graphics of all the GLMeshes in the scene
void URender(vector<GLMesh> scene);

// Compiles the shades for the program
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint &programId);

// Release the memory after the program is finished
void UDestroyShaderProgram(GLuint programId);
void UDestroyMesh(GLMesh& mesh);
void UDestroyTexture(GLuint textureId);


// build the objects that get pushed_back into the scene vector
void UBuildCylinder(GLMesh& mesh, vector<float> properties, float radius, float length);
void drawDesk(GLMesh &mesh);

void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);

// Creates the textures for the objects in the scene
bool UCreateTexture(const char* fileName, GLuint &textureId);
// void UCreateMesh(GLMesh &mesh);


/* Vertex Shader Source Code*/
const GLchar * vertexShaderSource = GLSL(440,
    layout (location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
    layout (location = 2) in vec2 textureCoordinate;  // Color data from Vertex Attrib Pointer 1

    out vec2 vertexTextureCoordinate; // variable to transfer color data to the fragment shader

    //Global variables for the transform matrices - Camera Movement
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
        shapeColor = color; // transfer color data to the fragment shader
        vertexTextureCoordinate = textureCoordinate; // reference incoming texture
    }
);


/* Fragment Shader Source Code*/
const GLchar * fragmentShaderSource = GLSL(440,
    out vec4 fragmentColor;

    in vec3 shapeColor
    in vec2 vertexTextureCoordinate;

    uniform sampler2D uTexture;
    uniform vec2 uvScale;

    void main()
    {
        fragmentColor = texture(uTexture, vertexTextureCoordinate) * vec4(shapeColor, 1.0f); // multiply the texture with the color
    }
);

// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char *image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}

int main(int argc, char* argv[])
{
    // gCamera.Pitch = -30.0F;

    // Ensure proper initialization
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    SceneBuilder::UBuildScene(scene);

    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;

    // // Create the mesh
    // UCreateMesh(gMesh); // Calls the function to create the Vertex Buffer Object

    for(auto& m : scene)
    {
        if (!UCreateTexture(m.texFilename, m.textureId))
        {
            cout << "Failed to load texture " << m.texFilename << endl;
            return EXIT_FAILURE;
        }

        // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gShaderProgram))
        {
            return EXIT_FAILURE;
        }

    }

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gShaderProgram);

    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 0);






    // // Load texture (relative to project's directory)
    // const char * texFilename = "wood.png";
    // if (!UCreateTexture(texFilename, gTextureId))
    // {
    //     cout << "Failed to load texture " << texFilename << endl;
    //     return EXIT_FAILURE;
    // }


    // render loop checks if the window is open
    int i;
    while (!glfwWindowShouldClose(gWindow))
    {
        // Sets the background color of the window to beige (it will be implicitely used by glClear)
        glClearColor(245.0f, 245.0f, 220.0f, 1.0f);

        // per-frame timing
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URender(scene);

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        if (i % 30 == 0) {
            cout << "FPS: " << 1.0f / gDeltaTime << endl;
            cout << "X: " << gCamera.Position.x << "Y: " << gCamera.Position.y << "Z: " << gCamera.Position.z << " R: " << gCamera.Position.r << " degrees" << endl;
        }
        i++;
    }

    for (auto& m : scene)
    {
        // Release mesh data
        UDestroyMesh(m);

    }

    scene.clear();

    // // Release texture
    // UDestroyTexture(gTextureId);

    // Release shader program
    UDestroyShaderProgram(gShaderProgram);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}

// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && gTexWrapMode != GL_REPEAT)
    {
        glBindTexture(GL_TEXTURE_2D, gTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_REPEAT;

        cout << "Current Texture Wrapping Mode: REPEAT" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && gTexWrapMode != GL_MIRRORED_REPEAT)
    {
        glBindTexture(GL_TEXTURE_2D, gTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_MIRRORED_REPEAT;

        cout << "Current Texture Wrapping Mode: MIRRORED REPEAT" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && gTexWrapMode != GL_CLAMP_TO_EDGE)
    {
        glBindTexture(GL_TEXTURE_2D, gTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_CLAMP_TO_EDGE;

        cout << "Current Texture Wrapping Mode: CLAMP TO EDGE" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && gTexWrapMode != GL_CLAMP_TO_BORDER)
    {
        float color[] = {1.0f, 0.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

        glBindTexture(GL_TEXTURE_2D, gTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_CLAMP_TO_BORDER;

        cout << "Current Texture Wrapping Mode: CLAMP TO BORDER" << endl;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
    {
        gUVScale += 0.1f;
        cout << "Current scale (" << gUVScale[0] << ", " << gUVScale[1] << ")" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
    {
        gUVScale -= 0.1f;
        cout << "Current scale (" << gUVScale[0] << ", " << gUVScale[1] << ")" << endl;
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) // press R to reset the camera
        gCamera.Position = glm::vec3(-1.0f, 2.0f, 5.0f);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            if (action == GLFW_PRESS)
                cout << "Left mouse button pressed" << endl;
            else
                cout << "Left mouse button released" << endl;
        }
        break;

        case GLFW_MOUSE_BUTTON_MIDDLE:
        {
            if (action == GLFW_PRESS)
                cout << "Middle mouse button pressed" << endl;
            else
                cout << "Middle mouse button released" << endl;
        }
        break;

        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            if (action == GLFW_PRESS)
                cout << "Right mouse button pressed" << endl;
            else
                cout << "Right mouse button released" << endl;
        }
        break;

        default:
            cout << "Unhandled mouse button event" << endl;
            break;
    }
}

// Function called to render a frame
void URender()
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. Scales the object by 2
    glm::mat4 scale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
    // 2. Rotates shape by 0 degrees on the x axis
    glm::mat4 rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Sets object to the origin
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
    // model matrix: combines all transformations from left to right
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

    glUseProgram(gProgramId);

    //Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMesh.vao);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nIndices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}

// Implements the UCreateMesh function
void UCreateMesh(GLMesh &mesh)
{
    drawDesk(mesh);
}

bool UCreateTexture(const char* filename, GLuint &textureId)
{
    int width, height, channels;
    unsigned char *image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture.

        return true;
    }

    // Error loading the image
    return false;
}

void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}

void UDestroyMesh(GLMesh &mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(1, &mesh.vbo);
}

// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint &programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}

void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}

void drawDesk(GLMesh &mesh)
{
    // Vertex data
    GLfloat verts[] = {
        // Desktop
        //Positions          //Texture Coordinates
        -1.0f, -0.00f, -0.5f,  0.0f, 0.0f,
         1.0f, -0.00f, -0.5f,  1.0f, 0.0f,
         1.0f,  0.05f, -0.5f,  1.0f, 1.0f,
         1.0f,  0.05f, -0.5f,  1.0f, 1.0f,
        -1.0f,  0.05f, -0.5f,  0.0f, 1.0f,
        -1.0f, -0.00f, -0.5f,  0.0f, 0.0f,

        -1.0f, -0.00f,  0.5f,  0.0f, 0.0f,
         1.0f, -0.00f,  0.5f,  1.0f, 0.0f,
         1.0f,  0.05f,  0.5f,  1.0f, 1.0f,
         1.0f,  0.05f,  0.5f,  1.0f, 1.0f,
        -1.0f,  0.05f,  0.5f,  0.0f, 1.0f,
        -1.0f, -0.00f,  0.5f,  0.0f, 0.0f,

        -1.0f,  0.05f,  0.5f,  1.0f, 0.0f,
        -1.0f,  0.05f, -0.5f,  1.0f, 1.0f,
        -1.0f, -0.00f, -0.5f,  0.0f, 1.0f,
        -1.0f, -0.00f, -0.5f,  0.0f, 1.0f,
        -1.0f, -0.00f,  0.5f,  0.0f, 0.0f,
        -1.0f,  0.05f,  0.5f,  1.0f, 0.0f,

         1.0f,  0.05f,  0.5f,  1.0f, 0.0f,
         1.0f,  0.05f, -0.5f,  1.0f, 1.0f,
         1.0f, -0.00f, -0.5f,  0.0f, 1.0f,
         1.0f, -0.00f, -0.5f,  0.0f, 1.0f,
         1.0f, -0.00f,  0.5f,  0.0f, 0.0f,
         1.0f,  0.05f,  0.5f,  1.0f, 0.0f,

        -1.0f, -0.00f, -0.5f,  0.0f, 1.0f,
         1.0f, -0.00f, -0.5f,  1.0f, 1.0f,
         1.0f, -0.00f,  0.5f,  1.0f, 0.0f,
         1.0f, -0.00f,  0.5f,  1.0f, 0.0f,
        -1.0f, -0.00f,  0.5f,  0.0f, 0.0f,
        -1.0f, -0.00f, -0.5f,  0.0f, 1.0f,

        -1.0f,  0.05f, -0.5f,  0.0f, 1.0f,
         1.0f,  0.05f, -0.5f,  1.0f, 1.0f,
         1.0f,  0.05f,  0.5f,  1.0f, 0.0f,
         1.0f,  0.05f,  0.5f,  1.0f, 0.0f,
        -1.0f,  0.05f,  0.5f,  0.0f, 0.0f,
        -1.0f,  0.05f, -0.5f,  0.0f, 1.0f

    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerUV = 2;

    mesh.nIndices = sizeof(verts) / (sizeof(verts[0])) * (floatsPerVertex + floatsPerUV);

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create VBO
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates
    GLint stride =  sizeof(float) * (floatsPerVertex + floatsPerUV);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex)));
    glEnableVertexAttribArray(2);

}