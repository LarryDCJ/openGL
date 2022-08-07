#include "ShapeBuilder.h"
#include "Mesh.h"
#include "SceneBuilder.h"

// image/texture loading
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>      // Image loading Utility functions

#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE


/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

using namespace std; // Standard namespace

const char* const WINDOW_TITLE = "Week 5 Milestone"; // Macro for window title

// Variables for window width and height
const int WINDOW_WIDTH  = 1920;
const int WINDOW_HEIGHT = 1080;

struct GLightMesh
{
    GLuint vao;
    GLuint vbo;
    GLuint nVertices;
};

GLightMesh ambLightMesh;
GLightMesh keyLightMesh;
ShapeBuilder builder;

// Main GLFW window
GLFWwindow* gWindow = nullptr;

// Shader program
GLuint gShaderProgram;
GLuint gLightProgramId;

// vector of GLMeshes with each GLMesh type being a different object
vector<GLMesh> scene;

// Toggle perspective view
bool perspective = false;

// Movement - Camera
Camera gCamera(glm::vec3(-5.0f, 4.5f, -0.3f), glm::vec3(0.0f, 1.0f, 0.0f), 10.0f, -30.0f);

float gLastX = WINDOW_WIDTH / 2.0f;
float gLastY = WINDOW_HEIGHT / 2.0f;
bool gFirstMouse = true;

// Frame Timing
float gDeltaTime = 0.0f; // time between current frame and last frame
float gLastFrame = 0.0f;

// Light color, position and scale
glm::vec3 gSpotLightColor(1.0f, 1.0f, 1.0f);
glm::vec3 gSpotLightPosition(2.5f, 1.0f, -1.5f);
glm::vec3 gSpotLightScale(0.1f);

// Light color, position and scale
glm::vec3 gKeyLightColor(0.0f, 0.0f, 0.2f);
glm::vec3 gKeyLightPosition(-2.5f, 5.0f, -1.5f);
glm::vec3 gKeyLightScale(0.1f);

bool gSpotLightOrbit = true;

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

// Renders the graphics of all the GLMeshes in the scene
void URender(vector<GLMesh> scene);

// Compiles the shades for the program
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource,
    GLuint &programId);

// Release the memory after the program is finished
void UDestroyMesh(GLMesh &mesh);
void UDestroyShaderProgram(GLuint programId);
void UDestroyTexture(GLuint textureId);

// // build the objects that get pushed_back into the scene vector
// void UBuildHollowCylinder(GLMesh &mesh, vector<float> properties, float radius, float length);
// void UBuildPlane(GLMesh &mesh, vector<float> properties);

void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);

// Creates the textures for the objects in the scene
bool UCreateTexture(const char* fileName, GLuint &textureId);
void UCreateLightMesh(GLightMesh &lightMesh);


/* Vertex Shader Source Code*/
const GLchar *vertexShaderSource = GLSL(440,
	layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
	layout(location = 1) in vec3 normal; // VAP position 1 for normals
	layout(location = 2) in vec2 textureCoordinate;

	out vec3 vertexNormal; // For outgoing normals to fragment shader
	out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
	out vec2 vertexTextureCoordinate;

	// Camera and projection matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main()
	{
		gl_Position = projection * view * model * vec4(position, 1.0f);
		vertexFragmentPos = vec3(model * vec4(position, 1.0f));
		vertexNormal = mat3(transpose(inverse(model))) * normal;
		vertexTextureCoordinate = textureCoordinate;
	}
);


/* Fragment Shader Source Code*/
const GLchar *fragmentShaderSource = GLSL(440,

	in vec3 vertexFragmentPos;
	in vec3 vertexNormal;
	in vec2 vertexTextureCoordinate; // for texture coordinates, not color

	out vec4 fragmentColor;

	uniform vec3 objectColor;
	uniform vec3 lightColor;
	uniform vec3 keyLightColor;
	uniform vec3 lightPos;
	uniform vec3 keyLightPos;
	uniform vec3 viewPosition;

	uniform sampler2D uTexture;
	uniform vec2 uvScale;

	void main()
	{
		//Calculate Ambient lighting*/
		float spotStrength = 0.1f; // Set ambient or global lighting strength
		float keyStrength = 1.0f; // Set ambient or global lighting strength
		vec3 spot = spotStrength * lightColor; // Generate ambient light color
		vec3 key = keyStrength * keyLightColor;

		//Calculate Diffuse lighting*/
		vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
		vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
		vec3 keyLightDirection = normalize(keyLightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube

		float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
		float keyImpact = max(dot(norm, keyLightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light

		vec3 diffuse = impact * lightColor; // Generate diffuse light color
		vec3 keyDiffuse = keyImpact * keyLightColor;

		//Calculate Specular lighting*/
		float specularIntensity = 0.4f; // Set specular light strength
		float highlightSize = 16.0f; // Set specular highlight size
		vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
		vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
		//Calculate specular component
		float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
		vec3 specular = specularIntensity * specularComponent * lightColor;
		vec3 keySpecular = specularIntensity * specularComponent * keyLightColor;

		// Texture holds the color to be used for all three components
		vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

		// Calculate phong result
		vec3 phong = (spot + key + diffuse /*+ keyDiffuse*/ + specular /*+ objectColor*/) * textureColor.xyz;

		fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU

	}
);

// Light Shader Source Code
const GLchar* lampVertexShaderSource = GLSL(440,

	layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
}
);

// Light Fragment Shader Source Code
const GLchar* lampFragmentShaderSource = GLSL(440,

	out vec4 fragmentColor; // For outgoing light color to the GPU

void main()
{
	fragmentColor = vec4(1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
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
    // Ensure proper initialization
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    SceneBuilder::UBuildScene(scene);

    // Create the shader program
	for (auto& m : scene)
	{
		if (!UCreateTexture(m.texFilename, m.textureId))
		{
			cout << "Failed to load texture " << m.texFilename << endl;
			return EXIT_FAILURE;

		}

		if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gShaderProgram))
			return EXIT_FAILURE;


	}

	if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLightProgramId))
		return EXIT_FAILURE;

    	// Create Light Object
	UCreateLightMesh(ambLightMesh);
	UCreateLightMesh(keyLightMesh);


    for (auto& m : scene)
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
    glUniform1i(glGetUniformLocation(gShaderProgram, "uTexture"), 0);

    // render loop checks if the window is open
    while (!glfwWindowShouldClose(gWindow))
    {
        // Sets the background color of the window to brown/beige (it will be implicitely used by glClear)
        glClearColor(207.0f, 167.0f, 122.0f, 1.0f);

        // per-frame timing
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        UProcessInput(gWindow);

        // Render this frame
        URender(scene);

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

    }

    for (auto& m : scene)
    {
        // Release mesh data
        UDestroyMesh(m);
    }

    scene.clear();

    // Release shader program
    UDestroyShaderProgram(gShaderProgram);
	UDestroyShaderProgram(gLightProgramId);

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

    // draw lines
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// fill shapes
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		perspective = false;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		perspective = true;

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
void URender(vector<GLMesh> scene)
{
    // Borrowed from the Tutorial; animates the Spot Light to circle around the scene
	constexpr float angularVelocity = glm::radians(45.0f);
	if (gSpotLightOrbit)
	{
		glm::vec4 newPosition = glm::rotate(angularVelocity * gDeltaTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(gSpotLightPosition, 1.0f);
		gSpotLightPosition.x = newPosition.x;
		gSpotLightPosition.y = newPosition.y;
		gSpotLightPosition.z = newPosition.z;
	}

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection
    glm::mat4 projection;
    if(!perspective)
    {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    else

        projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);

    for(auto i = 0; i < scene.size(); ++i)
    {
        auto mesh = scene[i];

        glUseProgram(gShaderProgram);

        //Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gShaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(gShaderProgram, "view");
        GLint projLoc = glGetUniformLocation(gShaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mesh.model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        		// Reference matrix uniforms from the shape shader program for the shape color, light color, light position, and camera position
		GLint objectColorLoc = glGetUniformLocation(gShaderProgram, "objectColor");

		// Spotlight
		GLint lightColorLoc = glGetUniformLocation(gShaderProgram, "lightColor");
		GLint lightPositionLoc = glGetUniformLocation(gShaderProgram, "lightPos");

		// Key light
		GLint keyLightColorLoc = glGetUniformLocation(gShaderProgram, "keyLightColor");
		GLint keyLightPositionLoc = glGetUniformLocation(gShaderProgram, "keyLightPos");

		// Camera view
		GLint viewPositionLoc = glGetUniformLocation(gShaderProgram, "viewPosition");

		// Pass color, light, and camera data to the shape shader
		glUniform3f(objectColorLoc, mesh.p[0], mesh.p[1], mesh.p[2]);

		// Spot Light
		glUniform3f(lightColorLoc, gSpotLightColor.r, gSpotLightColor.g, gSpotLightColor.b);
		glUniform3f(lightPositionLoc, gSpotLightPosition.x, gSpotLightPosition.y, gSpotLightPosition.z);

		// Key Light
		glUniform3f(keyLightColorLoc, gKeyLightColor.r, gKeyLightColor.g, gKeyLightColor.b);
		glUniform3f(keyLightPositionLoc, gKeyLightPosition.x, gKeyLightPosition.y, gKeyLightPosition.z);


		const glm::vec3 cameraPosition = gCamera.Position;
		glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

        GLint UVScaleLoc = glGetUniformLocation(gShaderProgram, "uvScale");
        glUniform2fv(UVScaleLoc, 1, glm::value_ptr(mesh.gUVScale));

        // Activate the VBOs contained within the mesh's VAO
        glBindVertexArray(mesh.vao);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh.textureId);

        // Draws the triangles
        glDrawArrays(GL_TRIANGLES, 0, mesh.nIndices);
        // glDrawElements(GL_TRIANGLES, mesh.nIndices, GL_UNSIGNED_INT, 0);
    }

	// Draw the Spot Light
	glUseProgram(gLightProgramId);
	glBindVertexArray(ambLightMesh.vao);

	// Light location and Scale
	glm::mat4 model = glm::translate(gSpotLightPosition) * glm::scale(gSpotLightScale);

	// Matrix uniforms from the Light Shader program
	GLint modelLoc = glGetUniformLocation(gLightProgramId, "model");
	GLint viewLoc = glGetUniformLocation(gLightProgramId, "view");
	GLint projLoc = glGetUniformLocation(gLightProgramId, "projection");

	// Matrix data
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Draw the light
	glDrawArrays(GL_TRIANGLES, 0, ambLightMesh.nVertices);



	// Draw the Key Light
	glUseProgram(gLightProgramId);
	glBindVertexArray(keyLightMesh.vao);

	// Light location and Scale
	model = glm::translate(gKeyLightPosition) * glm::scale(gKeyLightScale);

	// Matrix uniforms from the Light Shader program
	modelLoc = glGetUniformLocation(gLightProgramId, "model");
	viewLoc = glGetUniformLocation(gLightProgramId, "view");
	projLoc = glGetUniformLocation(gLightProgramId, "projection");

	// Matrix data
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Draw the light
	glDrawArrays(GL_TRIANGLES, 0, keyLightMesh.nVertices);

	// deactivate vao's
	glBindVertexArray(0);
	glUseProgram(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.

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

void UDestroyMesh(GLMesh& mesh)
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

// Template for creating a cube light
void UCreateLightMesh(GLightMesh& lightMesh)
{
	// Position and Color data
	GLfloat verts[] = {
			//Positions          //Normals
			// ------------------------------------------------------
			//Back Face          //Negative Z Normal  Texture Coords.
		   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		   //Front Face         //Positive Z Normal
		  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
		   0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
		   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
		   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
		  -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
		  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

		  //Left Face          //Negative X Normal
		 -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 //Right Face         //Positive X Normal
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 //Bottom Face        //Negative Y Normal
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		//Top Face           //Positive Y Normal
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	lightMesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

	glGenVertexArrays(1, &lightMesh.vao); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(lightMesh.vao);

	// Create 2 buffers: first one for the vertex data; second one for the indices
	glGenBuffers(1, &lightMesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, lightMesh.vbo); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

	// Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}