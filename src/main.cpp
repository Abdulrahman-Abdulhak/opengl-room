#include <iostream>

#include <glad/glad.h> // ! Keep this import above glfw3 import
#include <GLFW/glfw3.h>

#include "utils/Shader/Shader.h"
#include "utils/Time/Time.h"

#include "math/Mesh/Mesh.h"

int main(void) {
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Room", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD\n";
    return -1;
  }

  std::cout << "Using OpenGL Driver: " << glGetString(GL_VERSION) << std::endl;

  std::vector<Vertex> verts = {
    {{-0.5f,-0.5f,0.0f}, {1,0,0}},
    {{ 0.5f,-0.5f,0.0f}, {0,1,0}},
    {{ 0.0f, 0.5f,0.0f}, {0,0,1}},
  };

  Mesh tri(verts);
  Shader shader("test");

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    Time::update();

    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    if (shader.reloadIfChanged()) {
      std::cout << "Shader reloaded OK\n";
    }

    shader.bind();
    tri.draw();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}