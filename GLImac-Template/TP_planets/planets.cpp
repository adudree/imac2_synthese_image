#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/glm.hpp>
#include <glimac/Image.hpp>
#include <glimac/Sphere.hpp>
#include <vector>

using namespace glimac;

int main(int argc, char** argv) {
    // Initialize SDL and open a window
    GLint WIDTH = 800;
    GLint HEIGHT = 800;

    SDLWindowManager windowManager(WIDTH, HEIGHT, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    // ============== VERSION ================ //

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;


    // ======= CHARGEMENT DES SHADERS ======== //
    
    std::unique_ptr<Image> terre = loadImage("/home/adudree/Documents/IMAC2/SI/GLImac-Template/assets/textures/EarthMap.jpg");
    if (terre) std::cout << "Texture Terre chargée." << std::endl;

    std::unique_ptr<Image> lune = loadImage("../GLImac-Template/assets/textures/MoonMap.jpg");
    if (lune) std::cout << "Texture Lune chargée." << std::endl;


    FilePath applicationPath(argv[0]);
    Program program = loadProgram(
        applicationPath.dirPath() + "shaders/" + argv[1],
        applicationPath.dirPath() + "shaders/" + argv[2]
        );
    program.use();

    GLint locMVPMatrix = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
    GLint locMVMatrix = glGetUniformLocation(program.getGLId(), "uMVMatrix");
    GLint locNormalMatrix = glGetUniformLocation(program.getGLId(), "uNormalMatrix");
    GLint locTexture = glGetUniformLocation(program.getGLId(), "uTexture");
    
    glEnable(GL_DEPTH_TEST);    // Test de profondeur du GPU 

    glm::mat4 ProjMatrix; 
    glm::mat4 MVMatrix;
    glm::mat4 NormalMatrix;

        ProjMatrix = glm::perspective(70.f, float(WIDTH/HEIGHT), 0.1f, 100.0f);

    // ================ SPHERE ================ //

    Sphere mySphere = Sphere(1, 32, 16);
    int nbSommets = mySphere.getVertexCount(); 
    const ShapeVertex *tabSommets = mySphere.getDataPointer();

    // ================= VBO ================== //

    GLuint vbo; 
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, nbSommets * sizeof(ShapeVertex), tabSommets, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // ================= VAO ================== //
    
    GLuint vao; 
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_NORMAL = 1;
    const GLuint VERTEX_ATTR_TEX_COORD = 2;

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_TEX_COORD);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);
    glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat)));
    glVertexAttribPointer(VERTEX_ATTR_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*) (6 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // =============== TEXTURE ================ //

    // ---------------- TERRE ----------------- //

    GLuint texTerre;
    glGenTextures(1, &texTerre);
    glBindTexture(GL_TEXTURE_2D, texTerre);

    glTexImage2D(GL_TEXTURE_2D, 
                0,
                GL_RGBA,
                terre->getWidth(),
                terre->getHeight(),
                0,
                GL_RGBA,
                GL_FLOAT,
                terre->getPixels()
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // ----------------- LUNE ----------------- //

    GLuint texLune;
    glGenTextures(1, &texLune);
    glBindTexture(GL_TEXTURE_2D, texLune);

    glTexImage2D(GL_TEXTURE_2D, 
                0,
                GL_RGBA,
                lune->getWidth(),
                lune->getHeight(),
                0,
                GL_RGBA,
                GL_FLOAT,
                lune->getPixels()
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);



    // =========== APPLICATION LOOP =========== //

    bool done = false;

    std::vector<glm::vec3> rotAxis; 
    for (int i = 0; i < 32; i++) {
        rotAxis.push_back(glm::sphericalRand(5.f));
    }

    while(!done) {
        // Event loop:
        SDL_Event e;
        while(windowManager.pollEvent(e)) {
            if(e.type == SDL_QUIT) {
                done = true; // Leave the loop
            }
        }

        // ============ RENDERING CODE ============ //
    
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glBindVertexArray(vao);

        // --------------- PLANETE ---------------- //
        MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));
        MVMatrix = glm::rotate(MVMatrix, windowManager.getTime(), glm::vec3(0, 1, 0));
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        glUniformMatrix4fv( locMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix));
        glUniformMatrix4fv( locMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv( locNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        
        glUniform1i(locTexture, 0);

        glBindTexture(GL_TEXTURE_2D, texTerre); 

        glDrawArrays(GL_TRIANGLES, 0, nbSommets);
        glBindTexture(GL_TEXTURE_2D, 0);

        // --------------- LUNES ---------------- //

        glBindTexture(GL_TEXTURE_2D, texLune); 
        glUniform1i(locTexture, 0);

        for (int i = 0; i < 32; i++) {
            glm::mat4 moonMVMatrix, moonNormalMatrix;
            moonMVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));
            moonMVMatrix = glm::rotate(moonMVMatrix, windowManager.getTime() + i, rotAxis[i]);
            moonMVMatrix = glm::translate(moonMVMatrix, glm::vec3(-2, 0, 0));
            moonMVMatrix = glm::scale(moonMVMatrix, glm::vec3(0.2, 0.2, 0.2));
            
            moonNormalMatrix = glm::transpose(glm::inverse(moonMVMatrix));

            glUniformMatrix4fv(locMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*moonMVMatrix));
            glUniformMatrix4fv(locMVMatrix, 1, GL_FALSE, glm::value_ptr(moonMVMatrix));
            glUniformMatrix4fv(locNormalMatrix, 1, GL_FALSE, glm::value_ptr(moonNormalMatrix));

            glDrawArrays(GL_TRIANGLES, 0, nbSommets);        

        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Update the display
        windowManager.swapBuffers();
    }
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &texTerre);
    glDeleteTextures(1, &texLune);

    return EXIT_SUCCESS;
}
