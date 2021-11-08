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

struct EarthProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uEarthTexture;
    GLint uCloudTexture;

    EarthProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "shaders/multiTex3D.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uEarthTexture = glGetUniformLocation(m_Program.getGLId(), "uEarthTexture");
        uCloudTexture = glGetUniformLocation(m_Program.getGLId(), "uCloudTexture");
    }
};

struct MoonProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture;

    MoonProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl", applicationPath.dirPath() + "shaders/tex3D.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uTexture = glGetUniformLocation(m_Program.getGLId(), "moonTexture");
    }
};


int main(int argc, char** argv) {

    // ============ INITIALISATION =========== //

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

    FilePath applicationPath(argv[0]);
    EarthProgram earthProgram(applicationPath);
    MoonProgram moonProgram(applicationPath);
    
    glEnable(GL_DEPTH_TEST);    // Test de profondeur du GPU 

    glm::mat4 ProjMatrix = glm::perspective(70.f, float(WIDTH/HEIGHT), 0.1f, 100.0f);

    // ======= CHARGEMENT DES IMAGES ========= //
    
    std::unique_ptr<Image> terre = loadImage("/home/adudree/Documents/IMAC2/SI/GLImac-Template/assets/textures/EarthMap.jpg");
    if (terre) std::cout << "Texture Terre chargée." << std::endl;

    std::unique_ptr<Image> lune = loadImage("../GLImac-Template/assets/textures/MoonMap.jpg");
    if (lune) std::cout << "Texture Lune chargée." << std::endl;

    std::unique_ptr<Image> nuage = loadImage("../GLImac-Template/assets/textures/CloudMap.jpg");
    if (lune) std::cout << "Texture Nuage chargée." << std::endl;


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

    glActiveTexture(GL_TEXTURE0);
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);


    // ---------------- NUAGE ----------------- //

    GLuint texNuage;
    glGenTextures(1, &texNuage);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texNuage);

    glTexImage2D(GL_TEXTURE_2D, 
                0,
                GL_RGBA,
                nuage->getWidth(),
                nuage->getHeight(),
                0,
                GL_RGBA,
                GL_FLOAT,
                nuage->getPixels()
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    // ----------------- LUNE ----------------- //

    GLuint texLune;
    glGenTextures(1, &texLune);

    glActiveTexture(GL_TEXTURE0);
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

    glActiveTexture(GL_TEXTURE0);
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
        earthProgram.m_Program.use();

        glUniform1i(earthProgram.uEarthTexture, 0);
        glUniform1i(earthProgram.uCloudTexture, 1);

        glm::mat4 globalMVMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -5));

        glm::mat4 earthMVMatrix = glm::rotate(globalMVMatrix, windowManager.getTime(), glm::vec3(0, 1, 0));
        
        glUniformMatrix4fv(earthProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(earthMVMatrix));
        glUniformMatrix4fv(earthProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(earthMVMatrix))));
        glUniformMatrix4fv(earthProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * earthMVMatrix));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texTerre);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texNuage);

        glDrawArrays(GL_TRIANGLES, 0, nbSommets);

        // Debind des textures 
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0); 
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0); 

        // --------------- LUNES ---------------- //

        moonProgram.m_Program.use();

        glUniform1i(moonProgram.uTexture, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texLune); 

        for (int i = 0; i < 32; i++) {
        
            glm::mat4 moonMVMatrix = glm::rotate(globalMVMatrix, windowManager.getTime() + i, rotAxis[i]);
            moonMVMatrix = glm::translate(moonMVMatrix, glm::vec3(-2, 0, 0));
            moonMVMatrix = glm::scale(moonMVMatrix, glm::vec3(0.2, 0.2, 0.2));
        
            glUniformMatrix4fv(moonProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(moonMVMatrix));
            glUniformMatrix4fv(moonProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(moonMVMatrix))));
            glUniformMatrix4fv(moonProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * moonMVMatrix));
    
            glDrawArrays(GL_TRIANGLES, 0, nbSommets);
        }

        // Debind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &texTerre);
    glDeleteTextures(1, &texLune);
    glDeleteTextures(1, &texNuage);

    return EXIT_SUCCESS;
}