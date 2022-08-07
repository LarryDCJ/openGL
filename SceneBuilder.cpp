#include "SceneBuilder.h"
#include "ShapeBuilder.h"
#include "Mesh.h"

using namespace std;


void SceneBuilder::UBuildScene(vector<GLMesh>& scene)
{

    srand(time(nullptr));

    GLMesh coffeeCup;
    coffeeCup.p = {
         1.0f,  0.0f, 0.0f, 1.0f, // color data rgba
         1.0f,  1.0f, 1.0f,       // scale @ x y z
         0.0f,  1.0f, 0.0f, 0.0f, // x amount of rotation and rotation @ x y z
         0.0f,  0.0f, 1.0f, 0.0f, // y amount of rotation and rotation @ x y z
         0.0f,  0.0f, 0.0f, 1.0f, // z amount of rotation and rotation @ x y z
         0.05f, 0.0f, 0.0f,       // translation @ x y z
         1.0f,  1.0f              // texture size in relation to the object size
    };
    coffeeCup.texFilename = "./smiley.png";
    coffeeCup.height = 0.8f;
    coffeeCup.length = 1.0f;
    coffeeCup.radius = 0.5f;
    coffeeCup.innerRadius = 0.25f;
    coffeeCup.numSides = 20;
    ShapeBuilder::UBuildCylinder(coffeeCup);
    scene.push_back(coffeeCup);


    GLMesh desk;
    desk.p = {
         1.0f,  1.0f, 1.0f, 1.0f, // color data rgba
         4.0f,  2.0f, 6.0f,       // scale @ x y z
         0.0f,  1.0f, 0.0f, 0.0f, // x amount of rotation and rotation @ x y z
         0.0f,  0.0f, 1.0f, 0.0f, // y amount of rotation and rotation @ x y z
         0.0f,  0.0f, 0.0f, 1.0f, // z amount of rotation and rotation @ x y z
         1.0f,  0.0f, 0.0f,       // translation @ x y z
         1.0f,  1.0f              // texture size in relation to the object size
    };
    desk.texFilename = "wood.png";
    ShapeBuilder::UBuildPlane(desk);
    scene.push_back(desk);

}