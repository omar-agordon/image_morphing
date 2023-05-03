#define STB_IMAGE_IMPLEMENTATION
#include <glad.h>
#include <GLFW/glfw3.h>
#include <typeinfo>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader_s.h>
using std::cerr;
using std::endl;
#include <fstream>
using std::ofstream;
#include <cstdlib>
#include "delaunay.h"


//fucntion declarations
void init_glfw();
void framebuffer_size_callback(GLFWwindow*, int, int);
void processInput(GLFWwindow* window);

//manage user inputs
void glfw_callback_key_(GLFWwindow*, int, int, int, int);
void glfw_callback_button(GLFWwindow*, int, int, int);

//saving to files
int save_coords_to_file_s();
int save_coords_to_file_d();

//binding to triangles
void bindVertexToVAO_s(unsigned int& VAO, unsigned int& VBO);
void bindVertexToVAO_d(unsigned int& VAO3, unsigned int& VBO3);

//loading files
int load_s();
int load_d();

int s=0; // loop index
int d = 0;
double x_coord_s[50];
double y_coord_s[50];// list of coords for both images
double x_coord_d[50];
double y_coord_d[50];
int p1[50];
int p2[50];//list of delaunay triangle vertexs
int p3[50];
int d1[50];
int d2[50];
int d3[50];
int triangle_amount;
int draw_s = 0;
int draw_d = 0;




int main()
{
	init_glfw();
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(600, 600, "Morphing Program", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

 

   

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
    std::printf("RENDERER %s\n", glGetString(GL_RENDERER));
    std::printf("VENDOR %s\n", glGetString(GL_VENDOR));
    std::printf("VERSION %s\n", glGetString(GL_VERSION));
    std::printf("SHADING_LANGUAGE_VERSION %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glViewport(0, 0, 600, 600);
    //shaders for textures and display triagualtion trinagles
    Shader image1Shader("5.1.transform1.vs", "5.1.transform1.fs");
    Shader image2Shader("5.1.transform2.vs", "5.1.transform2.fs");
    Shader triagShader("6.1.coordinate_systems.vs", "6.1.coordinate_systems.fs");
   
    float vertices[] = {
        // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
    };

   
    //triangles to represent the two triangles for rectangle image
    unsigned int indices[] = {
        0, 1, 3, 
        1, 2, 3  
    };
    //vbo,vao and ebo 
    unsigned int VBO; 
    unsigned int VAO;
    unsigned int EBO;
    //binding
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    //texture laoding
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    //texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image using stbi_load
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("textures/image1.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    //texture 2
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    //texture wrapping parameter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set texture filtering parameters 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("textures/image2.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    //call shader programs
    image1Shader.use();
    image1Shader.setInt("texture1", 0);
    image1Shader.use();
    image1Shader.setInt("texture2", 0);

    glfwSetKeyCallback(window, &glfw_callback_key_);
    glfwSetMouseButtonCallback(window, glfw_callback_button);

  //vao and vbos for triangulation 
    unsigned int VBO2, VAO2;
    unsigned int VBO3, VAO3;

	while (!glfwWindowShouldClose(window))
	{
        processInput(window);

        // screen rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

     //set back to fill mode for the texturing display
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        //bind first texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
       
        glm::mat4 transform = glm::mat4(1.0f); // transformation matrix
        transform = glm::translate(transform, glm::vec3(0.5f, 0.5f, 0.0f));
        image1Shader.use();
        unsigned int transformLoc = glGetUniformLocation(image1Shader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    
        //draw texture 1
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //bind texture 2
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glm::mat4 transform2 = glm::mat4(1.0f); 
        //transformation is used to stretch images to same size to fit screen
        transform2 = glm::translate(transform2, glm::vec3(-0.5f, 0.5f, 0.0f));
        image2Shader.use();
        unsigned int transformLoc2 = glGetUniformLocation(image2Shader.ID, "transform");
        glUniformMatrix4fv(transformLoc2, 1, GL_FALSE, glm::value_ptr(transform2));
        
        //draw texture 2
        glBindVertexArray(VAO); 
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //condition drawing triangulation off button press
        if (draw_s==1) {
            //call bind function to craete buffers for triangle meshes 
            bindVertexToVAO_s(VAO2, VBO2);
            triagShader.use();
            glm::mat4 projection = glm::mat4(1.0f);       
            projection = glm::ortho(0.0f, 600.0f, 600.0f, 0.0f, -1.0f, 1.0f);
            triagShader.setMat4("projection", projection);

            glBindVertexArray(VAO2);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawArrays(GL_TRIANGLES, 0, 120);

        }
        //restrict the same on destiantion image triangualtion
        if (draw_d == 1) {
           
            bindVertexToVAO_d(VAO3, VBO3);
            triagShader.use();
            glm::mat4 projection = glm::mat4(1.0f);
            projection = glm::ortho(0.0f, 600.0f, 600.0f, 0.0f, -1.0f, 1.0f);
            triagShader.setMat4("projection", projection);

            glBindVertexArray(VAO3);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawArrays(GL_TRIANGLES, 0, 120);

        }
 
        //save coords to files in the loop 
        //dynamically retrieve them 
        save_coords_to_file_s();
        save_coords_to_file_d();

		glfwSwapBuffers(window);
		glfwPollEvents();
		

	}
    //delete buffers
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteVertexArrays(1, &VAO3);
    glDeleteBuffers(1, &VBO3);
    glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;

}

void init_glfw()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//function for dynamically retrieving control points from user
void glfw_callback_button(GLFWwindow* aWindow, int aButton, int aAction, int)
{
    double xpos, ypos;
    //left for source
    if (aButton == GLFW_MOUSE_BUTTON_LEFT && aAction == GLFW_PRESS)
    {
        
       
        glfwGetCursorPos(aWindow, &xpos, &ypos);
        x_coord_s[s] = xpos;
        y_coord_s[s] = ypos;
        s = s + 1;
        std::printf(" %f\n", xpos);
        std::printf(" %f\n", ypos);
        std::printf("\n");
        

    }
    //right for destination 
    else if (aButton == GLFW_MOUSE_BUTTON_RIGHT && aAction == GLFW_PRESS)
        {
           

            glfwGetCursorPos(aWindow, &xpos, &ypos);
            x_coord_d[d] = xpos;
            y_coord_d[d] = ypos;
            d = d + 1;
            std::printf(" %f\n", xpos);
            std::printf(" %f\n", ypos);
            std::printf("\n");


        
    }


}

//function save to source co-ordinate file 
int save_coords_to_file_s() {
    ofstream outdata; // outdata is like cin
    
    
    outdata.open("source_nodes.txt"); // opens the file
    if (!outdata) { // file couldn't be opened
        cerr << "Error: file could not be opened" << endl;
        exit(1);
    }

    outdata << "#  Spatial dimension M = 2" << endl;
    outdata << "#  Spatial dimension N = " << s << endl;

    for (int x = 0; x < s; ++x)
        outdata << x_coord_s[x] << " "<< y_coord_s[x] << endl;

        
    outdata.close();

    return 0;

}

//function save to destiantion co-ordinate file 
int save_coords_to_file_d() {
    ofstream outdata; // outdata is like cin


    outdata.open("destination_nodes.txt"); // opens the file
    if (!outdata) { // file couldn't be opened
        cerr << "Error: file could not be opened" << endl;
        exit(1);
    }
    //produce information needed for Delaunay library call 
    outdata << "#  Spatial dimension M = 2" << endl;
    outdata << "#  Spatial dimension N = " << d << endl;

    for (int x = 0; x < d; ++x)
        outdata << x_coord_d[x] << " " << y_coord_d[x] << endl;


    outdata.close();

    return 0;

}

//fucntion to catch user inputs for usability
void glfw_callback_key_(GLFWwindow* aWindow, int aKey, int, int aAction, int)
{
    //s triggers triangualtion of control points for source image
    if (GLFW_KEY_S== aKey && GLFW_PRESS == aAction)
    {
        triangulate("source");
        //reads the file produced by Delaunay triangualtion 
        load_s();
        //sets global to true to allow for binding of vaos and vbos for displayign the triangualtion mesh
        draw_s = 1;
        return;
    } 
    //D triggers triangualtion of control points for destination image
    else if (GLFW_KEY_D == aKey && GLFW_PRESS == aAction)
    {
        triangulate("destination");
        //reads the file produced by Delaunay triangualtion 
        load_d();
        draw_d = 1;
        return;
    }
}

//functions for reading in Delaunay triangulation files
int load_s()
{
    ifstream inFile;
    int n = 0;

    inFile.open("source_elements.txt");

    if (inFile.is_open()) {
        while (inFile >> p1[n] >> p2[n] >> p3[n]) {
            n++;
            triangle_amount++;
        }

        inFile.close();

       
  
        return 0;
    }
    else {
        cout << "Error opening output file" << endl;
        return -1; 
    } 
}

int load_d()
{
    ifstream inFile;
    int n = 0;

    inFile.open("destination_elements.txt");

    if (inFile.is_open()) {
        while (inFile >> d1[n] >> d2[n] >> d3[n]) {
            n++;
        }

        inFile.close();

        

        return 0;
    }
    else {
        cout << "Error opening output file" << endl;
        return -1;
    }
}


//bind vertexs to vaos
void bindVertexToVAO_s(unsigned int& VAO, unsigned int& VBO) {
float v1x;
float v1y;
float v2x;
float v2y;
float v3x;
float v3y;

//have to set a maximum capacity on the amount of trinangles that can be produced and so the amount of control points allowed
//i tried to make the buffer dynamic but it failed to load into the vbo
float triag_vertices[360] = {};

int index3 = 0;
for (int i = 0; i < triangle_amount; i++) {
    //conversions from doubles to float necessary
    //read the corresponding coordinate to the Delaunay elements file output and do so for each line 
    v1x = (float)x_coord_s[p1[i] - 1];
    v1y = (float)y_coord_s[p1[i] - 1];
    v2x = (float)x_coord_s[p2[i] - 1];
    v2y = (float)y_coord_s[p2[i] - 1];
    v3x = (float)x_coord_s[p3[i] - 1];
    v3y = (float)y_coord_s[p3[i] - 1];

    triag_vertices[index3] = v1x;
    index3++;
    triag_vertices[index3] = v1y;
    index3++;
    triag_vertices[index3] = 0.0f;
    index3++;

    triag_vertices[index3] = v2x;
    index3++;
    triag_vertices[index3] = v2y;
    index3++;
    triag_vertices[index3] = 0.0f;
    index3++;

    triag_vertices[index3] = v3x;
    index3++;
    triag_vertices[index3] = v3y;
    index3++;
    triag_vertices[index3] = 0.0f;
    index3++;

    
}
 


glGenVertexArrays(1, &VAO);
glGenBuffers(1, &VBO);
glBindVertexArray(VAO);

glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(triag_vertices), triag_vertices, GL_STATIC_DRAW);

glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

glBindBuffer(GL_ARRAY_BUFFER, 0);

glBindVertexArray(0);

}

//The same as the function  bindVertexToVAO_s fucntion except for the destiantion image
void bindVertexToVAO_d(unsigned int& VAO3, unsigned int& VBO3) {
    float v1x;
    float v1y;
    float v2x;
    float v2y;
    float v3x;
    float v3y;


    float triag_vertices[360] = {};
    //use destiantion image variables instead
    int index3 = 0;
    for (int i = 0; i < triangle_amount; i++) {
        v1x = (float)x_coord_d[d1[i] - 1];
        v1y = (float)y_coord_d[d1[i] - 1];
        v2x = (float)x_coord_d[d2[i] - 1];
        v2y = (float)y_coord_d[d2[i] - 1];
        v3x = (float)x_coord_d[d3[i] - 1];
        v3y = (float)y_coord_d[d3[i] - 1];

        triag_vertices[index3] = v1x;
        index3++;
        triag_vertices[index3] = v1y;
        index3++;
        triag_vertices[index3] = 0.0f;
        index3++;

        triag_vertices[index3] = v2x;
        index3++;
        triag_vertices[index3] = v2y;
        index3++;
        triag_vertices[index3] = 0.0f;
        index3++;

        triag_vertices[index3] = v3x;
        index3++;
        triag_vertices[index3] = v3y;
        index3++;
        triag_vertices[index3] = 0.0f;
        index3++;


    }



    glGenVertexArrays(1, &VAO3);
    glGenBuffers(1, &VBO3);
    glBindVertexArray(VAO3);

    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triag_vertices), triag_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

}
