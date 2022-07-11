#include "camera.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "components/spatial.hpp"

static const GLchar blit_vertex_shader[] = {
#include "shaders/camera.vert.cstr"
};
static const GLchar blit_fragment_shader[] = {
#include "shaders/camera.frag.cstr"
};

static const GLfloat blit_vertices[] = {
    -1.f, -1.f,
     1.f, -1.f,
     1.f,  1.f,
    -1.f, -1.f,
     1.f,  1.f,
    -1.f,  1.f,
};

CameraSystem::CameraSystem(entt::registry &registry): registry(registry) {
    registry.on_construct<Camera>().connect<&entt::registry::emplace_or_replace<Spatial>>();
}

CameraSystem::~CameraSystem() {
    if (init_gl) {
        glDeleteFramebuffers(1, &fbo);
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteProgram(program);
    }
    if (init_textures) {
        glDeleteTextures(1, &color_texture);
        glDeleteTextures(1, &depth_texture);
    }
}

void CameraSystem::set_active(entt::entity entity) {
    registry.emplace_or_replace<Camera>(entity);
    active_camera = entity;
}

void CameraSystem::set_viewport(int new_width, int new_height) {
    if (init_textures && (new_width != width || new_height != height)) {
        glDeleteTextures(1, &color_texture);
        glDeleteTextures(1, &depth_texture);
        init_textures = false;
    }
    width = new_width;
    height = new_height;
}

entt::entity CameraSystem::get_active() {
    return active_camera;
}

Camera CameraSystem::get_camera() {
    return registry.get<Camera>(active_camera);
}

glm::vec3 CameraSystem::get_position() {
    auto spatial = registry.get<Spatial>(active_camera);
    return glm::vec3(spatial.transform[3]);
}

glm::mat4 CameraSystem::get_projection() {
    auto camera = registry.get<Camera>(active_camera);
    return glm::perspective(glm::radians(camera.fov), (float)width / height, camera.near, camera.far);
}

glm::mat4 CameraSystem::get_view() {
    auto spatial = registry.get<Spatial>(active_camera);
    return get_projection() * glm::inverse(spatial.transform);
}

void CameraSystem::render(int screen_width, int screen_height) {
    set_viewport(screen_width * render_scale, screen_height * render_scale);

    if (!init_gl) {
        glGenFramebuffers(1, &fbo);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(blit_vertices), blit_vertices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        ShaderCompiler compiler;
        compiler.compileStage(GL_VERTEX_SHADER, blit_vertex_shader);
        compiler.compileStage(GL_FRAGMENT_SHADER, blit_fragment_shader);
        program = compiler.create();

        glUseProgram(program);
        loc_position = glGetAttribLocation(program, "position");
        loc_tex = glGetUniformLocation(program, "tex");
        glEnableVertexAttribArray(loc_position);
        glVertexAttribPointer(loc_position, 2, GL_FLOAT, GL_FALSE, 0, 0);

        init_gl = true;
    }

    if (!init_textures) {
        glGenTextures(1, &color_texture);
        glBindTexture(GL_TEXTURE_2D, color_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glGenTextures(1, &depth_texture);
        glBindTexture(GL_TEXTURE_2D, depth_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

        init_textures = true;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    for (auto callback : on_render) {
        callback(width, height);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST);
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_texture);
    glBindVertexArray(vao);
    glUniform1i(loc_tex, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}
