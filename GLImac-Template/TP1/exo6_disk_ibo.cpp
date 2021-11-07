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

    vertices.push_back(Vertex2DColor(glm::vec2(0, 0), glm::vec3(1, 0, 1)));
    for (int i = 0; i < N; i++)
    {
        float x1 = r*glm::cos(theta * i);
        float y1 = r*glm::sin(theta * i); 
        vertices.push_back(Vertex2DColor(glm::vec2(x1, y1), glm::vec3(1, 0, 0)));
    }

    vertices.push_back(Vertex2DColor(glm::vec2(0, 0), glm::vec3(1, 0, 0)));
    vertices.push_back(Vertex2DColor(   glm::vec2(  r*glm::cos(theta * N), 
                                                    r*glm::sin(theta * N)),
                                        glm::vec3(1, 0, 0)));

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2DColor), vertices.data(), GL_STATIC_DRAW);

    // Débinder le VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // => Creation du IBO
    GLuint ibo;
    glGenBuffers(1, &ibo);

    // => On bind sur GL_ELEMENT_ARRAY_BUFFER, cible reservée pour les IBOs
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // => Tableau d'indices: ce sont les indices des sommets à dessiner
    // Chaque indice correspond au sommet correspondant dans le VBO

    std::vector<uint32_t> indices;
    for (int i = 0; i < N; i++) {
        indices.push_back(0);
        indices.push_back(i+1);
        indices.push_back(i+2);
    }
    indices.push_back(0);
    indices.push_back(N+1);
    indices.push_back(N+2);

    // => On remplit l'IBO avec les indices:
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    // => Comme d'habitude on debind avant de passer à autre chose
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Création du VAO 
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // Binding du VAO 
    glBindVertexArray(vao);

    // => On bind l'IBO sur GL_ELEMENT_ARRAY_BUFFER; puisqu'un VAO est actuellement bindé,
    // cela a pour effet d'enregistrer l'IBO dans le VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

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
        sizeof(Vertex2DColor), offsetof(Vertex2DColor, position));

    // couleur des points
    glVertexAttribPointer(
        VERTEX_ATTR_COLOR, 
        3, GL_FLOAT, GL_FALSE, 
        sizeof(Vertex2DColor), (GLvoid*) offsetof(Vertex2DColor, color));

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
        
        // => On utilise glDrawElements à la place de glDrawArrays
        // Cela indique à OpenGL qu'il doit utiliser l'IBO enregistré dans le VAO
        glDrawElements(GL_TRIANGLES, N*3, GL_UNSIGNED_INT, 0);
        
        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    return EXIT_SUCCESS;
}
