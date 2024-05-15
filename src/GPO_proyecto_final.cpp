/************************  GPO_01 ************************************
ATG, 2019
******************************************************************************/

#include <GpO.h>

// TAMA�O y TITULO INICIAL de la VENTANA
int ANCHO = 800, ALTO = 600;  // Tama�o inicial ventana
const char* prac = "OpenGL (GpO)";   // Nombre de la practica (aparecera en el titulo de la ventana).
GLuint posRegilla;
vec3 regilla=vec3(16, 16, 0);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////     CODIGO SHADERS 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define GLSL(src) "#version 330 core\n" #src

const char* vertex_prog = GLSL(
layout(location = 0) in vec3 pos; 
layout(location = 1) in vec2 uv;
out vec2 UV;
uniform mat4 MVP=mat4(1.0f); 
void main()
 {
  gl_Position = MVP*vec4(pos,1); // Construyo coord homog�neas y aplico matriz transformacion M
  UV=uv;                             // Paso color a fragment shader
 }
);

const char* fragment_prog_pix_it1 = GLSL(
in vec2 UV;
out vec3 outputColor;
uniform vec3 regilla=vec3(16, 16, 4);
uniform sampler2D unit;
uniform vec2 resolucion=vec2(800, 600);
void main(){
	float luz=1;

	int x = int(gl_FragCoord.x)%int(regilla.x);
	int y = int(gl_FragCoord.y)%int(regilla.y);
	if(x==0 || y==0){
		discard;
	}else{
		x = x>regilla.x/2 ? int(regilla.x)-x : x;
		y = y>regilla.y/2 ? int(regilla.y)-y : y;
		if(x<regilla.z || y<regilla.z){
			luz=min(x, y)/regilla.z;
		}
	}

	vec2 Pixels=resolucion/vec2(regilla.x, regilla.y);
	float dx = regilla.x * (1.0 / Pixels.x);
	float dy = regilla.y * (1.0 / Pixels.y);
	vec2 Coord = vec2(dx * floor(UV.x / dx),
						dy * floor(UV.y / dy));
	outputColor = texture(unit, Coord).rgb*luz;
 }
);

const char* fragment_prog_pix_it2 = GLSL(
in vec2 UV;
out vec3 outputColor;
uniform vec3 regilla=vec3(16, 16, 4);
uniform sampler2D unit;
uniform vec2 resolucion=vec2(800, 600);
void main(){
    vec2 p = UV * resolucion;

    p.x -= mod(p.x, regilla.x);
    p.y -= mod(p.y, regilla.y);
    
    outputColor = texture(unit, p / resolucion).rgb;
}
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////   RENDER CODE AND DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLFWwindow* window;
GLuint prog[2];
objeto spider;
GLuint textura;

void dibujar_indexado(objeto obj)
{
	// Activamos VAO asociado a obj y lo dibujamos con glDrawElements 
	glBindVertexArray(obj.VAO);
	glDrawElements(GL_TRIANGLES, obj.Ni, obj.tipo_indice, (void*)0);  // Dibujar (indexado)
	glBindVertexArray(0);  //Desactivamos VAO activo.
}


vec3 pos_obs = vec3(3.0f, 3.0f, 2.0f);
vec3 target = vec3(0.0f, 0.0f, 0.95f);
vec3 up = vec3(0, 0, 1);

mat4 PP, VV; // matrices de proyeccion y perspectiva
// Preparaci�n de los datos de los objetos a dibujar, envialarlos a la GPU
// Compilaci�n programas a ejecutar en la tarjeta gr�fica:  vertex shader, fragment shaders
// Opciones generales de render de OpenGL
void init_scene()
{
	int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height); 
    
	spider = cargar_modelo("../data/spider.bix");  // Preparar datos de objeto, mandar a GPU
	textura = cargar_textura("../data/spider.jpg", GL_TEXTURE0);

	PP = perspective(glm::radians(25.0f), 4.0f / 3.0f, 0.1f, 20.0f);  //40� Y-FOV,  4:3 ,  Znear=0.1, Zfar=20
	VV = lookAt(pos_obs, target, up);  // Pos camara, Lookat, head up

	// Mandar programas a GPU, compilar y crear programa en GPU

	// Compilear Shaders
	prog[0] = Compile_Link_Shaders(vertex_prog, fragment_prog_pix_it1); // Mandar programas a GPU, compilar y crear programa en GPU
	prog[1] = Compile_Link_Shaders(vertex_prog, fragment_prog_pix_it2); // Mandar programas a GPU, compilar y crear programa en GPU
    
    glEnable(GL_CULL_FACE); 
    glEnable(GL_DEPTH_TEST); 

    glUseProgram(prog[0]);    // Indicamos que programa vamos a usar

	posRegilla=glGetUniformLocation(prog[0], "regilla");
	glUseProgram(prog[0]);    // Indicamos que programa vamos a usar 

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

// float fov = 35.0f, aspect = 4.0f / 3.0f; //###float fov = 40.0f, aspect = 4.0f / 3.0f;

// Actualizar escena: cambiar posici�n objetos, nuevos objetros, posici�n c�mara, luces, etc.
void render_scene()
{
	glClearColor(0.0f,0.0f,0.0f,0.0f);  // Especifica color para el fondo (RGB+alfa)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);          // Aplica color asignado borrando el buffer

	float t = (float)glfwGetTime();  // Contador de tiempo en segundos 

	///////// Aqui vendr�a nuestr c�digo para actualizar escena  /////////	
	mat4 M, T, R, S;
	R=rotate(t, vec3(0, 0, 1.f));  
	T=translate(vec3(0.f, 0.f, 0.f));
	M = T*R;

	transfer_mat4("MVP", PP*VV*M); 
	glUniform3fv(posRegilla, 1, &regilla[0]);
	
	// ORDEN de dibujar
	dibujar_indexado(spider);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROGRAMA PRINCIPAL
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	init_GLFW();            // Inicializa lib GLFW
	window = Init_Window(prac);  // Crea ventana usando GLFW, asociada a un contexto OpenGL	X.Y
	load_Opengl();         // Carga funciones de OpenGL, comprueba versi�n.
	init_scene();          // Prepara escena
	
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window))
	{
		render_scene();
		glfwSwapBuffers(window);
		glfwPollEvents();
		show_info();
	}

	glfwTerminate();
	exit(EXIT_SUCCESS);
}


//////////  FUNCION PARA MOSTRAR INFO OPCIONAL EN EL TITULO DE VENTANA  //////////
void show_info()
{
	static int fps = 0;
	static double last_tt = 0;
	double elapsed, tt;
	char nombre_ventana[128];   // buffer para modificar titulo de la ventana

	fps++; tt = glfwGetTime();  // Contador de tiempo en segundos 

	elapsed = (tt - last_tt);
	if (elapsed >= 0.5)  // Refrescar cada 0.5 segundo
	{
		sprintf_s(nombre_ventana, 128, "%s: %4.0f FPS @ %d x %d", prac, fps / elapsed, ANCHO, ALTO);
		glfwSetWindowTitle(window, nombre_ventana);
		last_tt = tt; fps = 0;
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////  ASIGNACON FUNCIONES CALLBACK
///////////////////////////////////////////////////////////////////////////////////////////////////////////


// Callback de cambio tama�o de ventana
void ResizeCallback(GLFWwindow* window, int width, int height)
{
	glfwGetFramebufferSize(window, &width, &height); 
	glViewport(0, 0, width, height);
	ALTO = height;	ANCHO = width;
}

// Callback de pulsacion de tecla
float z=4.0f;
static void KeyCallback(GLFWwindow* window, int key, int code, int action, int mode)
{
	fprintf(stdout, "Key %d Code %d Act %d Mode %d\n", key, code, action, mode);
	if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);

	if(action!=GLFW_RELEASE){
		switch(key){
			case GLFW_KEY_UP: regilla.y+=0.1; break;
			case GLFW_KEY_DOWN: regilla.y-=0.1; break;
			case GLFW_KEY_LEFT: regilla.x-=0.1; break;
			case GLFW_KEY_RIGHT: regilla.x+=0.1; break;
			case GLFW_KEY_KP_ADD: regilla.z+=0.1; break;
			case GLFW_KEY_KP_SUBTRACT: regilla.z-=0.1; break;
			case GLFW_KEY_0: 
				glUseProgram(prog[0]); 
				posRegilla=glGetUniformLocation(prog[0], "regilla");
				break;
			case GLFW_KEY_1: 
				glUseProgram(prog[1]); 
				posRegilla=glGetUniformLocation(prog[1], "regilla");
				break;
			case GLFW_KEY_TAB: float aux=z; z=regilla.z; regilla.z=aux; break;
		}
	}
}


void asigna_funciones_callback(GLFWwindow* window)
{
	glfwSetWindowSizeCallback(window, ResizeCallback);
	glfwSetKeyCallback(window, KeyCallback);
}



