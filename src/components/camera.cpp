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
}

CameraSystem::~CameraSystem() {
    if (fbo) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
    }
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (program) glDeleteProgram(program);
    if (color_texture) glDeleteTextures(1, &color_texture);
    if (depth_texture) glDeleteTextures(1, &depth_texture);
}

void CameraSystem::set_active(entt::entity entity) {
    active_camera = entity;
}

entt::entity CameraSystem::get_active() {
    return active_camera;
}

void CameraSystem::render(int screen_width, int screen_height) {
    if (screen_width != width || screen_height != height || !color_texture || !depth_texture) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

        if (color_texture) glDeleteTextures(1, &color_texture);
        glGenTextures(1, &color_texture);
        glBindTexture(GL_TEXTURE_2D, color_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        if (depth_texture) glDeleteTextures(1, &depth_texture);
        glGenTextures(1, &depth_texture);
        glBindTexture(GL_TEXTURE_2D, depth_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, screen_width, screen_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

        width = screen_width;
        height = screen_height;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto camera = registry.get<Camera>(active_camera);
    auto spatial = registry.get<Spatial>(active_camera);
    auto projection = glm::perspective(glm::radians(camera.fov), (float)width / height, camera.near, camera.far);
    auto view = projection * glm::inverse(spatial.transform);

    glEnable(GL_DEPTH_TEST);
    for (auto &callback : on_render) {
        callback(view);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screen_width, screen_height);

    glDisable(GL_DEPTH_TEST);
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_texture);
    glBindVertexArray(vao);
    glUniform1i(loc_tex, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}
