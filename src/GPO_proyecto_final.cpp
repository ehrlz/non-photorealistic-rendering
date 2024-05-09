/************************  GPO_01 ************************************
ATG, 2019
******************************************************************************/

#include <GpO.h>
#include <vector>

using namespace std;

// TAMA�O y TITULO INICIAL de la VENTANA
int ANCHO = 800, ALTO = 600;  // Tama�o inicial ventana
const char* prac = "OpenGL (GpO)";   // Nombre de la practica (aparecera en el titulo de la ventana).

GLuint prog[2];

GLuint posRegilla;
vec3 regilla=vec3(16, 16, 0);

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

const char* fragment_prog_base = GLSL(
in vec3 col;
out vec3 outputColor;
void main() 
 {
	outputColor = col;
 }
);

const char* fragment_prog = GLSL(
in vec3 col;
out vec3 outputColor;
uniform vec3 regilla=vec3(16, 16, 4);
void main() 
 {
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

	outputColor = col*luz;
 }
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////   RENDER CODE AND DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLFWwindow* window;
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
	prog[0] = Compile_Link_Shaders(vertex_prog, fragment_prog_base); // Programa base
	prog[1] = Compile_Link_Shaders(vertex_prog, fragment_prog); // Programa con regilla

	posRegilla=glGetUniformLocation(prog[1], "regilla");
	glUseProgram(prog[0]); 
}


vec3 pos_obs=vec3(10.0f,0.0f,0.0f); //###vec3 pos_obs=vec3(1.5f,0.0f,0.0f); 
vec3 target = vec3(0.0f,0.0f,0.0f);
vec3 up = vec3(0,0,1);

float fov = 35.0f, aspect = 4.0f / 3.0f; //###float fov = 40.0f, aspect = 4.0f / 3.0f;

// Actualizar escena: cambiar posici�n objetos, nuevos objetros, posici�n c�mara, luces, etc.
void render_scene_base()
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

	glUniform3fv(posRegilla, 1, &regilla[0]);
	
	// ORDEN de dibujar
	glBindVertexArray(triangulo.VAO);              // Activamos VAO asociado al objeto
    glDrawArrays(GL_TRIANGLES, 0, triangulo.Nv);   // Orden de dibujar (Nv vertices)	
	glBindVertexArray(0);                          // Desconectamos VAO

	////////////////////////////////////////////////////////

}

void render_scene(){
	//Recoger colores del buffer que se ha pintado
	vector<vector<vec4>> preRender(ALTO, vector<vec4>(ANCHO));
	
	for(int i=0; i<ALTO; i++){
		for(int j=0; j<ANCHO; j++){
			glReadPixels(j, i, 1, 1, GL_RGB, GL_FLOAT, &preRender[i][j]);
			printf("Color: %f %f %f\n", preRender[i][j].r, preRender[i][j].g, preRender[i][j].b);
		}
	}

	//Crear textura con los colores recogidos

	//Dividir la textura en regilla

	//Calcular el color medio de cada cuadrado de la regilla

	//Pintar la regilla con los colores medios

	//Liberar memoria
	vector<vector<vec4>>().swap(preRender);
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
		render_scene_base();
		glfwSwapBuffers(window);
		render_scene();
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
			case GLFW_KEY_TAB: float aux=z; z=regilla.z; regilla.z=aux;
		}
	}
}


void asigna_funciones_callback(GLFWwindow* window)
{
	glfwSetWindowSizeCallback(window, ResizeCallback);
	glfwSetKeyCallback(window, KeyCallback);
}



