#pragma once

#include <vector>
#include "Vec.h"
#include "Triangle.h"

class Model
{
private:

public:
    const std::vector<vec3f> verticies;
    const std::vector<Triangle> triangles;

    Model(std::vector<vec3f> verticies, std::vector<Triangle> triangles):
        verticies(std::move(verticies)),
        triangles(std::move(triangles)){}
};