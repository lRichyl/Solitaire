#ifndef RENDERER_H
#define RENDERER_H
#include "glad/glad.h"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "window.h"
#include "math.h"

struct RendererInfo{
     static const int QUADS_PER_BATCH = 1000;
     static const int FLOATS_PER_QUAD = 36;
     static const int INDICES_PER_QUAD = 6;
     static const int TOTAL_INDICES =  QUADS_PER_BATCH * INDICES_PER_QUAD;
     static int MAX_TEXTURE_UNITS_PER_BATCH;
     static const int NUMBER_OF_BATCHES = 5;
};



struct ShaderProgram{
     unsigned int id;
     const char *name;
};
struct Batch{
     unsigned int vbo;
     unsigned int vao;
     unsigned int texture_index = 0;
     unsigned int vertices_index = 0;
     int number_of_quads_to_copy = 0;
     int total_indices_to_draw = 0;
     unsigned int registered_textures_ids[120] = {};// This is just a hardcoded random limit
     ShaderProgram shader_program;
     // float vertex_buffer[16] = {-0.8f, 0.8f,
     //                          -0.8f, 0.0f,
     //                           0.8f, 0.0f,
     //                           0.8f, 0.8f};
     float vertex_buffer[RendererInfo::QUADS_PER_BATCH * RendererInfo::FLOATS_PER_QUAD] = {};
};
struct Renderer{
     unsigned int ibo;
     unsigned int fbo;
     unsigned int framebuffer_ibo;
     unsigned int framebuffer_vao;
     unsigned int framebuffer_vbo;
     unsigned int framebuffer_rbo;
     unsigned int framebuffer_texture;
     V2 drawing_resolution = {800, 600};
     ShaderProgram framebuffer_shader_program;
     Window *window;
     unsigned int framebuffer_index_buffer[6] = {0,1,2,2,3,0};
     float framebuffer_vertex_buffer[16] = {-1.0f, 1.0f, 0.0f, 1.0f,
                                         -1.0f, -1.0f, 0.0f, 0.0f,
                                          1.0f, -1.0f, 1.0f, 0.0f,
                                          1.0f, 1.0f, 1.0f, 1.0f};
     glm::mat4 projection;
     //Use this matrix later to move the camera.
     glm::mat4 view;
     unsigned int index_buffer[RendererInfo::TOTAL_INDICES] = {};
     int batch_index = 0;
     Batch main_batch;
     Batch batches[RendererInfo::NUMBER_OF_BATCHES];
     Batch ui_batch;
     Batch *current_batch;
     ShaderProgram default_shader_program;
     ShaderProgram current_shader;
};



struct Texture{
     unsigned int id;
     int width;
     int height;
     int channels;
     unsigned char *data_buffer;
};

struct Rect{
     float x;
     float y;
     float w;
     float h;
};

Renderer* create_renderer(Window *window);
void compile_shader_program(ShaderProgram *shader_program, const char *vs_path, const char *fs_path);
void initialize_texture_sampler(ShaderProgram shader);
void load_mvp_to_shader(Renderer *renderer, ShaderProgram shader);
void render_quad(Renderer *renderer, Rect *position, Texture *texture, Rect *clip_region = NULL, bool mirror = false, float alpha_value = 255, V3 color = {1.0f,1.0f,1.0f}, bool mirrorY = false);
void render_colored_rect(Renderer *renderer, Rect *position, V3 color, float alpha_value = 1);
void render_quad_to_ui(Renderer *renderer, Rect *position, Texture *texture, Rect *clip_region = NULL, bool mirror = false, float alpha_value = 255, V3 color = {1.0f,1.0f,1.0f}, bool mirrorY = false);
void render_quad_with_shader(Renderer *renderer, Rect *position, Texture *texture, ShaderProgram shader,Rect *clip_region = NULL , bool mirrorX = false, float alpha_value = 255, V3 color = {1.0f,1.0f,1.0f}, bool mirrorY = false);
void change_drawing_resolution(Renderer *renderer, int width, int height);
Texture make_texture(const char *path);
void renderer_draw(Renderer *renderer);
void destroy_renderer(Renderer *renderer);
#endif
