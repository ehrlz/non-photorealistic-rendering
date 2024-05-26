/************************  GPO_01 ************************************
ATG, 2019
******************************************************************************/

#include <GpO.h>

// TAMAÑO y TITULO INICIAL de la VENTANA
int ANCHO = 800, ALTO = 600;  // Tama�o inicial ventana
const char* prac = "Proyecto NPR";   // Nombre de la practica (aparecera en el titulo de la ventana).
GLuint posRejilla;
vec3 rejilla=vec3(16, 16, 0);

// SHADERS EN FICHEROS

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////   RENDER CODE AND DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLFWwindow* window;
GLuint prog[3];

objeto spider;
GLuint textura_spider;
objeto buda;
objeto helmet;
GLuint textura_helmet;

char* vertex_prog;
char* fragment_prog;

int scene_flag = 0;
int model_flag = 0;
void change_scene(int option);
void change_model(int option);

bool pixelArtActive = false;
enum SCENES {BASE, PIXEL, TOON};
enum MODELS {SPIDER, BUDA, HELMET};

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

// Preparación de los datos de los objetos a dibujar, envialarlos a la GPU
// Compilación programas a ejecutar en la tarjeta gráfica:  vertex shader, fragment shaders
// Opciones generales de render de OpenGL
void init_scene()
{
	int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height); 
    
	spider = cargar_modelo("../data/spider.bix");  // Preparar datos de objeto, mandar a GPU
	textura_spider = cargar_textura("../data/spider.jpg", GL_TEXTURE1);

	buda = cargar_modelo("../data/buda_n.bix");

	helmet = cargar_modelo_obj("../data/helmet.obj");
	textura_helmet = cargar_textura("../data/helmet.jpg", GL_TEXTURE2);


	PP = perspective(glm::radians(25.0f), 4.0f / 3.0f, 0.1f, 20.0f);  //25º Y-FOV,  4:3 ,  Znear=0.1, Zfar=20
	VV = lookAt(pos_obs, target, up);  // Pos camara, Lookat, head up

	// Mandar programas a GPU, compilar y crear programa en GPU

	// Compilado de Shaders
	// Mandar programas a GPU, compilar y crear programa en GPU
	vertex_prog = leer_codigo_de_fichero("../data/shaders/base.vs");
	fragment_prog = leer_codigo_de_fichero("../data/shaders/base.fs");
	prog[0] = Compile_Link_Shaders(vertex_prog, fragment_prog);

	fragment_prog = leer_codigo_de_fichero("../data/shaders/pixel2.fs");
	prog[1] = Compile_Link_Shaders(vertex_prog, fragment_prog);

	vertex_prog = leer_codigo_de_fichero("../data/shaders/toon.vs");
	fragment_prog = leer_codigo_de_fichero("../data/shaders/toon.fs");
	prog[2] = Compile_Link_Shaders(vertex_prog, fragment_prog);

	// Inicializado de postprocesado
	initFrameBuffer();
	create_postprocess_screen();
	compile_second_render_shaders();

	posRejilla=glGetUniformLocation(prog[0], "rejilla");
	change_scene(BASE);	// Indicamos que programa vamos a usar 

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// float fov = 35.0f, aspect = 4.0f / 3.0f; //###float fov = 40.0f, aspect = 4.0f / 3.0f;

// Actualizar escena: cambiar posici�n objetos, nuevos objetros, posici�n c�mara, luces, etc.
void render_scene()
{	
	if(pixelArtActive){
		// printf("SCENE WITH POSTRENDER\n");
		render_to_texture();
	}

	glClearColor(0.1f,0.1f,0.1f,1.0f);  // Especifica color para el fondo (RGB+alfa)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);          // Aplica color asignado borrando el buffer

	if(pixelArtActive){
		glEnable(GL_DEPTH_TEST);
	}

	float t = (float)glfwGetTime();  // Contador de tiempo en segundos 

	///////// Aqui vendr�a nuestr c�digo para actualizar escena  /////////	
	mat4 M, T, R, S;

	if(scene_flag == BASE || scene_flag == PIXEL){
		R=rotate(t, vec3(0, 0, 1.f));  
		T=translate(vec3(0.f, 0.f, 0.f));
		M = T*R;
		transfer_mat4("MVP", PP*VV*M);
		if(scene_flag == PIXEL){
			transfer_vec3("rejilla", rejilla);
			transfer_vec2("resolucion", vec2(ANCHO, ALTO));
		}
	}
	else if(scene_flag == TOON){
		if(model_flag == BUDA){
			mat4 R1 = rotate(radians(90.0f), vec3(1.f, 0.f, 0.f));
			mat4 R2 = rotate(t, vec3(0.f, 1.f, 0.f));
			T=translate(vec3(0.f, 0.f, 0.f));
			M = T*R1*R2;

			L = vec3(cos(az) * cos(el), sin(az), cos(az) * sin(el)); // al estar el buda "tumbado", az y el están invertidos
		}else if (model_flag == HELMET){
			T=translate(vec3(0.f, 0.f, 0.f));
			R=rotate(t, vec3(0.f, 0.f, 1.f));
			S=scale(vec3(0.1f,0.1f,0.1f));
			M = T * R * S;

			L = vec3(cos(az) * cos(el), sin(az), cos(az) * sin(el));
		}

		transfer_mat4("PV",PP*VV);
		transfer_mat4("M", M);
		transfer_vec3("luz", L);
	}else{
		fprintf(stderr, "[ERROR]: Bad scene def.");
		
	}
	
	//L = vec3(cos(el) * cos(az), sin(el), cos(el) * sin(az));
	//transfer_vec3("luz", L);

	// ORDEN de dibujar
	switch (model_flag)
	{
		case SPIDER:
		transfer_int("unit",1);
		dibujar_indexado(spider);
		break;
		case BUDA:
		dibujar_indexado(buda);
		break;
		case HELMET:
		dibujar_indexado(helmet);
		break;
	}

	if(pixelArtActive){
		post_process();
		glUseProgram(prog[scene_flag]);
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
	
	delete_second_render(); // TODO delete objetos
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

	second_render_reshape();
}

// Callback de pulsacion de tecla
float z=4.0f; // Ancho sombreado permite intercambio de tamaño de sombras
static float LIGHT_MOVE_SCALE = 0.1;

static void KeyCallback(GLFWwindow* window, int key, int code, int action, int mode)
{
	//fprintf(stdout, "Key %d Code %d Act %d Mode %d\n", key, code, action, mode);
	if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);

	if(action!=GLFW_RELEASE){
		switch(key){
			case GLFW_KEY_UP: rejilla.y+=0.1; break; // Aumento de longitud pixeles en y
			case GLFW_KEY_DOWN: rejilla.y-=0.1; break; // Disminucion de longitud pixeles en y
			case GLFW_KEY_LEFT: 
				if(scene_flag == TOON){
					az -= LIGHT_MOVE_SCALE;
				}else{ // Disminucion de longitud pixeles en x
					rejilla.x-=0.1;
				}
				break;
			case GLFW_KEY_RIGHT:
				if(scene_flag == TOON){
					az += LIGHT_MOVE_SCALE;
				}else{ // Aumento de longitud pixeles en x
					rejilla.x+=0.1;
				}
				break;

			// Cambio de tamaño del sombreado
			case GLFW_KEY_KP_ADD: rejilla.z+=0.1; break;
			case GLFW_KEY_KP_SUBTRACT: rejilla.z-=0.1; break;

			// Cambio de shader
			case GLFW_KEY_1: change_scene(PIXEL); break;
			case GLFW_KEY_2: change_scene(TOON); break;
			case GLFW_KEY_B: change_scene(BASE); break;
			case GLFW_KEY_0: 
				if(pixelArtActive){
					pixelArtActive=false;
				}else{
					pixelArtActive=true;
				}
				break;
			
			//Intercambio de sombreado
			case GLFW_KEY_TAB:
				{
					float aux=z; z=rejilla.z; rejilla.z=aux; // in a block to avoid cross initialization
				}
				break;
			case GLFW_KEY_PERIOD:
				change_model((model_flag+1) % 3);
				break;
			case GLFW_KEY_COMMA:
				change_model((model_flag-1) % 3);
				break;
		}
	}
}

void change_scene(int option){
	if(option < 0 || option > 2){ // UPDATE IF A NEW SHADER IS IMPLEMENTED
		fprintf(stderr,"Scene not available\n");
		return;
	} 
	scene_flag = option;
	
	switch (option)
	{
	case 0:
		printf("Base shading\n");
		change_model(SPIDER);
		break;
	case 1:
		printf("Pixel shading 2\n");
		change_model(SPIDER);
		break;
	case 2:
		printf("Toon shading\n");
		change_model(BUDA);
		break;
	}
	glUseProgram(prog[option]);
}

// SPIDER -> PIXEL, BUDA & HELMET -> TOON
void change_model(int option){
	if(option < 0 || option > 2){ // UPDATE IF A NEW MODEL IS IMPLEMENTED
		fprintf(stderr,"Model not available\n");
		return;
	}
	
	switch (option)
	{
	case 0:
		if(scene_flag == TOON){
			fprintf(stderr, "[ERROR] Modelo spiderman solo para modo pixel\n");
			return;
		}
		printf("SPIDER\n");
		break;
	case 1:
		if(scene_flag != TOON){
			fprintf(stderr, "[ERROR] Modelo buda solo para modo toon\n");
			return;
		}
		printf("BUDA\n");
		break;
	case 2:
		if(scene_flag != TOON){
			fprintf(stderr, "[ERROR] Modelo spiderman solo para modo toon\n");
			return;
		}
		printf("HELMET\n");
		break;
	}
	model_flag = option;
}


void asigna_funciones_callback(GLFWwindow* window)
{
	glfwSetWindowSizeCallback(window, ResizeCallback);
	glfwSetKeyCallback(window, KeyCallback);
}



