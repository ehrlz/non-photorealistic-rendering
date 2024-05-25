#define minGW

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


//###include <glad\glad.h> 
#include <glad/glad.h> 

//###include <glfw\glfw3.h>
#include <GLFW/glfw3.h>


// GLM LIB 
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp> 
#include <glm/gtc/matrix_transform.hpp>

// Version OpenGL usada
#define OPENGL_MAJOR 4
#define OPENGL_MINOR 0


#ifdef minGW
  #define fopen_s(fp,fmt,mode)  *(fp)=fopen((fmt),(mode))
  #define sprintf_s snprintf
#endif

#ifndef minGW
 #define M_PI 3.14159265359f
#endif
  

using namespace glm;


// DEclaraciones  de tipos
typedef struct {GLuint VAO; GLuint Ni; GLuint Nv; GLuint Nt; GLuint tipo_indice; } objeto;


/// DECLARACIONES DE FUNCIONES AUXILIARES

void init_scene(void);
void show_info();

void vuelca_mat4(mat4 M);

int check_errores_programa(GLuint);
GLuint compilar_shader(const char*, GLuint);
GLuint link_programa(GLuint, GLuint);
GLuint Compile_Link_Shaders(const char*, const char*);
char* leer_codigo_de_fichero(const char*);


GLuint cargar_cube_map(const char *, GLuint);
GLuint cargar_textura(const char *,GLuint, bool);
objeto cargar_modelo(char*);
objeto cargar_modelo_obj(const char*);

void transfer_mat4(const char*, mat4);
void transfer_mat3(const char*, mat3);
void transfer_vec4(const char*, vec4);
void transfer_vec3(const char*, vec3);
void transfer_vec2(const char*, vec2);
void transfer_int(const char*, GLuint);
void transfer_float(const char*, GLfloat);


/*
//// Funciones asociadas a eventos
void framebuffer_size_callback(GLFWwindow*, int, int);
static void ScrollCallback(GLFWwindow*, double, double);
static void KeyCallback(GLFWwindow*, int, int, int, int);
static void CursorPosCallback(GLFWwindow*, double, double);
static void MouseCallback(GLFWwindow*, int, int, int);
*/


// FUNCIONES DE INICIALIZACION de OPENGL y VENTANAS
void asigna_funciones_callback(GLFWwindow*);
void init_GLFW(void);
void load_Opengl(void);
GLFWwindow*  Init_Window(const char*);

void setupImGui(GLFWwindow* window);
void renderImGui(int *scene_flag, int *model_flag, int *render_texture, int *color_levels, float *toon_border,
            vec3 *model_color, float *b_lightness, float *y_lightness, float *alpha, float *beta, vec4 *light_coefs);
void terminateImGui(void);

// CONTENT
enum SCENES {PIXEL1, PIXEL2, TOON, PHONG, BLINN, GOOCH};
enum MODELS {SPIDER, BALL, HELMET, CAT};