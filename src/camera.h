#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"

class camera {
  public:
    double aspect_ratio = 1.0; // Default Aspect ratio
    int image_width = 100; // Default image width (pixels)
  
    void render(const hittable& world) {
        initialise();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        // Render

        for (int j = 0; j < image_height; j++) {
            std::clog << "\nScanlines remaining: " << image_height-j << ' ' << std::flush;
            for (int i = 0; i < image_width; i++) {
                auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                auto ray_direction = pixel_center - center;
                ray r = ray(center, ray_direction);

                auto pixel_color = ray_color(r, world);
                write_color(std::cout, pixel_color);
            }
        }
        std::clog << "\rDone.                 \n";

    }

  private:
    int image_height; // Rendered image height
    point3 center; // Camera center
    point3 pixel00_loc; // location of the 0, 0 pixel.
    vec3 pixel_delta_u; // offset to pixel to the right
    vec3 pixel_delta_v; // offset to pixel below

    
    void initialise() {

        // Calculate the image height, and ensure that it's at least 1.
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

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

    color ray_color(const ray& r, const hittable& world) const {
        hit_record rec;

        if (world.hit(r, interval(0, infinity), rec)) {
            return (rec.normal + color(1,1,1)) / 2;
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = (unit_direction.y() + 1)/2;
        return (1-a) * color(1, 1, 1) + a * color(0.5, 0.7, 1.0);
    }

};

#endif