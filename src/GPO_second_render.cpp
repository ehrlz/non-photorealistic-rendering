    #include <GpO.h>

GLuint frame_buffer;
GLuint tex_color_buffer;
GLuint rbo_depth;

GLuint prog_second_render;
char *vertex_prog_second_render;
char *fragment_prog_second_render;

GLuint rectVAO, rectVBO;

static const float fbo_vertices[] = {
    // Coords            texCoords
    1.f, 1.f, 0.f,     1.f, 1.0f, // Top-right
    -1.f, 1.f, 0.f,    0.f, 1.0f, // Top-left
    1.f, -1.f, 0.f,    1.f, 0.0f, // Bottom-right
    
    -1.f, 1.f, 0.f,    0.f, 1.0f, // Top-left
    -1.f, -1.f, 0.f,   0.f, 0.0f, // Bottom-left
    1.f, -1.f, 0.f,    1.f, 0.0f, // Bottom-right
};

void compile_second_render_shaders()
{
    vertex_prog_second_render = leer_codigo_de_fichero("../data/shaders/pixelArt.vs");
    fragment_prog_second_render = leer_codigo_de_fichero("../data/shaders/pixelArt.fs");
    prog_second_render = Compile_Link_Shaders(vertex_prog_second_render, fragment_prog_second_render);
    glUseProgram(prog_second_render);
    transfer_int("bufferTexture", 0);
}


// Inicializa las estructuras del framebuffer
void initFrameBuffer()
{

    // Framebuffer para "juntar" todo
    glGenFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

    // Creación de textura para almacenar el renderizado
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex_color_buffer);                                                    
    glBindTexture(GL_TEXTURE_2D, tex_color_buffer);                                         
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Parámetros
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ANCHO, ALTO, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); // Vacía

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_color_buffer, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_NONE);

    // Buffer de profundidad
    glGenRenderbuffers(1, &rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, ANCHO, ALTO);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Comprobación de errores
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "ERROR::FRAMEBUFFER:: Framebuffer no esta completo! Causa: %d\n", status);
        return;
    }
    // Volvemos a operar en la pantalla
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Crea el VAO para poder renderizar una cuadrado con las textura procesada
void create_postprocess_screen()
{
    glGenVertexArrays(1, &rectVAO);
    glGenBuffers(1, &rectVBO);

    glBindVertexArray(rectVAO);

    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Desvinculado de VBO y VAO
    glBindVertexArray(0);
}

// Cambia de la pantalla a la textura
void render_to_texture()
{
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glViewport(0, 0, ANCHO, ALTO);
}

// Dibuja la textura en la pantalla
void post_process()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, ANCHO, ALTO);
    glDisable(GL_DEPTH_TEST);

    glClearColor(0.1f,0.1f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(prog_second_render);
    glActiveTexture(GL_TEXTURE0);

    transfer_int("bufferTexture", 0);

    glBindVertexArray(rectVAO);
    glBindTexture(GL_TEXTURE_2D, tex_color_buffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

// En caso de cambio de tamaño de la ventana
void second_render_reshape()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ALTO, ANCHO, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, ALTO, ANCHO);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void delete_second_render(void)
{
    glDeleteBuffers(1, &rectVBO);
    glDeleteRenderbuffers(1, &rbo_depth);
    glDeleteTextures(1, &tex_color_buffer);
    glDeleteFramebuffers(1, &frame_buffer);
    glDeleteProgram(prog_second_render);
}