#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
    auto a = r.direction().length_squared(); // O ⋅ O or squared length
    auto b = 2 * dot(originSphere, r.direction()); // 2(O ⋅ B)
    auto c = originSphere.length_squared() - radius * radius; // O ⋅ O (or squared length) - r²
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

// Shaders for quad display
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
void main() {
    TexCoord = aTexCoord;
    gl_Position = vec4(aPos, 0.0, 1.0);
})";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D screenTexture;
void main() {
    FragColor = texture(screenTexture, TexCoord);
})";

// Utility function to compile a shader
GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
    }
    return shader;
}

int main() {
    // GLFW initialization
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int image_width = 1280; // Example: 1280x720 (HD, < 1920x1080)
    int image_height = 720;

    // Create GLFW window in 16/9
    GLFWwindow* window = glfwCreateWindow(image_width, image_height, "RayTracer", nullptr, nullptr);
    if (!window) {
        std::cerr << "GLFW window creation failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync

    // GLAD initialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD initialization failed" << std::endl;
        return -1;
    }

    // ImGui initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Dynamic image variables
    std::vector<unsigned char> framebuffer(image_width * image_height * 3);

    // Initialize Camera
    Camera camera(image_width, image_height, point3(0, 0, 0), 1.0);
    float camera_speed = 1.0f;

    // Helper function to render the scene
    auto render_scene = [&]() {
        for (int j = 0; j < image_height; j++) {
            for (int i = 0; i < image_width; i++) {
                ray r = camera.get_ray(i, j);
                color pixel_color = ray_color(r);
                int idx = (j * image_width + i) * 3;
                framebuffer[idx + 0] = static_cast<unsigned char>(256 * std::clamp(pixel_color.x(), 0.0, 0.999));
                framebuffer[idx + 1] = static_cast<unsigned char>(256 * std::clamp(pixel_color.y(), 0.0, 0.999));
                framebuffer[idx + 2] = static_cast<unsigned char>(256 * std::clamp(pixel_color.z(), 0.0, 0.999));
            }
        }
    };

    // Initial render
    render_scene();

    // OpenGL texture creation
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, framebuffer.data());

    // Shader program creation for fullscreen quad
    GLuint vertShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    // Fullscreen quad (2 triangles)
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    double lastFrameTime = glfwGetTime();
    double delta_time = 0.0f;

    bool rightMousePressed = false;
    double lastMouseX = 0.0, lastMouseY = 0.0;
    float sensitivity = 0.2f;
    bool invertY = true;

    while (!glfwWindowShouldClose(window)) {
        double currentFrameTime = glfwGetTime();
        delta_time = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glfwPollEvents();

        bool camera_updated = false;

        // Mouse look controls
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            if (!rightMousePressed) {
                rightMousePressed = true;
                glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else {
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                double offsetX = mouseX - lastMouseX;
                double offsetY = mouseY - lastMouseY;
                lastMouseX = mouseX;
                lastMouseY = mouseY;

                float delta_yaw = static_cast<float>(offsetX) * sensitivity;
                float delta_pitch = static_cast<float>(invertY ? offsetY : -offsetY) * sensitivity;
                camera.rotate(delta_yaw, delta_pitch);
                camera_updated = true;
            }
        } else {
            if (rightMousePressed) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            rightMousePressed = false;
        }

        // Keyboard movement controls
        vec3 move_offset(0, 0, 0);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            move_offset -= camera.get_right() * camera_speed * static_cast<float>(delta_time);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            move_offset += camera.get_right() * camera_speed * static_cast<float>(delta_time);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            move_offset += camera.get_forward() * camera_speed * static_cast<float>(delta_time);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            move_offset -= camera.get_forward() * camera_speed * static_cast<float>(delta_time);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            move_offset -= camera.get_up() * camera_speed * static_cast<float>(delta_time);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            move_offset += camera.get_up() * camera_speed * static_cast<float>(delta_time);
        }

        if (move_offset.length_squared() > 0) {
            camera.move(move_offset);
            camera_updated = true;
        }

        // Re-render if camera changed
        if (camera_updated) {
            render_scene();
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width, image_height, GL_RGB, GL_UNSIGNED_BYTE, framebuffer.data());
        }

        // ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static bool reload = false;
        ImGui::Begin("Raytracer Output");
        if (ImGui::Button("Reload")) reload = true;
        ImGui::Text("delta_time: %.4f s", delta_time);
        ImGui::Text("camera: [%.2f, %.2f, %.2f]", camera.get_position().x(), camera.get_position().y(), camera.get_position().z());
        ImGui::Text("yaw: %.1f°, pitch: %.1f°", camera.get_yaw(), camera.get_pitch());
        ImGui::Separator();
        ImGui::Text("Camera:");
        ImGui::SliderFloat("Camera Speed", &camera_speed, 0.1f, 2.0f);
        ImGui::Text("Mouse Controls:");
        ImGui::SliderFloat("Sensitivity", &sensitivity, 0.1f, 2.0f);
        ImGui::Checkbox("Invert Y-axis", &invertY);
        ImGui::End();

        // If reload requested, recalculate image and reload texture
        if (reload) {
            render_scene();
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width, image_height, GL_RGB, GL_UNSIGNED_BYTE, framebuffer.data());
            reload = false;
        }

        // OpenGL rendering of full-screen quad
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(glGetUniformLocation(shaderProgram, "screenTexture"), 0);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glUseProgram(0);

        // ImGui render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    glDeleteTextures(1, &tex);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
