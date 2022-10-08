#ifndef CPU_SCENE_H
#define CPU_SCENE_H

#include <vector>
#include <random>

#include "util.h"

std::vector<Sphere> spheres;
std::vector<vec3> lights;

void random_sphere() {
    // Random position
    float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/40)) - 20;
    float y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1.5)) - 3.5;
    float z = -5 - static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/35));

    // Random color
    float xx = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1));
    float yy = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1));
    float zz = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1));

    spheres.push_back({{x,y,z}, y+4, {1.0, {0.9, 0.5, 0.1, 0.0}, {xx, yy, zz}, 50.}});
}

void random_light() {
    int x = -30 + rand() % 61;
    int y = 20 + rand() % 31;
    int z = -30 + rand() % 61;

    lights.push_back({static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)});
}

void gen_scene(int n_spheres, int n_lights) {
    srand(time(NULL));

    // These are constant between scenes to provide mirror and dielectric views
    spheres.push_back({{7,    5,    -18}, 3, mirror});
    spheres.push_back({{-1.0, -2, -12}, 2, glass});

    for (int i = 0; i < n_spheres; i++) {
        random_sphere();
    }

    for (int i = 0; i < n_lights; i++) {
        random_light();
    }
}

#endif
