#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>

using namespace glimac;

int main(int argc, char** argv) {
    // Initialize SDL and open a window
    SDLWindowManager windowManager(800, 800, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    FilePath applicationPath(argv[0]);
    Program program = loadProgram(
        applicationPath.dirPath() + "shaders/" + argv[1],
        applicationPath.dirPath() + "shaders/" + argv[2]
        );
    program.use();


    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    /*******************
     * INITIALIZATION  *
     *******************/
    
    // Création du vbo
    GLuint vbo;
    glGenBuffers(1, &vbo);
    
    // Binding du VBO    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Remplir le VBO 
    GLfloat vertices[] = {
        -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,    // sommet 1
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,     // sommet 2
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f       // sommet 3 
        };

    glBufferData(GL_ARRAY_BUFFER, 15 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    // Débinder le VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Création du VAO 
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // Binding du VAO 
    glBindVertexArray(vao);

   // Ativation des attributs de vertex
    const GLuint VERTEX_ATTR_POSITION = 0;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    const GLuint VERTEX_ATTR_COLOR = 1;
    glEnableVertexAttribArray(VERTEX_ATTR_COLOR);

    // Spécification des attributs de vertex
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // position des points
    glVertexAttribPointer(
        VERTEX_ATTR_POSITION, 
        2, GL_FLOAT, GL_FALSE, 
        5*sizeof(GLfloat), 0);

    // couleur des points
    glVertexAttribPointer(
        VERTEX_ATTR_COLOR, 
        3, GL_FLOAT, GL_FALSE, 
        5*sizeof(GLfloat), (GLvoid*) (2*sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Débind du VAO
    glBindVertexArray(0);

    /*******************
     * END OF INITIAL. *
     *******************/

    // Application loop:
    bool done = false;
    while(!done) {
        // Event loop:
        SDL_Event e;
        while(windowManager.pollEvent(e)) {
            if(e.type == SDL_QUIT) {
                done = true; // Leave the loop after this iteration
            }
        }


        /******************
         * RENDERING CODE *
         ******************/
        
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    return EXIT_SUCCESS;
}
