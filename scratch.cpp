#include "SceneBuilder.h"
#include "ShapeBuilder.h"
#include "Mesh.h"

using namespace std;


void SceneBuilder::UBuildScene(vector<GLMesh>& scene)
{

	// seed the rand() function once
	srand(time(nullptr));

// PEN BODY
	GLMesh cyl_gMesh01;
	cyl_gMesh01.p = {
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, -2.0f,
		1.0f, 1.0f
	};
	cyl_gMesh01.length = 4.0f;	cyl_gMesh01.radius = 0.5f;	cyl_gMesh01.numSides = 8.0f;
	cyl_gMesh01.texFilename = "textures/pen_body.png";
	ShapeBuilder::UBuildCylinder(cyl_gMesh01);
	scene.push_back(cyl_gMesh01);



	// PEN CLIP
	GLMesh cyl_gMesh02;
	cyl_gMesh02.p = {
		1.0f, 0.843f, 0.0f, 1.0f,
		1.5f, 0.2f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.75f, 0.9f, -1.5f,
		1.0f, 1.0f
	};
	cyl_gMesh02.length = 1.5f;	cyl_gMesh02.radius = 0.1f;	cyl_gMesh02.numSides = 8.0f;
	cyl_gMesh02.texFilename = "textures/pen_clip.png";
	ShapeBuilder::UBuildCylinder(cyl_gMesh02);
	scene.push_back(cyl_gMesh02);



	// PEN BUTT
	GLMesh cyl_gMesh03;
	cyl_gMesh03.p = {
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, -2.1f,
		1.0f, 1.0f
	};

	cyl_gMesh03.length = 3.0f;	cyl_gMesh03.radius = 0.4f;	cyl_gMesh03.numSides = 8.0f;
	cyl_gMesh03.texFilename = "textures/black.png";
	ShapeBuilder::UBuildCylinder(cyl_gMesh03);
	scene.push_back(cyl_gMesh03);


	// PEN TIP
	GLMesh con_gMesh01;
	con_gMesh01.p = {
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 2.0f,
		1.0f, 1.0f
	};
	con_gMesh01.length = 1.0f;	con_gMesh01.radius = 0.5f;	con_gMesh01.numSides = 8.0f;
	con_gMesh01.texFilename = "textures/pen_tip.png";
	ShapeBuilder::UBuildCone(con_gMesh01);
	scene.push_back(con_gMesh01);



	// DESK
	GLMesh plan_gMesh01;
	plan_gMesh01.p = {
		1.0f, 1.0f, 1.0f, 1.0f,				// color r, g, b a
		4.0f, 2.0f, 4.0f,					// scale x, y, z
		0.0f, 1.0f, 0.0f, 0.0f,				// x amount of rotation, rotate x, y, z
		45.0f, 0.0f, 1.0f, 0.0f,				// y amount of rotation, rotate x, y, z
		0.0f, 0.0f, 0.0f, 1.0f,				// z amount of rotation, rotate x, y, z
		1.0f, 0.0f, 0.0f,					// translate x, y, z
		1.0f, 1.0f
	};
	plan_gMesh01.texFilename = "textures/desk.png";
	ShapeBuilder::UBuildPlane(plan_gMesh01);
	scene.push_back(plan_gMesh01);


}

