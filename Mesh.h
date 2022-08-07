#pragma once

#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <vector>           // vector

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/camera.h>

struct GLMesh
{
    GLuint vao;         // Handle for the vertex array object
    GLuint nIndices;    // Number of indices of the mesh

    // Vertex buffer object is a buffer object that stores actual data for the vertices of an mesh
    GLuint vbo;         // Handle for the vertex buffer object
    GLuint vbos[2];     // Array of handles for the vertex buffer objects

    // mesh data
    std::vector<float> v;
    // color, scale, rotation, translation, texture size
    std::vector<float> p;

    //physical properties of the mesh
    float height;
    float length;
    float radius;
    float innerRadius;
    float numSides;

    // matrix object of the individual mesh
    glm::mat4 scale;
    glm::mat4 xrotation;
    glm::mat4 yrotation;
    glm::mat4 zrotation;
    glm::mat4 rotation;
    glm::mat4 translation;
    glm::mat4 model;
	glm::vec2 gUVScale;

    const char* texFilename;
    GLuint textureId;

    //texture wrapping modes
    GLint gTexWrapMode = GL_REPEAT;
    // GLint gTexWrapModeU = GL_MIRRORED_REPEAT;
    // GLint gTexWrapModeV = GL_CLAMP_TO_EDGE;
    // GLint gTexWrapModeW = GL_CLAMP_TO_BORDER;

    class Mesh
    {
    };

};