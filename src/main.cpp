#include <iostream>

#include <glad/glad.h> // ! Keep this import above glfw3 import
#include <GLFW/glfw3.h>

#include "utils/Shader/Shader.h"
#include "utils/Time/Time.h"
#include "utils/Texture/Texture.h"
#include "utils/Camera/Camera.h"

#include "math/Mesh/Mesh.h"
#include "math/Primitives/Primitives.h"
#include "utils/Skybox/Skybox.h"

#include "room.h"

// TODO: create a better mouse input handling system
void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
  static bool first = true;
  static double lastX = 0, lastY = 0;

  if (first) {
    lastX = xPos;
    lastY = yPos;
    first = false;
  }

  float xOffset = (float)(xPos - lastX);
  lastX = xPos;
  float yOffset = (float)(lastY - yPos);
  lastY = yPos;

  Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
  camera->processMouseMovement(xOffset, yOffset, 0.1f);
}
// TODO: create a better keyboard input handling system
void checkKeyboardEvents(GLFWwindow* window, float cameraSpeed, float deltaTime) {
  Camera* camera = (Camera*)glfwGetWindowUserPointer(window);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera->processKeyboard(CameraMovement::Forward, deltaTime, cameraSpeed);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera->processKeyboard(CameraMovement::Backward, deltaTime, cameraSpeed);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera->processKeyboard(CameraMovement::Left, deltaTime, cameraSpeed);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera->processKeyboard(CameraMovement::Right, deltaTime, cameraSpeed);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    camera->processKeyboard(CameraMovement::Up, deltaTime, cameraSpeed);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    camera->processKeyboard(CameraMovement::Down, deltaTime, cameraSpeed);
}

static void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

int main(void) {
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW\n";
    return -1;
  }

  const int SCR_W = 1280;
  const int SCR_H = 720;
  const float SCR_ASPECT = (float)SCR_W / (float)SCR_H;

  window = glfwCreateWindow(SCR_W, SCR_H, "Room", NULL, NULL);
  if (!window) {
    glfwTerminate();
    std::cerr << "Failed to Create Window\n";
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD\n";
    return -1;
  }

  std::cout << "Using OpenGL Driver: " << glGetString(GL_VERSION) << std::endl;

  glEnable(GL_DEPTH_TEST);

  const int cubemapSize = 1024;

  Camera camera(
    glm::vec3(0.0f, 2.0f, 4.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    45.0f,
    SCR_ASPECT
  );
  glfwSetCursorPosCallback(window, mouseCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetWindowUserPointer(window, &camera);

  auto skyboxCube = Primitives::Cube(1.0f);
  Skybox skybox(
    skyboxCube,
    TEXTURES_DIR + std::string("/skybox/qwantani_moon_noon_puresky_1k.hdr"),
    cubemapSize,
    SCR_W,
    SCR_H
  );

  auto roomWidth = 10.f;
  auto roomHeight = 3.f;
  auto roomDepth = 10.f;

  Room room(
    roomWidth,
    roomHeight,
    roomDepth,
    TEXTURES_DIR + std::string("/painted-plaster/diffuse.jpg"),
    TEXTURES_DIR + std::string("/wood-shutter/diffuse.jpg"),
    TEXTURES_DIR + std::string("/granite-tile/diffuse.jpg"),
    true
  );

  Shader roomShader("room");
  roomShader.bind();
  roomShader.setMat4("model", glm::mat4(1.0f));
  roomShader.setFloat("uTile", 0.5f);
  roomShader.setVec3("uRoomCenter", glm::vec3(0.0f, roomHeight * 0.5f, 0.0f));
  roomShader.setVec3("uHalfSize", glm::vec3(
    roomWidth * 0.5f,
    roomHeight * 0.5f,
    roomDepth * 0.5f
  ));

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    Time::update();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    camera.setAspect((float)width / (float)height);

    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // if (shader.reloadIfChanged()) {
    //   std::cout << "Shader reloaded OK\n";
    // }

    skybox.draw(camera);

    roomShader.bind();
    roomShader.setMat4("view", camera.getViewMatrix());
    roomShader.setMat4("projection", camera.getProjectionMatrix());

    room.draw(roomShader);

    float cameraSpeed = 3.0f;
    checkKeyboardEvents(window, cameraSpeed, Time::deltaTime);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}