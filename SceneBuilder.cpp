# include "SceneBuilder.h"
# include "ShapeBuilder.h"
# include "mesh.h"

using namespace std;

void SceneBuilder::UBuildScene{std::vector<GLMesh>& scene}
{

#define textureFolderLocation "/home/lcawley/codebase/openGL/"

#define concat(first, second) first second

    srand(time(nullptr));

    GLMesh gMesh01;
    gMesh01.p = {
        1.0f, 1.0f, 1.0f, 1.0f, // color data rgba
        2.0f, 2.0f, 2.0f, // scale @ x y z
        0.0f, 1.0f, 0.0f, 0.0f, // x amount of rotation and rotation @ x y z
        45.0f, 0.0f, 1.0f, 0.0f, // y amount of rotation and rotation @ x y z
        0.0f, 0.0f, 0.0f, 1.0f,   // z amount of rotation and rotation @ x y z
        -1.5f, 0.0f, 0.0f, 0.0f, // translation @ x y z
        1.0f, 1.0f // texture size in relation to the object size
    };
    gMesh01.height = 0.8f;
#define texture "smiley.png"
    gMesh01.texture = concat(textureFolderLocation, texture);
    gMesh01.length = 1.0f;
    gMesh01.radius = 0.5f;
    gMesh01.numSides = 20;
    gMesh01.type = GL_TRIANGLE_FAN;
    ShapeBuilder::UBuildCylinder(gMesh01);
    scene.push_back(gMesh01);

}