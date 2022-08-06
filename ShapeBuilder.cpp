# include <cstdlib>
# include <GL/eglew.h>
# include <GLFW/glfw3.h>
# include <glm/glm.hpp>
# include <glm/gtx/transform.hpp>
# include <glm/glm.hpp>
# include <glm/gtc/type_ptr.hpp>
# include <vector>
# include <cmath>

# include "ShapeBuilder.h"

using namespace std;

void Shapebuilder::UBuildCylinder(GLMesh& mesh)
{
    vector<float> c = { mesh.p[0], mesh.p[1], mesh.p[2], mesh.p[3] };

    float r = mesh.radius;
    float l = mesh.length;
    float s = mesh.numSides;
    float h = mesh.height;

    constexpr float pi = 3.14159265359;
    const float sectorStep = 2 * pi / s;

    vector<float> v;

    for (auto i=1; i < s + 1; i++)
    {
        // triangle fan, bottom
        v.insert(v.end(), { 0.5f , 0.5f, 0.0f, c[0], c[1], c[2], c[3], 0.5f, 0.5f });
        v.insert(v.end(), { 0.5f + r * cos(i * sectorStep),           // x
                            0.5f + r * sin(i * sectorStep),           // y
                            0.0f ,                                    // z
                            c[0], c[1], c[2], c[3],                   // color r g b a
                            0.5f + (r * cos(i*sectorStep)),           // texture x
                            0.5f + (r * sin(i*sectorStep))});         // texture y
        v.insert(v.end(), { 0.5f + r * cos((i + 1) * sectorStep),
                            0.5f + r * sin((i + 1) * sectorStep),
                            0.0f,
                            c[0], c[1], c[2], c[3],                   // color r g b a
                            0.5f + (r * cos((i + 1) * sectorStep)),
                            0.5f + (r * sin((i + 1) * sectorStep)) });
    }

    for (auto i=1; i < s + 1; i++)
    {
        // triangle fan, top
        v.insert(v.end(), { 0.5f + 0.5f, l, c[0], c[1], c[2], c[3], 0.5f, 0.5f });
        v.insert(v.end(), { 0.5f + r * cos(i * sectorStep),
                            0.5f + r * sin(i * sectorStep),
                            l,
                            c[0], c[1], c[2], c[3],                   // color r g b a
                            0.5f + (r * cos((i + 1) * sectorStep)),
                            0.5f + (r * sin((i + 1) * sectorStep)) });
        v.insert(v.end(), { 0.5f + r * cos(i * sectorStep),
                            0.5f + r * sin(i * sectorStep),
                            l,
                            c[0], c[1], c[2], c[3],                   // color r g b a
                            0.5f + (r * cos((i + 1) * sectorStep)),
                            0.5f + (r * sin((i + 1) * sectorStep)) });
    }

    constexpr float x = 3.0f;
    float j = 1.0f / (s/x);
    float k = 0.0f

    // sides
    for (auto i = 1; i < s + 1; i++)
    {
        v.insert(v.end(), { 0.5f + r * cos(i * sectorStep),
                            0.5f + r * sin(i * sectorStep),
                            0.0f,
                            c[0], c[1], c[2], c[3],                   // color r g b a
                            k,
                            0 });

        v.insert(v.end(), { 0.5f + r * cos(i * sectorStep),
                            0.5f + r * sin(i * sectorStep),
                            0.0f,
                            l,
                            c[0], c[1], c[2], c[3],                   // color r g b a
                            k,
                            1.0f });

        v.insert(v.end(), { 0.5f + r * cos((i + 1) * sectorStep),
                            0.5f + r * sin((i + 1) * sectorStep),
                            0.0f,
                            l,
                            c[0], c[1], c[2], c[3],                   // color r g b a
                            k + j,
                            1.0f });

        v.insert(v.end(), { 0.5f + r * cos((i + 1) * sectorStep),
                            0.5f + r * sin((i + 1) * sectorStep),
                            l,
                            c[0], c[1], c[2], c[3],                   // color r g b a
                            k + j,
                            1.0f });

        v.insert(v.end(), { 0.5f + r * cos((i + 1) * sectorStep),
                            0.5f + r * sin((i + 1) * sectorStep),
                            0.0f,
                            c[0], c[1], c[2], c[3],                   // color r g b a
                            k + j,
                            0.0f });

        v.insert(v.end(), { 0.5f + r * cos(i * sectorStep),
                            0.5f + r * sin(i * sectorStep),
                            0.0f,
                            c[0], c[1], c[2], c[3],                   // color r g b a
                            k,
                            0.0f });
        k += j;
    }

    mesh.v = v;
    v.clear();
    UTranslator(mesh);
}