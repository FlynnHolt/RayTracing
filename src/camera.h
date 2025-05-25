#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"

class camera {
  public:
    double aspect_ratio = 1.0; // Default Aspect ratio
    int image_width = 100; // Default image width (pixels)
    int samples_per_pixel = 10; // Count of random samples for each pixel
    int max_depth = 10; // Max number of ray bounces.
  
    void render(const hittable& world) {
        initialise();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        // Render

        for (int j = 0; j < image_height; j++) {
            std::clog << "\nScanlines remaining: " << image_height-j << ' ' << std::flush;
            for (int i = 0; i < image_width; i++) {
                color pixel_color(0,0,0);

                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }

                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }
        std::clog << "\rDone.                 \n";

    }

  private:
    int image_height; // Rendered image height
    double pixel_samples_scale;
    point3 center; // Camera center
    point3 pixel00_loc; // location of the 0, 0 pixel.
    vec3 pixel_delta_u; // offset to pixel to the right
    vec3 pixel_delta_v; // offset to pixel below

    
    void initialise() {

        // Calculate the image height, and ensure that it's at least 1.
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        // Camera
        center = point3(0,0,0);
        auto focal_length = 1.0;

        // Viewport dimensions
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (double(image_width)/image_height);

        // Calculate the vectors accross the horizontal and down the vertical edges of the viewport
        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0);

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u/image_width;
        pixel_delta_v = viewport_v/image_height;

        // Calculate the location of the upper left pixel
        auto viewport_upper_left = center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + (pixel_delta_u + pixel_delta_v)/2;

    }

    ray get_ray(int i, int j) const {
        // Construct a camera ray originating at the camera and pointing at a randomly sampled
        // point around the pixel location (i, j).

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);
        
        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
        // Returns the vector to a random point in a unit square at the origin (xy plane).
        // from [-0.5, -0.5, 0] to [0.5, 0.5, 0]
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        // If the max depth is exceeding =, we don't gather any more light.
        if (depth <= 0) {
            return color(0, 0, 0);
        }

        hit_record rec;

        if (world.hit(r, interval(0.001, infinity), rec)) {
            // Diffuse Material:
            // 1. Ray bounces in random direction and returns 50% of the light.
            // 2. Lambertian reflection and 50% of the light
            vec3 new_direction = rec.normal + random_unit_vector(); //random_on_hemisphere(rec.normal);
            return 0.5 * ray_color(ray(rec.p, new_direction), depth-1, world);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = (unit_direction.y() + 1)/2;
        return (1-a) * color(1, 1, 1) + a * color(0.5, 0.7, 1.0);
    }

};

#endif