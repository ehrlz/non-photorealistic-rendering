/************************  GPO_01 ************************************
ATG, 2019
******************************************************************************/

#include <GpO.h>

// TAMA�O y TITULO INICIAL de la VENTANA
int ANCHO = 800, ALTO = 600;  // Tama�o inicial ventana
const char* prac = "OpenGL (GpO)";   // Nombre de la practica (aparecera en el titulo de la ventana).


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////     CODIGO SHADERS 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define GLSL(src) "#version 330 core\n" #src

const char* vertex_prog = GLSL(
layout(location = 0) in vec3 pos; 
layout(location = 1) in vec3 color;
out vec3 col;
uniform mat4 MVP=mat4(1.0f);
void main()
 {
  gl_Position = MVP*vec4(pos,1); // Construyo coord homog�neas y aplico matriz transformacion M
  col = color;                             // Paso color a fragment shader
 }
);

const char* fragment_prog = GLSL(
in vec3 col;
out vec3 outputColor;
void main() 
 {
	outputColor = col;
 }
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////   RENDER CODE AND DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLFWwindow* window;
GLuint prog;
objeto triangulo;

objeto crear_triangulo(void)
{
	objeto obj;
	GLuint VAO;
	GLuint buffer_pos, buffer_col;

	GLfloat pos_data[3][3] = { 0.0f,  0.0000f,  1.0f,  // Posici�n vertice 1
							   0.0f, -0.8660f, -0.5f,  // Posici�n vertice 2
							   0.0f,  0.8660f, -0.5f}; // Posici�n vertice 3

	GLfloat color_data[3][3] = { 1.0f, 0.0f, 0.0f,  // Color vertice 1
		                         0.0f, 1.0f, 0.0f,  // Color vertice 2 
								 0.0f, 0.0f, 1.0f }; // Color vertice 3

	// Mando posiciones en un VBO
	glGenBuffers(1, &buffer_pos); glBindBuffer(GL_ARRAY_BUFFER, buffer_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_data), pos_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Mando colores en otro VBO
	glGenBuffers(1, &buffer_col); glBindBuffer(GL_ARRAY_BUFFER, buffer_col);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_data), color_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// Creo y enlazo el VAO
	glGenVertexArrays(1, &VAO);	glBindVertexArray(VAO);

	// Indico donde hallar datos de posiciones dentro del VBO correspondiente
	glBindBuffer(GL_ARRAY_BUFFER, buffer_pos);
	glEnableVertexAttribArray(0);  // Organizaci�n de los datos del atributo 0 (pos) del vertex shade
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Indico donde hallar datos de colores dentro del VBO correspondiente
	glBindBuffer(GL_ARRAY_BUFFER, buffer_col);
	glEnableVertexAttribArray(1);  // Organizaci�n de los datos del atributo 0 (pos) del vertex shade
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);  //Cerramos VAO con todo listo para ser pintado

	obj.VAO = VAO; obj.Nv = 3;  // Devuelvo objeto VAO + n�mero de vertices en estructura obj

	return obj;

}


// Preparaci�n de los datos de los objetos a dibujar, envialarlos a la GPU
// Compilaci�n programas a ejecutar en la tarjeta gr�fica:  vertex shader, fragment shaders
// Opciones generales de render de OpenGL
void init_scene()
{
	int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height); 
    
	triangulo = crear_triangulo();  // Preparar datos de objeto, mandar a GPU

	// Mandar programas a GPU, compilar y crear programa en GPU

	// Compilear Shaders
	GLuint VertexShaderID = compilar_shader(vertex_prog, GL_VERTEX_SHADER);
	GLuint FragmentShaderID = compilar_shader(fragment_prog, GL_FRAGMENT_SHADER);

	// Enlazar sharders en el programa final
	prog = glCreateProgram();
	glAttachShader(prog, VertexShaderID);  glAttachShader(prog, FragmentShaderID);
	glLinkProgram(prog); check_errores_programa(prog);

	// Limpieza final de los shaders una vez compilado el programa
	glDetachShader(prog, VertexShaderID);  glDeleteShader(VertexShaderID);
	glDetachShader(prog, FragmentShaderID);  glDeleteShader(FragmentShaderID);

	// Alternativamente usar la funci�n Compile_Link_Shaders().
	//	prog = Compile_Link_Shaders(vertex_prog, fragment_prog); 

	glUseProgram(prog);    // Indicamos que programa vamos a usar 
}


vec3 pos_obs=vec3(10.0f,0.0f,0.0f); //###vec3 pos_obs=vec3(1.5f,0.0f,0.0f); 
vec3 target = vec3(0.0f,0.0f,0.0f);
vec3 up = vec3(0,0,1);

float fov = 35.0f, aspect = 4.0f / 3.0f; //###float fov = 40.0f, aspect = 4.0f / 3.0f;

// Actualizar escena: cambiar posici�n objetos, nuevos objetros, posici�n c�mara, luces, etc.
void render_scene()
{
	glClearColor(0.0f,0.0f,0.0f,1.0f);  // Especifica color para el fondo (RGB+alfa)
	glClear(GL_COLOR_BUFFER_BIT);          // Aplica color asignado borrando el buffer

	float t = (float)glfwGetTime();  // Contador de tiempo en segundos 


	///////// Actualizacion matrices M, V, P  /////////	
	mat4 P,V,M,T,R,S;

	P = perspective(glm::radians(fov), aspect, 0.5f, 20.0f);  //40� FOV,  4:3 ,  Znear=0.5, Zfar=20
	V = lookAt(pos_obs, target, up  );  // Pos camara, Lookat, head up
	
	//T = translate(0.0f, 0.0f, 3.0f*sin(t));  
	T = glm::translate(glm::vec3(0.0, 0.0, 3.0f*sin(t))); 
	
	M = T;
	transfer_mat4("MVP",P*V*M);
	
	// ORDEN de dibujar
	glBindVertexArray(triangulo.VAO);              // Activamos VAO asociado al objeto
    glDrawArrays(GL_TRIANGLES, 0, triangulo.Nv);   // Orden de dibujar (Nv vertices)	
	glBindVertexArray(0);                          // Desconectamos VAO

	////////////////////////////////////////////////////////

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
static void KeyCallback(GLFWwindow* window, int key, int code, int action, int mode)
{
	fprintf(stdout, "Key %d Code %d Act %d Mode %d\n", key, code, action, mode);
	if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
}


void asigna_funciones_callback(GLFWwindow* window)
{
	glfwSetWindowSizeCallback(window, ResizeCallback);
	glfwSetKeyCallback(window, KeyCallback);
}



