#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "ray.h"

class Camera {
public:
    Camera(int image_width, int image_height,
           const point3& position = point3(0, 0, 0),
           double focal_length = 1.0);

    [[nodiscard]] ray get_ray(int pixel_x, int pixel_y) const;
    void move(const vec3& offset);
    void rotate(float delta_yaw, float delta_pitch);
    void set_position(const point3& position);

    [[nodiscard]] point3 get_position() const { return camera_center_; }
    [[nodiscard]] vec3 get_forward() const { return front_; }
    [[nodiscard]] vec3 get_right() const { return camera_right_; }
    [[nodiscard]] vec3 get_up() const { return camera_up_; }
    [[nodiscard]] float get_yaw() const { return yaw_; }
    [[nodiscard]] float get_pitch() const { return pitch_; }

private:
    /**
     * @brief Update all camera vectors and viewport calculations
     *
     * This method recalculates the camera's coordinate system and viewport
     * parameters whenever the camera position or orientation changes.
     */
    void update_camera_vectors();

    // Image dimensions
    int image_width_;
    int image_height_;

    // Camera parameters
    point3 camera_center_;
    double focal_length_;
    float yaw_;
    float pitch_;

    // Camera coordinate system
    vec3 front_;
    vec3 camera_right_;
    vec3 camera_up_;
    vec3 world_up_;

    // Viewport parameters
    double viewport_height_;
    double viewport_width_{};
    vec3 viewport_u_;
    vec3 viewport_v_;
    vec3 pixel_delta_u_;
    vec3 pixel_delta_v_;
    point3 viewport_upper_left_;
    point3 pixel00_loc_;
};

#endif // CAMERA_H
