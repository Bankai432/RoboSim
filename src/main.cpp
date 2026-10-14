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
#include <algorithm>
#include "simulation/Agent.h"

void framebuffer_size_callback(GLFWwindow* window,int width,int height){
    glViewport(0,0,width,height);
}

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
    int width,height;
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwGetFramebufferSize(window, &width, &height);
    framebuffer_size_callback(window, width, height);
    glEnable(GL_DEPTH_TEST);

    Environment env;

    glm::mat4 groundModel=glm::mat4(1.0f);
    groundModel=glm::translate(groundModel,glm::vec3(0.0f,-0.05f,0.0f));
    groundModel=glm::scale(groundModel,glm::vec3(40.0f,0.1f,40.0f));
    glm::mat4 projection=glm::perspective(glm::radians(45.0f),(float)width/(float)height,0.1f,100.0f);
    
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
    

    GLuint rayVAO,rayVBO,rayNormalVBO;
    glGenVertexArrays(1,&rayVAO);
    glGenBuffers(1,&rayVBO);
    glGenBuffers(1,&rayNormalVBO);

    glBindVertexArray(rayVAO);

    glBindBuffer(GL_ARRAY_BUFFER,rayVBO);
    glBufferData(GL_ARRAY_BUFFER,10*3*sizeof(float),nullptr,GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    float rayNormals[30];
    for (int i=0;i<10;i++) {
        rayNormals[i*3+0]=0.0f;
        rayNormals[i*3+1]=1.0f;
        rayNormals[i*3+2]=0.0f;
    }

    glBindBuffer(GL_ARRAY_BUFFER,rayNormalVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(rayNormals),rayNormals,GL_STATIC_DRAW);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);


    Shader shader("shaders/rect.vert","shaders/rect.frag");

    const std::vector<Obstacle>& obstacles=env.getObstacles();
    glm::vec3 offset(8.0f,8.0f,8.0f);
    glm::vec3 cameraPosition=env.getRobot().position+offset;
    const float cameraSpeed=4.0f;

    const float SIM_DT=0.01f;
    float accumulator=0.0f;

    Agent agent;

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
        deltaTime=std::min(deltaTime,0.25f);

        accumulator+=deltaTime;
        while(accumulator>=SIM_DT){
            Observation obs=env.observe();
            Action agentAction=agent.chooseAct(obs);
            env.step(agentAction,SIM_DT);
            accumulator-=SIM_DT;
            //env.step(act,SIM_DT);
            //accumulator-=SIM_DT;
        }

        Observation obs=env.observe();

        const auto& position=env.getRobot().position;
        const auto& targetposition=env.getTarget().position;
        
        constexpr float MAX_RAY_RANGE=5.0f;
        const float rayAngles[5]={
            -1.5707963f,  // Left
            -0.7853982f,  // Front-left
            0.0f,        // Forward
            0.7853982f,  // Front-right
            1.5707963f   // Right
        };

        float rayVertices[30];

        for(int i=0;i<5;i++){
            const float distance=obs.rayDistances[i];
            const float angle=env.getRobot().yaw+rayAngles[i];
            const int start=i*6;

            rayVertices[start+0]=position.x;
            rayVertices[start+1]=position.y+0.15f;
            rayVertices[start+2]=position.z;

            rayVertices[start+3]=position.x+std::sin(angle)*distance;
            rayVertices[start+4]=position.y+0.15f;
            rayVertices[start+5]=position.z-std::cos(angle)*distance;
        }

        glBindBuffer(GL_ARRAY_BUFFER,rayVBO);
        glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(rayVertices),rayVertices);
        glBindBuffer(GL_ARRAY_BUFFER,0);

        glm::vec3 desiredCameraPosition=position+offset;
        float smoothing=1.0f-std::exp(-cameraSpeed*deltaTime);
        cameraPosition=glm::mix(cameraPosition,desiredCameraPosition,smoothing);
        glm::mat4 view=glm::lookAt(cameraPosition,position,glm::vec3(0.0f,1.0f,0.0f));

        glm::mat4 targetmodel=glm::mat4(1.0f);
        targetmodel=glm::translate(targetmodel,glm::vec3(targetposition.x,0.5f,targetposition.z));
        targetmodel=glm::scale(targetmodel,glm::vec3(0.1f,1.0f,0.1f));

        glm::mat4 model=glm::mat4(1.0f);
        model=glm::translate(model,glm::vec3(position.x,0.5f,position.z));
        model=glm::rotate(model,(-1)*env.getRobot().yaw,glm::vec3(0.0f,1.0f,0.0f));
        


        glfwGetFramebufferSize(window, &width, &height);
        if(height>0) projection=glm::perspective(glm::radians(45.0f),(float)width/(float)height,0.1f,100.0f);


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

        GLint colorLoc=glGetUniformLocation(shader.ID,"objectColor");
        GLint modelLoc=glGetUniformLocation(shader.ID,"model");
        GLint viewLoc=glGetUniformLocation(shader.ID,"view");
        GLint projectionLoc=glGetUniformLocation(shader.ID,"projection");

        glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc,1,GL_FALSE,glm::value_ptr(projection));

        glBindVertexArray(VAO);

        glUniform3f(colorLoc,0.9f,0.9f,0.9f);
        glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(groundModel));
        glDrawArrays(GL_TRIANGLES,0,36);

        if(env.isSuccess()) glUniform3f(colorLoc,0.0f,1.0f,0.0f);
        else if(env.isCollision()) glUniform3f(colorLoc,1.0f,0.0f,0.0f);
        else glUniform3f(colorLoc,0.3f,0.5f,0.5f);

        glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES,0,36);

        glUniform3f(colorLoc,1.0f,0.1f,0.1f);
        glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(targetmodel));
        glDrawArrays(GL_TRIANGLES,0,36);
        
        for(const auto& obstacle : obstacles) {
            glm::mat4 obstaclemodel=glm::mat4(1.0f);
            obstaclemodel=glm::translate(obstaclemodel,obstacle.position);
            obstaclemodel=glm::scale(obstaclemodel,obstacle.size);
        
            glUniform3f(colorLoc,0.9f,0.5f,0.4f);
            glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(obstaclemodel));
            glDrawArrays(GL_TRIANGLES,0,36);    
        }

        glBindVertexArray(rayVAO);

        glm::mat4 identityModel(1.0f);
        glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(identityModel));
        glLineWidth(2.0f);

        for(int i=0;i<5;i++){
            if(obs.rayDistances[i]<MAX_RAY_RANGE-0.01f) glUniform3f(colorLoc,1.0f,0.2f,0.2f);
            else glUniform3f(colorLoc, 0.2f, 1.0f, 0.3f);

            glDrawArrays(GL_LINES,i*2,2);
        }
        glLineWidth(3.0f);
        glBindVertexArray(0);

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }
    std::cout<<'\n';
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
