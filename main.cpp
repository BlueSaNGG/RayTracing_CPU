#include "util.h"
#include "scene.h"

using namespace std;

constexpr int width = 1200;
constexpr int height = 800;
constexpr float fov = 1.0472; // Radians

vec3 reflect(const vec3 &ray, const vec3 &normal) {
    return ray - normal * 2.f * (ray * normal);
}

vec3 refract(const vec3 &ray, const vec3 &normal, const float eta_normal, const float eta_ray = 1.f) {
    // The eta of air is 1, so eta_ray default value is 1
    float cos_ray = -std::max(-1.f, std::min(1.f, ray * normal));

    if (cos_ray < 0)
        return refract(ray, -normal, eta_ray, eta_normal);

    float k = 1 - pow(eta_ray / eta_normal, 2) * (1 - pow(cos_ray, 2));

    if (k < 0)
        return vec3{1, 0, 0};

    float eta_div = eta_ray / eta_normal;
    return ray * eta_div + normal * (eta_div * cos_ray - sqrt(k));
}

std::tuple<bool, float> ray_sphere_intersect(const vec3 &orig, const vec3 &dir, const Sphere &s) {
    vec3 L = s.center - orig;
    float tca = L * dir;
    float d2 = L * L - tca * tca;

    if (d2 > s.radius * s.radius)
        return {false, 0};

    float thc = std::sqrt(s.radius * s.radius - d2);
    float t0 = tca - thc, t1 = tca + thc;

    if (t0 > .001)
        return {true, t0};

    if (t1 > .001)
        return {true, t1};

    return {false, 0};
}

std::tuple<bool, vec3, vec3, Material> scene_intersect(const vec3 &orig, const vec3 &dir) {
    vec3 pt, N;
    Material material;

    float nearest_dist = 1e10;
    if (std::abs(dir.y) > .001) {
        float d = -(orig.y + 4) / dir.y;
        vec3 p = orig + dir * d;
        if (d > .001 && d < nearest_dist && std::abs(p.x) < 100 && p.z < 50 && p.z > -100) {
            nearest_dist = d;
            pt = p;
            N = {0, 1, 0};
            material.diffuse = (int(.5 * pt.x + 1000) + int(.5 * pt.z)) & 1 ? vec3{.3, .3, .3} : vec3{.3, .2, .1};
        }
    }

    for (const Sphere &s: spheres) { // intersect the ray with all spheres
        auto[intersection, d] = ray_sphere_intersect(orig, dir, s);
        if (!intersection || d > nearest_dist) continue;
        nearest_dist = d;
        pt = orig + dir * nearest_dist;
        N = (pt - s.center).normalized();
        material = s.material;
    }
    return {nearest_dist < 1000, pt, N, material};
}

vec3 cast_ray(const vec3 &orig, const vec3 &dir, const int depth = 0) {
    auto[hit, point, N, material] = scene_intersect(orig, dir);
    if (depth > 4 || !hit)
        return {0.2, 0.7, 0.8};

    vec3 reflect_dir = reflect(dir, N).normalized();
    vec3 refract_dir = refract(dir, N, material.refraction).normalized();
    vec3 reflect_color = cast_ray(point, reflect_dir, depth + 1);
    vec3 refract_color = cast_ray(point, refract_dir, depth + 1);

    float diffuse_light_intensity = 0, specular_light_intensity = 0;
    for (const vec3 &light: lights) { // checking if the point lies in the shadow of the light
        vec3 light_dir = (light - point).normalized();
        auto[hit, shadow_pt, trashnrm, trashmat] = scene_intersect(point, light_dir);
        if (hit && (shadow_pt - point).norm() < (light - point).norm()) continue;
        diffuse_light_intensity += std::max(0.f, light_dir * N);
        specular_light_intensity += std::pow(std::max(0.f, -reflect(-light_dir, N) * dir), material.specular);
    }
    return material.diffuse * diffuse_light_intensity * material.albedo[0] +
           vec3{1., 1., 1.} * specular_light_intensity * material.albedo[1] + reflect_color * material.albedo[2] +
           refract_color * material.albedo[3];
}

int main() {
    gen_scene(20, 3);
    std::vector<vec3> framebuffer(width * height);

#pragma omp parallel for
    for (int pix = 0; pix < width * height; pix++) {
        float dir_x = (pix % width + 0.5) - width / 2.;
        float dir_y = -(pix / width + 0.5) + height / 2.;
        float dir_z = -height / (2. * tan(fov / 2.));
        framebuffer[pix] = cast_ray(vec3{0, 0, 0}, vec3{dir_x, dir_y, dir_z}.normalized());
    }

    std::ofstream ofs("./out.ppm", std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (vec3 &color: framebuffer) {
        float max = std::max(1.f, std::max(color[0], std::max(color[1], color[2])));
        for (int chan: {0, 1, 2})
            ofs << (char) (255 * color[chan] / max);
    }

    return 0;
}
