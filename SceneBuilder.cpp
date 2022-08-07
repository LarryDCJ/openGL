#include "SceneBuilder.h"
#include "ShapeBuilder.h"
#include "Mesh.h"

using namespace std;


void SceneBuilder::UBuildScene(vector<GLMesh>& scene)
{

     srand(time(nullptr));

     GLMesh coffeeCup;
     coffeeCup.p = {
         1.0f,  1.0f, 1.0f, 1.0f, // color data rgba
         1.0f,  1.0f, 1.0f,       // scale @ x y z
         0.0f,  1.0f, 0.0f, 0.0f, // x amount of rotation and rotation @ x y z
         0.0f,  0.0f, 1.0f, 0.0f, // y amount of rotation and rotation @ x y z
         0.0f,  0.0f, 0.0f, 1.0f, // z amount of rotation and rotation @ x y z
         1.0f,  0.0f, 0.0f,       // translation @ x y z
         1.0f,  1.0f              // texture size in relation to the object size
     };
     coffeeCup.texFilename = "textures/coffee_cup.png";
     coffeeCup.height = 0.8f;
     coffeeCup.length = 1.0f;
     coffeeCup.radius = 0.5f;
     coffeeCup.innerRadius = 0.475f;
     coffeeCup.numSides = 361;
     ShapeBuilder::UBuildHollowCylinder(coffeeCup);
     scene.push_back(coffeeCup);



    GLMesh coffeeCupTop;
    coffeeCupTop.p = {
          1.0f,  1.0f, 1.0f, 1.0f, // color data rgba
          1.0f,  1.0f, 1.0f,       // scale @ x y z
          0.0f,  1.0f, 0.0f, 0.0f, // x amount of rotation and rotation @ x y z
          0.0f,  0.0f, 1.0f, 0.0f, // y amount of rotation and rotation @ x y z
          0.0f,  0.0f, 0.0f, 1.0f, // z amount of rotation and rotation @ x y z
          1.0f,  0.7f, 0.0f,       // translation @ x y z
          1.0f,  1.0f,              // texture size in relation to the object size
    };
    coffeeCupTop.texFilename = "textures/coffee.png";
    coffeeCupTop.radius = 0.48f;
    coffeeCupTop.numSides = 360;
    ShapeBuilder::UBuildCircle(coffeeCupTop);
    scene.push_back(coffeeCupTop);

    GLMesh handle;
	handle.p = {
		0.5f,	0.5f,	0.5f,	1.0f,
		1.0f,	1.0f,	1.0f,
		90.0f,	1.0f,	0.0f,	0.0f,
		0.0f,	0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f,
		.5f,	0.9f,	0.48f,
		1.0f,	1.0f
	};
	handle.texFilename = "textures/white.png";
	handle.innerRadius = 0.20f;
	handle.radius = 0.3f;
	handle.height = 0.1f;
	handle.numSides = 360.0f;
	ShapeBuilder::UBuildHollowCylinder(handle);
	scene.push_back(handle);


    GLMesh desk;
    desk.p = {
        1.0f,  0.0f, 0.0f, 1.0f, // color data rgba
        2.0f,  1.0f, 6.0f,       // scale @ x y z
        0.0f,  1.0f, 0.0f, 0.0f, // x amount of rotation and rotation @ x y z
        0.0f,  0.0f, 1.0f, 0.0f, // y amount of rotation and rotation @ x y z
        0.0f,  0.0f, 0.0f, 1.0f, // z amount of rotation and rotation @ x y z
        1.0f,  0.0f, 0.0f,       // translation @ x y z
        1.0f,  1.0f              // texture size in relation to the object size
    };
    desk.texFilename = "textures/wood.png";
    ShapeBuilder::UBuildPlane(desk);
    scene.push_back(desk);

}