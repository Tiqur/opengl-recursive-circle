#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "include/glad/glad.h"
#include "include/stb_image.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"
#include "lib/shader.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <string>
#include <vector>



float zoom = 1.0f;
std::vector<float> vertices2;
std::vector<int> indices2;

void setVerticesAndIndices(std::vector<float> *vertices, std::vector<int> *indices, int delta)
{
  // Clear vectors
  vertices->clear();
  indices->clear();

  // Add 0, 0 ( center of "circle" )
  vertices->push_back(0.0f);
  vertices->push_back(0.0f);

  // Circle size
  const float radius = 0.7;

  // Convert degrees to radians
  float rad1 = std::atan2(radius, 0);

  for (int i = 1; i < delta+1; i++)
  {
    // Degrees to rotate
    const float deg = (360.0/delta)*i;
    //std::cout << deg << std::endl;

    // Convert degrees to radians and add to original radian amount
    const float rad2 = rad1-deg*(M_PI/180.0);

    // Convert back to Cartesian coordinates ( and round to 4th decimal )
    const float q = sqrt(radius*radius);
    const float x = q * round(cos(rad2)*10000)/10000;
    const float y = q * round(sin(rad2)*10000)/10000;

    // Append vertices
    vertices->push_back(x);
    vertices->push_back(y);

    // Append indices
    indices->push_back(i);
    indices->push_back(i < delta ? i + 1 : i%delta+1);
    indices->push_back(0);
  }
};

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
  zoom += yOffset * 0.01f; 
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

int main()
{
    // Initialize GLFW
    glfwInit();

    // Set the version of OpenGL to use 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window with 800x800 dimensions, named "Test"
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "Test", NULL, NULL);

    glfwSetScrollCallback(window, scroll_callback);

    // Check if the window was created successfully
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set the window as the current OpenGL context
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    gladLoadGL();

    // Set the viewport to the size of the window
    glViewport(0, 0, 800, 800);

    // Resize buffer on window resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // Generate VAO, VBO, and EBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind VAO and VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copy vertex data to VBO
    glBufferData(GL_ARRAY_BUFFER, vertices2.size()*sizeof(float), vertices2.data(), GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // Copy indices data to EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size()*sizeof(int), indices2.data(), GL_STATIC_DRAW);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Specify vertex attribute pointers
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VAO
    glBindVertexArray(0);

    // Load shaders
    Shader shader{"shaders/vertex.glsl", "shaders/fragment.glsl"};


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    float color1[4] = {0.0f, 1.0f, 0.8f, 1.0f};
    float color2[4] = {0.8f, 0.4f, 0.8f, 1.0f};
    int triangle_count = 2;

    bool spinX = false;
    bool spinY = false;
    bool spinZ = false;

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // The main loop of the program
    while(!glfwWindowShouldClose(window)) {
      // Process user input
      processInput(window);

      // Generate verticies and indices
      setVerticesAndIndices(&vertices2, &indices2, triangle_count*2);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, vertices2.size()*sizeof(float), vertices2.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size()*sizeof(int), indices2.data(), GL_STATIC_DRAW);

      // Clear screen with color
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // user shader
      shader.use();
      glBindVertexArray(VAO);

      // Create transformation matrix + rotate
      glm::mat4 trans = glm::mat4(1.0f);
      // Scale down
      trans = glm::scale(trans, glm::vec3(0.8f, 0.8f, 0.8f));
      // Spin
      if (spinX || spinY || spinZ)
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(1.0f*spinX, 1.0f*spinY, 1.0f*spinZ));
      // Pass to uniform in shader
      unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
      glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

      // Draw triangles
      for (int i = 0; i < indices2.size()/3; i++)
      {
        if (i % 2)
          shader.set4f("color", color1[0], color1[1], color1[2], color1[3]);
        else
          shader.set4f("color", color2[0], color2[1], color2[2], color2[3]);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3*i*sizeof(unsigned int)));
      }

      glBindVertexArray(0);


      ImGui::Begin("GUI");
      ImGui::Text("Triangle Circle Thing");
      ImGui::ColorEdit4("Color 1", color1);
      ImGui::ColorEdit4("Color 2", color2);
      ImGui::SliderInt("Triangles", &triangle_count, 2, 100);
      ImGui::Checkbox("Spin X", &spinX);
      ImGui::Checkbox("Spin Y", &spinY);
      ImGui::Checkbox("Spin Z", &spinZ);
      ImGui::End();

      ImGui::Begin("Vertices");
      std::string str;
      for (int i = 0; i < vertices2.size(); i+=2)
        str += std::to_string(vertices2[i]) += ", " + std::to_string(vertices2[i+1]) += "\n";
      ImGui::Text(str.c_str());
      ImGui::End();

      ImGui::Begin("Indices");
      str = "";
      for (int i = 0; i < indices2.size(); i+=3)
        str += std::to_string(indices2[i]) += ", " + std::to_string(indices2[i+1]) += ", " + std::to_string(indices2[i+2]) += "\n";
      ImGui::Text(str.c_str());
      ImGui::End();


      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      
      // Swap buffer
      glfwSwapBuffers(window);
      glfwPollEvents();
    }

    // End
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
