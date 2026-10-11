#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include "simulation/environment.h"
#include <iomanip>
#include "rendering/shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main(){
    if (!glfwInit()){
        std::cerr<<"Failed to initialize GLFW\n";
        return 1;
    }

    GLFWwindow* window=glfwCreateWindow(800,800,"Black Hole Renderer",nullptr,nullptr);

    if(!window){
        std::cerr<<"Failed to create GLFW window\n";
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr<<"Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    glViewport(0,0,800,800);
    glEnable(GL_DEPTH_TEST);

    Environment env;
    glm::mat4 model=glm::mat4(1.0f);
    glm::mat4 view=glm::lookAt(
        glm::vec3(4.0f,4.0f,4.0f),
        glm::vec3(0.0f,0.0f,0.0f),  
        glm::vec3(0.0f,1.0f,0.0f)  
    );

    glm::mat4 projection=glm::perspective(glm::radians(45.0f),800.0f/800.0f,0.1f,100.0f);

    float lastFrame=static_cast<float>(glfwGetTime());
    float printTimer=0.0f;

    float vertices[] = {
        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,

        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,

        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        0.5f,  0.5f,  0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,

        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,

        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,

        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f
    };
        
    float faceNormals[6][3]={
        { 0.0f,  0.0f,  1.0f},
        { 0.0f,  0.0f, -1.0f},
        {-1.0f,  0.0f,  0.0f},
        { 1.0f,  0.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f} 
    };

    float normals[108];
    for(int face=0;face<6;face++){
        for(int vertex=0;vertex<6;vertex++){
            for(int component=0;component<3;component++){
                normals[face*18+vertex*3+component]=faceNormals[face][component];
            }
        }
    }

    GLuint VBO, VAO, normalVBO;
    glGenBuffers(1,&VBO);
    glGenVertexArrays(1,&VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);

    glEnableVertexAttribArray(0);
        
    glGenBuffers(1, &normalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);

    glBufferData(GL_ARRAY_BUFFER,sizeof(normals),normals,GL_STATIC_DRAW);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);

    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader shader("shaders/rect.vert","shaders/rect.frag");

    while (!glfwWindowShouldClose(window)){

        glfwPollEvents();

        Action act=Action::STOP;
        if(glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS) act=Action::FORWARD;
        if(glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS) act=Action::LEFT;
        if(glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS) act=Action::RIGHT;
        if(glfwGetKey(window,GLFW_KEY_R)==GLFW_PRESS) env.reset();

        float currFrame=static_cast<float>(glfwGetTime());    
        float deltaTime=currFrame-lastFrame;
        lastFrame=currFrame;
        env.step(act,deltaTime);
        printTimer+=deltaTime;
        if(printTimer>=0.25f){
            const auto& robot=env.getRobot();
            std::cout<<std::fixed<<std::setprecision(2)
                    <<"\rPosition: ("
                    <<robot.position.x<<", "
                    <<robot.position.y<<", "
                    <<robot.position.z<<") Yaw: "
                    <<robot.yaw<<"     "
                    <<std::flush;
                    std::cout<<" | Collision: "<<env.isCollision()
                    <<" | Success: "<<env.isSuccess();
            printTimer = 0.0f;
        }

        glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        GLint modelLoc=glGetUniformLocation(shader.ID,"model");
        GLint viewLoc=glGetUniformLocation(shader.ID,"view");
        GLint projectionLoc=glGetUniformLocation(shader.ID,"projection");

        glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc,1,GL_FALSE,glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES,0,36);
        glfwSwapBuffers(window);
    }
    std::cout<<'\n';
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
