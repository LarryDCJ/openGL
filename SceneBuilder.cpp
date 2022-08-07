#include "SceneBuilder.h"
#include "ShapeBuilder.h"
#include "Mesh.h"

using namespace std;

void SceneBuilder::UBuildScene(vector<GLMesh>& scene)
{

#define textureFolderLocation "/home/lcawley/codebase/openGL/"

#define concat(first, second) first second

    srand(time(nullptr));

    GLMesh gMesh00;
    gMesh00.height = 0.8f;
    gMesh00.length = 1.0f;
    gMesh00.radius = 0.5f;
    gMesh00.numSides = 20;
    gMesh00.p = {
         1.0f,  1.0f, 1.0f, 1.0f, // color data rgba
         2.0f,  2.0f, 2.0f,       // scale @ x y z
         0.0f,  1.0f, 0.0f, 0.0f, // x amount of rotation and rotation @ x y z
         45.0f, 0.0f, 1.0f, 0.0f, // y amount of rotation and rotation @ x y z
         0.0f,  0.0f, 0.0f, 1.0f, // z amount of rotation and rotation @ x y z
        -1.5f,  0.0f, 0.0f, 0.0f, // translation @ x y z
         1.0f,  1.0f              // texture size in relation to the object size
    };
    gMesh00.texFilename = "smiley.png";
    ShapeBuilder::UBuildCylinder(gMesh00);
    scene.push_back(gMesh00);

}