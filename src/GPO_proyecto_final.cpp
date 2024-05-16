/************************  GPO_01 ************************************
ATG, 2019
******************************************************************************/

#include <GpO.h>

// TAMA�O y TITULO INICIAL de la VENTANA
int ANCHO = 800, ALTO = 600;  // Tama�o inicial ventana
const char* prac = "Proyecto NPR";   // Nombre de la practica (aparecera en el titulo de la ventana).
GLuint posRegilla;
vec3 regilla=vec3(16, 16, 0);

// SHADERS EN FICHEROS

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////   RENDER CODE AND DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLFWwindow* window;
GLuint prog[3];

objeto spider;
GLuint textura_spider;

objeto buda;

char* vertex_prog;
char* fragment_prog;

int scene_flag = 0;
void change_scene(int option);

enum SCENES {PIXEL1, PIXEL2, TOON};

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

// Iluminación
float az = 0.f, el = .75f; // Azimut, elevación
vec3 L; // Vector de iluminación

// Preparaci�n de los datos de los objetos a dibujar, envialarlos a la GPU
// Compilaci�n programas a ejecutar en la tarjeta gr�fica:  vertex shader, fragment shaders
// Opciones generales de render de OpenGL
void init_scene()
{
	int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height); 
    
	spider = cargar_modelo("../data/spider.bix");  // Preparar datos de objeto, mandar a GPU
	textura_spider = cargar_textura("../data/spider.jpg", GL_TEXTURE0);
	buda = cargar_modelo("../data/buda_n.bix");

	PP = perspective(glm::radians(25.0f), 4.0f / 3.0f, 0.1f, 20.0f);  //40� Y-FOV,  4:3 ,  Znear=0.1, Zfar=20
	VV = lookAt(pos_obs, target, up);  // Pos camara, Lookat, head up

	// Mandar programas a GPU, compilar y crear programa en GPU

	// Compilado de Shaders
	// Mandar programas a GPU, compilar y crear programa en GPU
	vertex_prog = leer_codigo_de_fichero("../data/shaders/pixel.vs");
	fragment_prog = leer_codigo_de_fichero("../data/shaders/pixel1.fs");
	prog[0] = Compile_Link_Shaders(vertex_prog, fragment_prog);

	fragment_prog = leer_codigo_de_fichero("../data/shaders/pixel2.fs");
	prog[1] = Compile_Link_Shaders(vertex_prog, fragment_prog);

	vertex_prog = leer_codigo_de_fichero("../data/shaders/toon.vs");
	fragment_prog = leer_codigo_de_fichero("../data/shaders/toon.fs");
	prog[2] = Compile_Link_Shaders(vertex_prog, fragment_prog);
	
	posRegilla=glGetUniformLocation(prog[0], "regilla");
	change_scene(PIXEL1);	// Indicamos que programa vamos a usar 

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

	if(scene_flag == TOON){
		mat4 R1 = rotate(radians(90.0f), vec3(1.f, 0.f, 0.f));
		mat4 R2 = rotate(t, vec3(0.f, 1.f, 0.f));
		T=translate(vec3(0.f, 0.f, 0.f));
		M = T*R1*R2;
		transfer_mat4("PV",PP*VV);
		transfer_mat4("M", M);

		L = vec3(cos(az) * cos(el), sin(az), cos(az) * sin(el)); // al estar el buda "tumbado", az y el están invertidos
		transfer_vec3("luz", L);
	}else{
		R=rotate(t, vec3(0, 0, 1.f));  
		T=translate(vec3(0.f, 0.f, 0.f));
		M = T*R;
		transfer_mat4("MVP", PP*VV*M); 
	}
	glUniform3fv(posRegilla, 1, &regilla[0]);
	
	//L = vec3(cos(el) * cos(az), sin(el), cos(el) * sin(az));
	//transfer_vec3("luz", L);

	// ORDEN de dibujar

	if(scene_flag == TOON){
		dibujar_indexado(buda);
	}else{
		dibujar_indexado(spider);
	}	
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
static float LIGHT_MOVE_SCALE = 0.1;

static void KeyCallback(GLFWwindow* window, int key, int code, int action, int mode)
{
	fprintf(stdout, "Key %d Code %d Act %d Mode %d\n", key, code, action, mode);
	if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);

	if(action!=GLFW_RELEASE){
		switch(key){
			case GLFW_KEY_UP: regilla.y+=0.1; break;
			case GLFW_KEY_DOWN: regilla.y-=0.1; break;
			case GLFW_KEY_LEFT: 
				if(scene_flag == TOON){
					az -= LIGHT_MOVE_SCALE;
				}else{
					regilla.x-=0.1;
				}
				break;
			case GLFW_KEY_RIGHT:
				if(scene_flag == TOON){
					az += LIGHT_MOVE_SCALE;
				}else{
					regilla.x+=0.1;
				}
				break;
			case GLFW_KEY_KP_ADD: regilla.z+=0.1; break;
			case GLFW_KEY_KP_SUBTRACT: regilla.z-=0.1; break;
			case GLFW_KEY_0: 
				change_scene(PIXEL1);
				posRegilla=glGetUniformLocation(prog[0], "regilla");
				break;
			case GLFW_KEY_1: 
				change_scene(PIXEL2);
				posRegilla=glGetUniformLocation(prog[1], "regilla");
				break;
			case GLFW_KEY_2: 
				change_scene(TOON);
				break;
			case GLFW_KEY_TAB: float aux=z; z=regilla.z; regilla.z=aux; break;
		}
	}
}

void change_scene(int option){
	if(option < 0 && option > 2) // UPDATE IF A NEW SHADER IS IMPLEMENTED
		return;
	
	switch (option)
	{
	case 0:
		printf("Pixel shading 1\n");
		break;
	case 1:
		printf("Pixel shading 2\n");
		break;
	case 2:
		printf("Toon shading\n");
		break;
	}
	scene_flag = option;
	glUseProgram(prog[option]);


}


void asigna_funciones_callback(GLFWwindow* window)
{
	glfwSetWindowSizeCallback(window, ResizeCallback);
	glfwSetKeyCallback(window, KeyCallback);
}



