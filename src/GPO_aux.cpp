

//####include <GpO.h>
#include "GpO.h"

#define STB_IMAGE_IMPLEMENTATION
//####include <stb\stb_image.h>
#include <stb_image.h>

#include <vector>

// Custom obj loader
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
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
	fprintf(stdout, "Leyendo codigo de %s (%ld bytes)\n", fich, nbytes);

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

GLuint cargar_textura(const char * imagepath, GLuint tex_unit)
{
  stbi_set_flip_vertically_on_load(true);

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


objeto cargar_modelo(const char* fichero)
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

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoords;
    glm::vec3 normal;
};

bool LoadObj(const std::string& path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << path << std::endl;
        return false;
    }

    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec2> temp_texCoords;
    std::vector<glm::vec3> temp_normals;

	float extra_texCoord;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream s(line);
        std::string type;
        s >> type;

        if (type == "v") {
            glm::vec3 position;
            s >> position.x >> position.y >> position.z;
			// std::cout << "Position: "<< position.x << " " << position.y << " " << position.z << std::endl;
            temp_positions.push_back(position);
        } else if (type == "vt") {
            glm::vec2 texCoord;
			if(!(s >> texCoord.x >> texCoord.y >> extra_texCoord)){
				s >> texCoord.x >> texCoord.y;
			}
			// std::cout << "UV: " << texCoord.x << " " << texCoord.y << std::endl;
            temp_texCoords.push_back(texCoord);
        } else if (type == "vn") {
            glm::vec3 normal;
            s >> normal.x >> normal.y >> normal.z;
			// std::cout << "Normal: "<< normal.x << " " <<  normal.y << " " << normal.z << std::endl;
            temp_normals.push_back(normal);
        } else if (type == "f") {
            std::string v1, v2, v3, v4;
            unsigned int vIndex[4], tIndex[4], nIndex[4];
            char slash;

			s >> v1 >> v2 >> v3 >> v4;

            // Parsear el primer vértice
            std::istringstream v1Stream(v1);
            v1Stream >> vIndex[0] >> slash >> tIndex[0] >> slash >> nIndex[0];

            // Parsear el segundo vértice
            std::istringstream v2Stream(v2);
            v2Stream >> vIndex[1] >> slash >> tIndex[1] >> slash >> nIndex[1];

            // Parsear el tercer vértice
            std::istringstream v3Stream(v3);
            v3Stream >> vIndex[2] >> slash >> tIndex[2] >> slash >> nIndex[2];

            // Agregar los vértices y los índices
            for (int i = 0; i < 3; i++) {

				// std::cout << vIndex[i] << "/" << tIndex[i] << "/" << nIndex[i] << " ";
                Vertex vertex;
                vertex.position = temp_positions[vIndex[i]-1];
                vertex.texCoords = temp_texCoords[tIndex[i]-1];
                vertex.normal = temp_normals[nIndex[i]-1];
                vertices.push_back(vertex);
                indices.push_back(vertices.size() - 1);
            }
			
            // Si hay un cuarto vértice, parsearlo y agregarlo
            if (!v4.empty()) {
                std::istringstream v4Stream(v4);
                v4Stream >> vIndex[3] >> slash >> tIndex[3] >> slash >> nIndex[3];
				// std::cout << vIndex[3] << "/" << tIndex[3] << "/" << nIndex[3];
                Vertex vertex;
                vertex.position = temp_positions[vIndex[3] - 1];
                vertex.texCoords = temp_texCoords[tIndex[3] - 1];
                vertex.normal = temp_normals[nIndex[3] - 1];
                vertices.push_back(vertex);
				indices.push_back(indices.at(indices.size()-3));
				indices.push_back(indices.at(indices.size()-2));
				indices.push_back(vertices.size() - 1);
            }

			// std::cout << std::endl;
        }
    }

    file.close();
    return true;
}

bool load_obj(const char* path, objeto& obj) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    if (!LoadObj(path, vertices, indices)) {
        return false;
    }

    obj.Nv = vertices.size();
    obj.Ni = indices.size();
    obj.tipo_indice = GL_UNSIGNED_INT;

	printf("Reading model from %s\n",path);
	printf("%d vertex, %d triangles, %d index\n", obj.Nv, obj.Ni / 2, obj.Ni);
	printf("%ld bytes per index, %ld per vertex\n", sizeof(unsigned int), sizeof(Vertex));

    GLuint VBO, EBO;
    glGenVertexArrays(1, &obj.VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(obj.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Textura
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(1);

    // Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    return true;
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
