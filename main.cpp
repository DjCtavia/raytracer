#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <iostream>

/**
 * ----- Sphere -----
 * center of Sphere: C(x0, y0, z0)
 * radius: r²
 * Notation: (x-x0)² + (y-y0)² + (z-z0)² = r²
 * Vector notation: ∥ P - C ∥² = r²
 *
 * (P is any point, C is center of sphere)
 *
 * ----- Ray -----
 * formula: P(t) = A + tB
 *
 * (A is origin of ray, B is direction of ray)
 *
 * So the equation of Sphere could be written as: ∥P(t) - C∥² = r²
 * And rewritten: ∥ O + tB∥² = r² where O = A - C
 *
 * We can now develop it as:
 * = (O + tB) ⋅ (O + tB) = r²
 * = (O ⋅ O) + 2t(O ⋅ B) + t²(B ⋅ B) = 0
 *
 * if the result of above calculation is >= to 0 we assume it's ok.
 * but we can actually improve it using for getting the value of 't' that represents the distance (or “time”) it takes to travel along the radius to reach the sphere.
 * if the discriminant >= 0 then:
 * t = (-b - sqrt(discriminant)) / 2a
 *
 * @param center
 * @param radius
 * @param r
 * @return
 */
double hit_sphere(const point3& center, double radius, const ray& r)
{
    vec3 originSphere = r.origin() - center; // O = A - C
    auto a = dot(r.direction(), r.direction()); // O ⋅ O
    auto b = 2 * dot(originSphere, r.direction()); // 2(O ⋅ B)
    auto c = dot(originSphere, originSphere) - radius * radius; // O ⋅ O - r²
    auto discriminant = b * b - 4 * a * c; // at² + bt + c = 0

    return discriminant < 0
               ? -1.0
               : (-b - sqrt(discriminant)) / (2.0 * a);
}

color ray_color(const ray& r)
{
    auto t = hit_sphere(point3(0, 0, -1), 0.5, r);
    if (t > 0.0)
    {
        vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1));
        return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);

    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main()
{
    // Image
    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int image_height = std::max(1, int(image_width / aspect_ratio));

    // Camera
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width) / image_height);
    auto camera_center = point3(0, 0, 0);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Render
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++)
    {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; i++)
        {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);

            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }

    std::clog << "\rDone.                 \n";

    return 0;
}
