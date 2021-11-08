#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/glm.hpp>
#include <glimac/Image.hpp>

using namespace glimac;

typedef struct Vertex2DUV {
    glm::vec2 position;
    glm::vec2 coordTexture;

    Vertex2DUV();
    Vertex2DUV(glm::vec2 pos, glm::vec2 text){
        position = pos;
        coordTexture = text;
    };

};

glm::mat3 translate(float tx, float ty)
{
  return glm::mat3(glm::vec3(1,  0,  0),
              glm::vec3(0,  1,  0),
              glm::vec3(tx, ty, 1));
}

glm::mat3 scale(float sx, float sy)
{
  return glm::mat3(glm::vec3(sx, 0,  0),
              glm::vec3(0,  sy, 0),
              glm::vec3(0,  0,  1));
}

glm::mat3 rotate(float alpha)
{
  float cosA = cos(alpha);
  float sinA = sin(alpha);

  return glm::mat3(glm::vec3(cosA,  sinA, 0),
              glm::vec3(-sinA, cosA, 0),
              glm::vec3(0, 0, 1));
}

int main(int argc, char** argv) {
    // Initialize SDL and open a window
    SDLWindowManager windowManager(800, 800, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    std::unique_ptr<Image> img = loadImage("../GLImac-Template/assets/textures/EarthMap.jpg");
    if (img)
        std::cout << "texture chargée." << std::endl;


    FilePath applicationPath(argv[0]);
    Program program = loadProgram(
        applicationPath.dirPath() + "shaders/" + argv[1],
        applicationPath.dirPath() + "shaders/" + argv[2]
        );
    program.use();

    GLint uTime = glGetUniformLocation(program.getGLId(), "uModelMatrix");
    std::cout << uTime << std::endl;

    GLint locuColor = glGetUniformLocation(program.getGLId(), "uColor");
    std::cout << "color : " << locuColor << std::endl;

    GLint locTexture = glGetUniformLocation(program.getGLId(), "uTexture");
    std::cout << "texture : " << locTexture << std::endl;
    float i = 0;

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/

    // ================ VBO ================== //

    GLuint vbo; 
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    Vertex2DUV vertices[] = {
        Vertex2DUV(glm::vec2(-1, -1), glm::vec2(0, 1)),
        Vertex2DUV(glm::vec2(1, -1), glm::vec2(1, 1)),
        Vertex2DUV(glm::vec2(0, 1), glm::vec2(0.5, 0))
    };

    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex2DUV), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // ================ VAO ================== //

    GLuint vao; 
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const GLuint VERTEX_ATTR_POSITION = 0;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);

    const GLuint VERTEX_ATTR_COLOR = 1;
    glEnableVertexAttribArray(VERTEX_ATTR_COLOR);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), 0);

    glVertexAttribPointer(VERTEX_ATTR_COLOR, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (GLvoid*) (2*sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ================ TEXTURE ================== //

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 
                0,
                GL_RGBA,
                img->getWidth(),
                img->getHeight(),
                0,
                GL_RGBA,
                GL_FLOAT,
                img->getPixels()
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

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

        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/

        glClear(GL_COLOR_BUFFER_BIT);

        i+=0.01;

        glm::mat3 rotP = rotate(i);
        glm::mat3 rotN = rotate(-i);
        glm::mat3 sca = scale(0.25, 0.25);
        glm::mat3 translate1 = translate(-0.5, -0.5);
        glm::mat3 translate2 = translate(-0.5, 0.5);
        glm::mat3 translate3 = translate(0.5, 0.5);
        glm::mat3 translate4 = translate(0.5, -0.5);

        glm::vec3 color = glm::vec3(0.8, 0.2, 0.4);

        glBindVertexArray(vao);
        glUniform3fv(locuColor, 1, glm::value_ptr(color));

        glBindTexture(GL_TEXTURE_2D, texture); 
        glUniform1i(locTexture, 0);

        glUniformMatrix3fv(uTime, 1, GL_FALSE, glm::value_ptr(translate1*rotN*sca));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUniformMatrix3fv(uTime, 1, GL_FALSE, glm::value_ptr(translate2*rotN*sca));
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glUniformMatrix3fv(uTime, 1, GL_FALSE, glm::value_ptr(translate3*rotN*sca));
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glUniformMatrix3fv(uTime, 1, GL_FALSE, glm::value_ptr(translate4*rotN*sca));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &texture);

    return EXIT_SUCCESS;
}