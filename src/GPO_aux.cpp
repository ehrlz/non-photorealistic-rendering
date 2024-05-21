

//####include <GpO.h>
#include "GpO.h"

#define STB_IMAGE_IMPLEMENTATION
//####include <stb\stb_image.h>
#include <stb_image.h>

// ASSIMP lib impl
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

////////////////////   

extern int ANCHO, ALTO;


// Funciones inicialización librerias, ventanas, OpenGL
void init_GLFW(void)
{
	if (!glfwInit())
	{
		fprintf(stdout, "No se inicializo libreria GLFW\n");
		exit(EXIT_FAILURE);
	}
	int Major, Minor, Rev;
	glfwGetVersion(&Major, &Minor, &Rev);
	printf("Libreria GLFW (ver. %d.%d.%d) inicializada\n", Major, Minor, Rev);
}


GLFWwindow*  Init_Window(const char* nombre)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(ANCHO, ALTO, nombre, NULL, NULL);
	if (window == NULL)
	{
		fprintf(stdout, "Fallo al crear ventana GLFW con OpenGL context %d.%d\n", OPENGL_MAJOR, OPENGL_MINOR);
		glfwTerminate();
		exit(EXIT_FAILURE);
		return NULL;
	}
	
	fprintf(stdout, "Ventana GLFW creada con contexto OpenGL %d.%d\n", OPENGL_MAJOR, OPENGL_MINOR);
	glfwMakeContextCurrent(window);
		
	asigna_funciones_callback(window);

	return window;
}


void load_Opengl(void)
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stdout, "Fallo al cargar funciones de OpenGL con GLAD\n");
		exit(EXIT_FAILURE);
	}	
	fprintf(stdout, "OpenGL Version: %s\n",glGetString(GL_VERSION));
	glViewport(0, 0, ANCHO, ALTO);

	printf("---------------------------------------------\n");
	return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CARGA, COMPILACION Y LINKADO DE LOS SHADERS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* leer_codigo_de_fichero(const char* fich)
{
	FILE* fid;
	fopen_s(&fid, fich, "rb");  if (fid == NULL) return NULL;

	fseek(fid, 0, SEEK_END);  long nbytes = ftell(fid);
	fprintf(stdout, "Leyendo codigo de %s (%d bytes)\n", fich, nbytes);

	char* buf = new char[nbytes + 1];
	fseek(fid, 0, SEEK_SET);
	fread(buf, 1, nbytes, fid);
	buf[nbytes] = 0;
	fclose(fid);

//	for (int k = 0; k < nbytes; k++) fprintf(stdout,"%c", buf[k]);
//	fprintf(stdout, "\n ------------------------\n");

	return buf;
}

GLuint compilar_shader(const char* Shader_source, GLuint type)
{
	//printf("--------------------------------\n");
	switch (type)
	{
	case GL_VERTEX_SHADER: printf("Compilando Vertex Shader :: "); break;
	case GL_FRAGMENT_SHADER: printf("Compilando Fragment Shader :: "); break;
	case GL_GEOMETRY_SHADER: printf("Compilando Geometric Shader :: "); break;
	}
		
  GLuint ShaderID = glCreateShader(type);  // Create shader object

  glShaderSource(ShaderID, 1, &Shader_source , NULL);    // Compile Shader
  glCompileShader(ShaderID);

  GLint Result = GL_FALSE;
  int InfoLogLength; char error[512]; 

  glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
  if (Result==GL_TRUE) fprintf(stdout,"Sin errores\n");  
  else 
  {
   fprintf(stdout,"ERRORES\n");  
   glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
   if(InfoLogLength>512) InfoLogLength=512;
   glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, error);
   fprintf(stdout,"\n%s\n", error);

  }
 printf("----------------------------------------------\n");
 return ShaderID;
}

int check_errores_programa(GLuint id)
{
 GLint Result = GL_FALSE;
 int InfoLogLength;
 char error[512]; 

 printf("Resultados del linker (GPU): ");
 glGetProgramiv(id, GL_LINK_STATUS, &Result);
 if (Result==GL_TRUE){
	fprintf(stdout,"Sin errores\n"); // Compiled OK
	return 0;
 } 
 else 
	{
     fprintf(stdout,"ERRORES\n");  
     glGetProgramiv(id, GL_INFO_LOG_LENGTH, &InfoLogLength);
	 if(InfoLogLength<1) InfoLogLength=1; if(InfoLogLength>512) InfoLogLength=512;
     glGetProgramInfoLog(id, InfoLogLength, NULL, error);
     fprintf(stdout, "\n%s\n",error);
	 //glfwTerminate();
	}
 printf("---------------------------------------------\n");
 return 1;
}


GLuint Compile_Link_Shaders(const char* vertexShader_source,const char*fragmentShader_source)
{
	// Compile Shaders
	GLuint VertexShaderID = compilar_shader(vertexShader_source, GL_VERTEX_SHADER);
	GLuint FragmentShaderID = compilar_shader(fragmentShader_source,GL_FRAGMENT_SHADER);

    // Link the shaders in the final program
 

	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);

	glLinkProgram(ProgramID);
	if(check_errores_programa(ProgramID) < 0) return -1;

	// Limpieza final
	glDetachShader(ProgramID, VertexShaderID);  glDeleteShader(VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);  glDeleteShader(FragmentShaderID);

	

    return ProgramID;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////  AUXILIARES 
/////////////////////////////////////////////////////////////////////////////////////////////////////////

GLuint cargar_textura(const char * imagepath, GLuint tex_unit, bool obj)
{
  stbi_set_flip_vertically_on_load(!obj);

  int width, height,nrChannels;
  unsigned char* data = stbi_load(imagepath, &width, &height,&nrChannels,0);

  if (data == NULL)
  {
	  fprintf(stdout, "Error al cargar imagen: existe el fichero %s?\n",imagepath);
	  glfwTerminate();
	  return 0;
  }

  glActiveTexture(tex_unit);   //glBindTexture(GL_TEXTURE_2D, 0); 
	
	GLuint textureID;
	glGenTextures(1, &textureID);             // Crear objeto textura
	glBindTexture(GL_TEXTURE_2D, textureID);  // "Bind" la textura creada
	
	//printf("%d %d data %8X\n", width, height, data);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);  //Pasa datos a GPU

	glGenerateMipmap(GL_TEXTURE_2D);
	
	stbi_image_free(data); 

	// Opciones de muestreo, magnificación, coordenadas fuera del borde, etc.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	

	//glBindTexture(GL_TEXTURE_2D, 0); 
	// DEvolvemos ID de la textura creada y cargada con la imagen
	return textureID;
}



GLuint cargar_cube_map(const char * imagepath, GLuint tex_unit)
{
	GLuint textureID;
	char fich[128];
	//const char* suf[6];
	//suf[0] = "posx"; suf[1] = "negx";
	//suf[2] = "posy"; suf[3] = "negy";
	//suf[4] = "posz"; suf[5] = "negz";

	char suf[6][5] = { "posx", "negx", "posy", "negy", "posz", "negz" };


	glActiveTexture(tex_unit);	//glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &textureID); glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	
	unsigned char *data;  int width, height, nrChannels;

	//stbi_set_flip_vertically_on_load(true);  // AQUI PARECE QUE NO ES NECESARIO PERO PUEDE QUE OCASIONALMENTE SI

	printf("CARGANDO CUBE_MAP de %s_xxx.jpg\n", imagepath);
	for (int k = 0; k < 6; k++)
	{
	 sprintf_s(fich, 128, "%s_%s.jpg", imagepath, suf[k]); //printf("CUBE_MAP: %s\n", fich);
	 data = stbi_load(fich, &width, &height, &nrChannels, 0);
	 if (data == NULL)
	 {
		 fprintf(stdout, "Error al cargar imagen: existe el fichero %s?\n", fich);
		 stbi_image_free(data);
		 glfwTerminate();
		 return 0;
	 }
	 glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+k, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	 stbi_image_free(data);
	}

	/*sprintf_s(fich,128,"%s_%s.jpg", imagepath, "posx"); printf("CUBE_MAP: %s\n", fich);
	data = stbi_load(fich, &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	sprintf_s(fich, 128, "%s_%s.jpg", imagepath, "negx"); printf("CUBE_MAP: %s\n", fich);
	data = stbi_load(fich, &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	sprintf_s(fich, 128, "%s_%s.jpg", imagepath, "posy"); printf("CUBE_MAP: %s\n", fich);
	data = stbi_load(fich, &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	sprintf_s(fich, 128, "%s_%s.jpg", imagepath, "negy"); printf("CUBE_MAP: %s\n", fich);
	data = stbi_load(fich, &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	sprintf_s(fich, 128, "%s_%s.jpg", imagepath, "posz"); printf("CUBE_MAP: %s\n", fich);
	data = stbi_load(fich, &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	sprintf_s(fich, 128, "%s_%s.jpg", imagepath, "negz"); printf("CUBE_MAP: %s\n", fich);
	data = stbi_load(fich, &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);*/

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);



	return textureID;
}


objeto cargar_modelo(char* fichero)
{
	objeto obj;
	GLuint VAO;
	GLuint buffer,i_buffer;
	
	

	GLuint N_vertices, N_caras, N_indices; 

	unsigned char *vertex_data; 
	unsigned char *indices; 


	 FILE* fid;
     fopen_s(&fid,fichero,"rb");

	 if (fid==NULL) { 
		 printf("Error al leer datos. Existe el fichero %s?\n",fichero); 
		 obj.VAO=0; obj.Ni=0; obj.tipo_indice=0;
		 glfwTerminate();
	     return obj;
	 }

	 fread((void*)&N_caras,4,1,fid);    
	 fread((void*)&N_indices,4,1,fid); 
	 //N_indices=3*N_caras; 
	 fread((void*)&N_vertices,4,1,fid); 

	 fseek(fid,0,SEEK_END);
	 unsigned int s_fichero=ftell(fid);


	 GLuint tipo = GL_UNSIGNED_INT; unsigned char s_index = 4;
	 if (N_vertices <= 65536) { tipo = GL_UNSIGNED_SHORT; s_index = 2; }
	 if (N_vertices <= 256)   { tipo = GL_UNSIGNED_BYTE; s_index = 1; }

	 fseek(fid,12,SEEK_SET);


	 GLuint bytes_indices=N_indices*s_index;
	 GLuint bytes_data = s_fichero-12-bytes_indices;
	 GLuint datos_per_vertex=((bytes_data/4)/N_vertices);

	 printf("Leyendo modelo de %s: (%d bytes)\n",fichero,s_fichero);
	  printf("%d vertices, %d triangulos. Lista de %d indices\n",N_vertices,N_caras,N_indices);
	// printf("%d vertices, %d triangulos\n",N_vertices,N_caras);
	 printf("Indices guardados en enteros de %d bytes\n",s_index);
	 printf("%d datos por vertice\n",datos_per_vertex);

	 obj.Ni=N_indices;
	 obj.Nv=N_vertices;
	 obj.Nt=N_caras;
	 obj.tipo_indice=tipo; 

	 vertex_data=(unsigned char*)malloc(N_vertices*datos_per_vertex*4); 
	 if (vertex_data==NULL) printf("ptr NULL\n"); 
	 indices=(unsigned char*)malloc(N_indices*s_index); 
	 if (indices ==NULL) printf("ptrindices NULL\n"); 

     fread((void*)vertex_data,4,datos_per_vertex*N_vertices,fid);
     fread((void*)indices,s_index,N_indices,fid);

	 fclose(fid);


    glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, N_vertices*datos_per_vertex*4, vertex_data, GL_STATIC_DRAW);

	// Defino 1er argumento (atributo 0) del vertex shader (siempre XYZ)
	glEnableVertexAttribArray(0); 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, datos_per_vertex*sizeof(float), 0);

	switch (datos_per_vertex)
	{
	case 3:   break;
	case 5:   // 2º atributo = UV
       	glEnableVertexAttribArray(1);
	    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, datos_per_vertex*sizeof(float), (void*)(3*sizeof(float)));
		break;
	case 6:   // 2º atributo = (nx,ny,nz)
       	glEnableVertexAttribArray(1);
	    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, datos_per_vertex*sizeof(float), (void*)(3*sizeof(float)));
		break;
	case 8:   // 2º atributo = UV, 3º atributo = (nx,ny,nz)
       	glEnableVertexAttribArray(1);
	    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, datos_per_vertex*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(2);
	    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, datos_per_vertex*sizeof(float), (void*)(5*sizeof(float)));
		break;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);  // Asignados atributos, podemos desconectar BUFFER

	glGenBuffers(1, &i_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, N_indices*s_index,indices, GL_STATIC_DRAW);
	
		
	glBindVertexArray(0);  //Cerramos Vertex Array con todo lidto para ser pintado

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	obj.VAO=VAO; 
	

	//GLuint k;
	//for (k=0;k<12;k++) printf("%.1f ",vertex_data[k]);
	//for (k=0;k<12;k++) printf("%1d ",indices[k]);

	// Una vez transferido datos liberamos memoria en CPU
	free((void*)vertex_data);
	free((void*)indices);


	return obj;

}

objeto cargar_modelo_obj(const char* fichero) {
    objeto obj;
    GLuint VAO;
    GLuint buffer, i_buffer;

    GLuint N_vertices, N_caras, N_indices;

    unsigned char* vertex_data;
    unsigned char* indices;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fichero, aiProcess_Triangulate | aiProcess_FlipUVs);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error al leer datos. Existe el fichero " << fichero << "?\n";
        obj.VAO = 0;
        obj.Ni = 0;
        obj.tipo_indice = 0;
        glfwTerminate();
        return obj;
    }

    aiMesh* mesh = scene->mMeshes[0]; // Suponiendo que solo hay un mesh en el archivo OBJ

    N_vertices = mesh->mNumVertices;
    N_caras = mesh->mNumFaces;
    N_indices = N_caras * 3;

	printf("Leyendo modelo de %s: \n",fichero);
	printf("%d vertices, %d triangulos. Lista de %d indices\n",N_vertices,N_caras,N_indices);
	// printf("%d vertices, %d triangulos\n",N_vertices,N_caras);
	//printf("Indices guardados en enteros de %d bytes\n",s_index);
	//printf("%d datos por vertice\n",datos_per_vertex);

    GLuint tipo = GL_UNSIGNED_INT;
    unsigned char s_index = 4;
    if (N_vertices <= 65536) {
        tipo = GL_UNSIGNED_SHORT;
        s_index = 2;
    }
    if (N_vertices <= 256) {
        tipo = GL_UNSIGNED_BYTE;
        s_index = 1;
    }

    std::vector<float> vertices;
    std::vector<unsigned int> indices_vec;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);

        if (mesh->HasNormals()) {
            vertices.push_back(mesh->mNormals[i].x);
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);
        } else{
			vertices.push_back(0.0f);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
		}

        if (mesh->mTextureCoords[0]) {
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        } else {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices_vec.push_back(face.mIndices[j]);
        }
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Defino 1er argumento (atributo 0) del vertex shader (siempre XYZ)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

	// 2o argumento (atributo 1) del vertex shader (uv)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	// 3er argumento (atributo 2) del vertex shader (nx,ny,nz)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);  // Asignados atributos, podemos desconectar BUFFER

    glGenBuffers(1, &i_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_vec.size() * s_index, indices_vec.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);  // Cierre vertex array

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    obj.VAO = VAO;
    obj.Ni = N_indices;
    obj.Nv = N_vertices;
    obj.Nt = N_caras;
    obj.tipo_indice = tipo;

    return obj;
}




void transfer_mat4(const char* name, mat4 M)
{
 GLuint loc;
 GLuint prog;
 
 glGetIntegerv(GL_CURRENT_PROGRAM,(GLint*)&prog);

 loc=glGetUniformLocation(prog,name); 
 if (loc == -1) {
	 printf("No existe variable llamada %s en el programa activo de la GPU (%d)\n", name, prog);
	 glfwTerminate(); //exit(EXIT_FAILURE);
	 //return NULL;
 }
 else glUniformMatrix4fv(loc, 1, GL_FALSE, &M[0][0]);
}

void transfer_mat3(const char* name, mat3 M)
{
 GLuint loc;
 GLuint prog;
 
 glGetIntegerv(GL_CURRENT_PROGRAM,(GLint*)&prog);

 loc=glGetUniformLocation(prog,name); 
 if (loc == -1) {
	 printf("No existe variable llamada %s en el programa activo de la GPU (%d)\n", name, prog);
	 glfwTerminate(); //exit(EXIT_FAILURE);
 }
 else glUniformMatrix3fv(loc, 1, GL_FALSE, &M[0][0]);
}


void transfer_vec4(const char* name, vec4 x)
{
 GLuint loc;
 GLuint prog;
 
 glGetIntegerv(GL_CURRENT_PROGRAM,(GLint*)&prog);
 loc=glGetUniformLocation(prog,name);
 if (loc == -1) {
	 printf("No existe variable llamada %s en el programa activo de la GPU (%d)\n", name, prog);
	 glfwTerminate(); //exit(EXIT_FAILURE);
 }
 else glUniform4fv(loc, 1, &x[0]);
}

void transfer_vec3(const char* name, vec3 x)
{
 GLuint loc;
 GLuint prog;
 
 glGetIntegerv(GL_CURRENT_PROGRAM,(GLint*)&prog);
 loc=glGetUniformLocation(prog,name);
 if (loc == -1) {
	 printf("No existe variable llamada %s en el programa activo de la GPU (%d)\n", name, prog);
	 glfwTerminate(); //exit(EXIT_FAILURE);
 }
 else glUniform3fv(loc, 1, &x[0]);
}

void transfer_vec2(const char* name, vec2 x)
{
 GLuint loc;
 GLuint prog;
 
 glGetIntegerv(GL_CURRENT_PROGRAM,(GLint*)&prog);
 loc=glGetUniformLocation(prog,name);
 if (loc == -1) {
	 printf("No existe variable llamCerramos Vertex Array con todo listo para ser pintadoada %s en el programa activo de la GPU (%d)\n", name, prog);
	 glfwTerminate(); //exit(EXIT_FAILURE);
 }
 else glUniform2fv(loc, 1, &x[0]);
}

void transfer_int(const char* name, GLuint valor)
{
 GLuint loc;
 GLuint prog;
 
 glGetIntegerv(GL_CURRENT_PROGRAM,(GLint*)&prog);
 loc=glGetUniformLocation(prog,name);
 if (loc == -1) {
	 printf("No existe variable llamada %s en el programa activo de la GPU (%d)\n", name, prog);
	 glfwTerminate(); //exit(EXIT_FAILURE);
 }
 else glUniform1i(loc,valor);
}


void transfer_float(const char* name, GLfloat valor)
{
 GLuint loc;
 GLuint prog;
 
 glGetIntegerv(GL_CURRENT_PROGRAM,(GLint*)&prog);
 loc=glGetUniformLocation(prog,name);
 if (loc == -1) {
	 printf("No existe variable llamada %s en el programa activo de la GPU (%d)\n", name, prog);
	 glfwTerminate(); //exit(EXIT_FAILURE);
 }
 else glUniform1f(loc, valor);
}


void vuelca_mat4(glm::mat4 M)
{
	int j, k;
	printf("--------------------------------------\n");
	for (k = 0; k<4; k++) { for (j = 0; j<4; j++) printf("%6.3f ", M[j][k]); printf("\n"); }
	printf("--------------------------------------\n");
}
