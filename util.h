#ifndef UTIL
#define UTIL

#include <tuple>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
// #include <omp.h>

struct vec3 {
    float x = 0;
    float y = 0;
    float z = 0;

    float &operator[](const int i) {
        return i == 0 ? x : (1 == i ? y : z);
    }

    const float &operator[](const int i) const {
        return i == 0 ? x : (1 == i ? y : z);
    }

    vec3 operator*(const float v) const {
        return {x * v, y * v, z * v};
    }

    float operator*(const vec3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    vec3 operator+(const vec3 &v) const {
        return {x + v.x, y + v.y, z + v.z};
    }

    vec3 operator-(const vec3 &v) const {
        return {x - v.x, y - v.y, z - v.z};
    }

    vec3 operator-() const {
        return {-x, -y, -z};
    }

    [[nodiscard]] float norm() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    [[nodiscard]] vec3 normalized() const {
        return (*this) * (1.f / norm());
    }
};

struct Material {
    float refraction = 1;
    float albedo[4] = {2, 0, 0, 0};
    vec3 diffuse = {0, 0, 0};
    float specular = 0;
};

struct Sphere {
    vec3 center;
    float radius;
    Material material;
};

constexpr Material glass = {1.5, {0.0, 0.9, 0.1, 0.8}, {0.6, 0.7, 0.8}, 125.};
constexpr Material mirror = {1.0, {0.0, 16.0, 0.8, 0.0}, {1.0, 1.0, 1.0}, 1425.};

#endif
