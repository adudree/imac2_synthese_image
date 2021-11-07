#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/glm.hpp>
#include <vector>

using namespace glimac;

typedef struct Vertex2DColor {
    glm::vec2 position;
    glm::vec3 color;

    Vertex2DColor();
    Vertex2DColor(glm::vec2 pos, glm::vec3 col){
        position = pos;
        color = col;
    };

} Vertex2DColor;

int main(int argc, char** argv) {
    // Initialize SDL and open a window
    SDLWindowManager windowManager(800, 600, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    FilePath applicationPath(argv[0]);
    Program program = loadProgram(
        applicationPath.dirPath() + "shaders/triangle.vs.glsl",
        applicationPath.dirPath() + "shaders/triangle.fs.glsl"
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
    int N = 150;    // nombre de triangles
    float r = 0.5;  // rayon
    float theta = 2*glm::pi<float>() / N; 

    std::vector<Vertex2DColor> vertices;

    for (int i = 0; i < N; i++)
    {
        float x1 = r*glm::cos(theta * i);
        float x2 = r*glm::cos(theta * (i+1)); 
        float y1 = r*glm::sin(theta * i); 
        float y2 = r*glm::sin(theta * (i+1));

        vertices.push_back(Vertex2DColor(glm::vec2(0, 0), glm::vec3(1, 0, 1)));
        vertices.push_back(Vertex2DColor(glm::vec2(x1, y1), glm::vec3(1, 0, 0)));
        vertices.push_back(Vertex2DColor(glm::vec2(x2, y2), glm::vec3(1, 0, 0)));
    }

    glBufferData(GL_ARRAY_BUFFER, 3*N * sizeof(Vertex2DColor), vertices.data(), GL_STATIC_DRAW);

    // Débinder le VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Création du VAO 
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // Binding du VAO 
    glBindVertexArray(vao);

   // Ativation des attributs de vertex
    const GLuint VERTEX_ATTR_POSITION = 3;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    const GLuint VERTEX_ATTR_COLOR = 8;
    glEnableVertexAttribArray(VERTEX_ATTR_COLOR);

    // Spécification des attributs de vertex
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // position des points
    glVertexAttribPointer(
        VERTEX_ATTR_POSITION, 
        2, GL_FLOAT, GL_FALSE, 
        5*sizeof(GLfloat), offsetof(Vertex2DColor, position));

    // couleur des points
    glVertexAttribPointer(
        VERTEX_ATTR_COLOR, 
        3, GL_FLOAT, GL_FALSE, 
        5*sizeof(GLfloat), (GLvoid*) offsetof(Vertex2DColor, color));

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
        glDrawArrays(GL_TRIANGLES, 0, N*3);
        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    return EXIT_SUCCESS;
}
