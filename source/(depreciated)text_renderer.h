#include "../glad.h"
#define GLFW_INCLUDE_NONE
#include "../glfw3.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "../stb_truetype.h"

stbtt_fontinfo font_info;

GLuint VAO;
GLuint program_text;

static const char *font_path = "../assests/consola.ttf";
static const char *vert_text_path = "../shaders/text_vert.glsl";
static const char *frag_text_path = "../shaders/text_frag.glsl";
