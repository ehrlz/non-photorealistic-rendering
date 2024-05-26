/************************  NPR_RENDERING ************************************
Alonso García Elías Herrero, 2024
******************************************************************************/

#include <GpO.h>

// TAMAÑO y TITULO INICIAL de la VENTANA
int ANCHO = 1200, ALTO = 900;  // Tamaño inicial ventana
const char* prac = "Proyecto NPR";   // Nombre de la practica (aparecera en el titulo de la ventana).
GLuint posRejilla;
vec3 rejilla=vec3(16, 16, 0);

// SHADERS EN FICHEROS

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////   RENDER CODE AND DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLFWwindow* window;
GLuint prog[6];

objeto spider;
GLuint textura_spider;
objeto ball;
objeto helmet;
GLuint textura_helmet;
objeto cat;
GLuint textura_cat;

char* vertex_prog;
char* fragment_prog;

int scene_flag = 0;
int model_flag = 0;
void change_scene(int option);
void change_model(int option);

int rotating = 1;

// ----- SHADER PARAMS -----
float az = 0.f, el = .75f; // Azimut, elevación

// phong and blinn
vec4 light_coefs = vec4(.2f,.7f,.1f,1.f); // ilum coefs: 20% ambiental + 70% diffuse + 10% spec | spec aperture

float toon_border = 0.2; // Toon shading
int color_levels = 3;

float b_lightness = 1.f; // Gooch
float y_lightness = 1.f;
float alpha = 0.f;
float beta = 0.f;

int render_texture = 1; // // Option (defecto: encendido)
vec3 model_color = vec3(1,1,1);

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
vec3 L;

// Preparación de los datos de los objetos a dibujar, envialarlos a la GPU
// Compilación programas a ejecutar en la tarjeta gráfica:  vertex shader, fragment shaders
// Opciones generales de render de OpenGL
void init_scene()
{
	int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height); 
    
	spider = cargar_modelo("../data/spider.bix");  // Preparar datos de objeto, mandar a GPU
	textura_spider = cargar_textura("../data/spider.jpg", GL_TEXTURE0, false);

	ball = cargar_modelo_obj("../data/ball_fountain.obj");

	helmet = cargar_modelo_obj("../data/helmet.obj");
	textura_helmet = cargar_textura("../data/helmet.jpg", GL_TEXTURE1, true);

	cat = cargar_modelo_obj("../data/cat.obj");
	textura_cat = cargar_textura("../data/cat.jpg", GL_TEXTURE2, true);

	PP = perspective(glm::radians(25.0f), 4.0f / 3.0f, 0.1f, 20.0f);  //25º Y-FOV,  4:3 ,  Znear=0.1, Zfar=20
	VV = lookAt(pos_obs, target, up);  // Pos camara, Lookat, head up

	// Compilado de Shaders
	// Mandar programas a GPU, compilar y crear programa en GPU

	// PIXEL 1
	vertex_prog = leer_codigo_de_fichero("../data/shaders/pixel.vs");
	fragment_prog = leer_codigo_de_fichero("../data/shaders/pixel1.fs");
	prog[0] = Compile_Link_Shaders(vertex_prog, fragment_prog);

	// PIXEL 2
	fragment_prog = leer_codigo_de_fichero("../data/shaders/pixel2.fs");
	prog[1] = Compile_Link_Shaders(vertex_prog, fragment_prog);

	// TOON SHADING
	vertex_prog = leer_codigo_de_fichero("../data/shaders/phong.vs"); // same v.s. as phong
	fragment_prog = leer_codigo_de_fichero("../data/shaders/toon.fs");
	prog[2] = Compile_Link_Shaders(vertex_prog, fragment_prog);
	
	// PHONG SHADING
	fragment_prog = leer_codigo_de_fichero("../data/shaders/phong.fs");
	prog[3] = Compile_Link_Shaders(vertex_prog, fragment_prog);

	// BLINN-PHONG SHADING
	// vertex shader is the same than phong
	fragment_prog = leer_codigo_de_fichero("../data/shaders/blinn-phong.fs");
	prog[4] = Compile_Link_Shaders(vertex_prog, fragment_prog);

	fragment_prog = leer_codigo_de_fichero("../data/shaders/gooch.fs");
	prog[5] = Compile_Link_Shaders(vertex_prog, fragment_prog);

	posRejilla=glGetUniformLocation(prog[0], "rejilla");
	change_scene(PIXEL1);	// Indicamos que programa vamos a usar 

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

// float fov = 35.0f, aspect = 4.0f / 3.0f; //###float fov = 40.0f, aspect = 4.0f / 3.0f;

float t;
float last_t;

// Actualizar escena: cambiar posici�n objetos, nuevos objetros, posici�n c�mara, luces, etc.
void render_scene()
{
	glClearColor(0.1f,0.1f,0.1f,0.0f);  // Especifica color para el fondo (RGB+alfa)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);          // Aplica color asignado borrando el buffer

	if(rotating) {  // stop rotation
		t = (float)glfwGetTime();  // Contador de tiempo en segundos
		last_t = t;
	}
	else {
		t = last_t;
	}
	 	

	///////// Aqui vendría nuestr código para actualizar escena  /////////	
	mat4 M, T, R, S;

	if(scene_flag == PIXEL1 || scene_flag == PIXEL2){
		R=rotate(t, vec3(0, 0, 1.f));  
		T=translate(vec3(0.f, 0.f, 0.f));
		M = T*R;

		transfer_mat4("MVP", PP*VV*M);
		transfer_vec3("rejilla", rejilla);
		transfer_vec2("resolucion", vec2(ANCHO, ALTO));
	}
	else {
		if(model_flag == BALL){
			T=translate(vec3(0.f, 0.f, 0.5f));
			S=scale(vec3(0.15f,0.15f,0.15f));
			M = T * S;
		}else if (model_flag == HELMET){
			T=translate(vec3(0.f, 0.f, 0.5f));
			R=rotate(t, vec3(0.f, 0.f, 1.f));
			S=scale(vec3(0.1f,0.1f,0.1f));
			M = T * R * S;
		}else if (model_flag == CAT){
			T=translate(vec3(0.f, 0.f, 0.f));
			R=rotate(t, vec3(0.f, 0.f, 1.f));
			S=scale(vec3(0.05f,0.05f,0.05f));
			M = T * R * S;
		}

		// Cálculo de la dirección de la luz
		L = vec3(2*sqrt(2)*cos(az), 2*sqrt(2)*sin(az), 1) / 3.f; // normalizado
		transfer_vec3("light", L);

		// Se transfieren las matrices de modelado
		transfer_mat4("PV",PP*VV);
		transfer_mat4("M", M);
	}
	
	// DIBUJO DEL MODELO
	switch (model_flag)
	{
		case SPIDER:
		dibujar_indexado(spider);
		break;
		case BALL:
		dibujar_indexado(ball);
		break;
		case HELMET:
		dibujar_indexado(helmet);
		break;
		case CAT:
		dibujar_indexado(cat);
		break;
	}
}


void apply_options()
{	
	// SHADER
	glUseProgram(prog[scene_flag]);

	if(scene_flag != PIXEL1 && scene_flag != PIXEL2){
		transfer_vec3("campos",pos_obs);
	}

	if(scene_flag == PHONG || scene_flag == BLINN){
		transfer_vec4("coefs", light_coefs);
	}

	if(scene_flag == TOON){
		transfer_float("toon_border", toon_border);
		transfer_int("color_levels", color_levels);
	} else if(scene_flag == GOOCH) {
		transfer_float("b_lightness", b_lightness);
		transfer_float("y_lightness", y_lightness);
		transfer_float("alpha",alpha);
		transfer_float("beta",beta);
	}

	// TEXTURE
	switch (model_flag)
	{
	case SPIDER:
		break;
	case BALL:
		render_texture = 0; // ball doesn't have texture
		break;
	case HELMET:
		if(scene_flag != PIXEL1 && scene_flag != PIXEL2)
			transfer_int("unit",1);
		break;
	case CAT:
		if(scene_flag != PIXEL1 && scene_flag != PIXEL2)
			transfer_int("unit",2);
		break;
	}

	// OPTIONS
	if(scene_flag != PIXEL1 && scene_flag != PIXEL2){ // TODO
		transfer_int("render_texture", render_texture);
		transfer_vec3("model_color", model_color); // selec color si no hay textura
	} 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROGRAMA PRINCIPAL
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	init_GLFW();            // Inicializa lib GLFW
	window = Init_Window(prac);  // Crea ventana usando GLFW, asociada a un contexto OpenGL	X.Y
	
	setupImGui(window);

	load_Opengl();         // Carga funciones de OpenGL, comprueba versi�n.
	init_scene();          // Prepara escena
	
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window))
	{
		render_scene();

		renderImGui(&scene_flag, &model_flag, &render_texture, &color_levels, &toon_border,
					&model_color, &b_lightness, &y_lightness, &alpha, &beta, &light_coefs);

		apply_options(); // aplica los casos de uso

		glfwSwapBuffers(window);
		glfwPollEvents();
		show_info();
	}

	terminateImGui();
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
				if(scene_flag != PIXEL1 && scene_flag != PIXEL2){
					az -= LIGHT_MOVE_SCALE;
				}else{ // Disminucion de longitud pixeles en x
					rejilla.x-=0.1;
				}
				break;
			case GLFW_KEY_RIGHT:
				if(scene_flag != PIXEL1 && scene_flag != PIXEL2){
					az += LIGHT_MOVE_SCALE;
				}else{ // Aumento de longitud pixeles en x
					rejilla.x+=0.1;
				}
				break;

			// Cambio de tamaño del sombreado
			case GLFW_KEY_KP_ADD: rejilla.z+=0.1; break;
			case GLFW_KEY_KP_SUBTRACT: rejilla.z-=0.1; break;

			// Cambio de shader
			case GLFW_KEY_0: change_scene(PIXEL1); break;
			case GLFW_KEY_1: change_scene(PIXEL2); break;
			case GLFW_KEY_2: change_scene(TOON); break;
			case GLFW_KEY_3: change_scene(PHONG); break;
			case GLFW_KEY_4: change_scene(BLINN); break;
			
			//Intercambio de sombreado
			case GLFW_KEY_TAB:
				{
					float aux=z; z=rejilla.z; rejilla.z=aux; // in a block to avoid cross initialization
				}
				break;
			case GLFW_KEY_PERIOD:
				change_model((model_flag+1) % 4);
				break;
			case GLFW_KEY_COMMA:
				--model_flag;
				if(model_flag < 0)
					model_flag = 0;
				change_model(model_flag-1);
				break;

			case GLFW_KEY_T:
				// BALL_FOUNTAIN DOESN'T HAVE TEXTURE
				// TODO: IMPLEMENT SWITCH PIXEL SHADER
				if(model_flag == BALL || scene_flag == PIXEL1 || scene_flag == PIXEL2)
					break;
				render_texture = ++render_texture % 2; 
				printf("RENDER STATUS: %d\n", render_texture);
				transfer_int("render_texture",render_texture);
				break;
			case GLFW_KEY_SPACE:
				rotating = ++rotating % 2;
				if(rotating) glfwSetTime((double)last_t); // set the time to the stop instant to continue
				break;
		}
	}
}

void change_scene(int option){
	if(option < 0 || option > 4){ // UPDATE IF A NEW SHADER IS IMPLEMENTED
		fprintf(stderr,"Scene not available\n");
		return;
	} 
	scene_flag = option;
	
	switch (option)
	{
	case 0:
		printf("Pixel shading 1\n");
		change_model(SPIDER);
		break;
	case 1:
		printf("Pixel shading 2\n");
		change_model(SPIDER);
		break;
	case 2:
		printf("Toon shading\n");
		change_model(model_flag);
		break;
	case 3:
		printf("Phong shading\n");
		change_model(model_flag);
		break;
	case 4:
		printf("Blinn-phong shading\n");
		change_model(model_flag);
		break;
	}
	glUseProgram(prog[option]);
}


// SPIDER -> PIXEL, esfera & HELMET -> TOON
void change_model(int option){
	if(option < 0 || option > 3){ // UPDATE IF A NEW MODEL IS IMPLEMENTED
		fprintf(stderr,"Model not available\n");
		return;
	}
	
	switch (option)
	{
	case 0:
		printf("SPIDER\n");
		break;
	case 1:
		printf("BALL\n");
		break;
	case 2:
		if (scene_flag == TOON){
			transfer_int("unit",1);
		}
		printf("HELMET\n");
		break;
	case 3:
		if (scene_flag == TOON){
			transfer_int("unit",2);
		}
		printf("CAT\n");
		break;
	}
	model_flag = option;
}

void asigna_funciones_callback(GLFWwindow* window)
{
	glfwSetWindowSizeCallback(window, ResizeCallback);
	glfwSetKeyCallback(window, KeyCallback);
}



