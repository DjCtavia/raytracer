#include "camera.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline float radians(float degrees) {
    return degrees * static_cast<float>(M_PI) / 180.0f;
}

Camera::Camera(int image_width, int image_height, const point3& position, double focal_length)
    : image_width_(image_width)
    , image_height_(image_height)
    , camera_center_(position)
    , focal_length_(focal_length)
    , yaw_(-90.0f)  // Start looking down negative Z axis
    , pitch_(0.0f)
    , front_(0, 0, -1)
    , world_up_(0, 1, 0)
    , viewport_height_(2.0)
{
    update_camera_vectors();
}

ray Camera::get_ray(int pixel_x, int pixel_y) const {
    auto pixel_center = pixel00_loc_ + (pixel_x * pixel_delta_u_) + (pixel_y * pixel_delta_v_);
    auto ray_direction = pixel_center - camera_center_;
    return ray(camera_center_, ray_direction);
}

void Camera::move(const vec3& offset) {
    camera_center_ += offset;
    update_camera_vectors();
}

void Camera::rotate(float delta_yaw, float delta_pitch) {
    yaw_ += delta_yaw;
    pitch_ += delta_pitch;

    // Constrain pitch to avoid gimbal lock
    if (pitch_ > 89.0f) pitch_ = 89.0f;
    if (pitch_ < -89.0f) pitch_ = -89.0f;

    update_camera_vectors();
}

void Camera::set_position(const point3& position) {
    camera_center_ = position;
}

void Camera::update_camera_vectors() {
    // Calculate new front vector from yaw and pitch
    float yaw_rad = radians(yaw_);
    float pitch_rad = radians(pitch_);

    front_.e[0] = cosf(yaw_rad) * cosf(pitch_rad);
    front_.e[1] = sinf(pitch_rad);
    front_.e[2] = sinf(yaw_rad) * cosf(pitch_rad);
    front_ = unit_vector(front_);

    // Calculate right and up vectors
    camera_right_ = unit_vector(cross(front_, world_up_));
    camera_up_ = unit_vector(cross(camera_right_, front_));

    // Update viewport parameters
    viewport_width_ = viewport_height_ * (static_cast<double>(image_width_) / image_height_);

    // Calculate viewport edge vectors
    viewport_u_ = viewport_width_ * camera_right_;
    viewport_v_ = viewport_height_ * -camera_up_;

    // Calculate pixel delta vectors
    pixel_delta_u_ = viewport_u_ / image_width_;
    pixel_delta_v_ = viewport_v_ / image_height_;

    // Calculate viewport center and upper-left corner
    auto viewport_center = camera_center_ + front_ * focal_length_;
    viewport_upper_left_ = viewport_center - viewport_u_ / 2 - viewport_v_ / 2;
    pixel00_loc_ = viewport_upper_left_ + 0.5 * (pixel_delta_u_ + pixel_delta_v_);
}
