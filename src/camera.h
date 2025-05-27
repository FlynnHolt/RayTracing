#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"

class camera {
  public:
    double aspect_ratio = 1.0; // Default Aspect ratio
    int image_width = 100; // Default image width (pixels)
    int samples_per_pixel = 10; // Count of random samples for each pixel
    int max_depth = 10; // Max number of ray bounces.

    double vfov = 90; // Vertical field of view (degrees)
    point3 lookfrom = point3(0,0,0); // Where the camera is
    point3 lookat = point3(0,0,-1); // Where it is looking
    vec3 vup = vec3(0,1,0); // The up direction

    double defocus_angle = 0;
    double focus_dist = 10;
  
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
    vec3 u, v, w; // Camera position basis vectors (orthonormal).
    vec3 defocus_disk_u; // Sizes of defocus disk
    vec3 defocus_disk_v;
    
    void initialise() {

        // Calculate the image height, and ensure that it's at least 1.
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        // Camera
        center = lookfrom;

        // Viewport dimensions
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2.0);
        auto viewport_height = 2.0 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image_width)/image_height);

        // Calculate the u, v, w basis vectors for the camera frame
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors accross the horizontal and down the vertical edges of the viewport
        auto viewport_u = viewport_width * u;
        auto viewport_v = viewport_height * -v;

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u/image_width;
        pixel_delta_v = viewport_v/image_height;

        // Calculate the location of the upper left pixel
        auto viewport_upper_left = center - focus_dist*w - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + (pixel_delta_u + pixel_delta_v)/2;

        // Calculate Camera defocus disk radius
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = defocus_radius * u;
        defocus_disk_v = defocus_radius * v;

    }

    ray get_ray(int i, int j) const {
        // Construct a camera ray originating at the defocus disk and pointing at a randomly sampled
        // point around the pixel location (i, j).

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);
        
        auto ray_origin = (defocus_angle <= 0) ? center : sample_defocus_disk();
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    point3 sample_defocus_disk() const {
        // Returns a random point in the defocus disk
        auto p = random_in_unit_disk();
        return center + (p.x() * defocus_disk_u) + (p.y() * defocus_disk_v);
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
            ray scattered;
            color attenuation;
            
            if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                return attenuation * ray_color(scattered, depth-1, world);
            }
            return color(0, 0, 0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = (unit_direction.y() + 1)/2;
        return (1-a) * color(1, 1, 1) + a * color(0.5, 0.7, 1.0);
    }

};

#endif