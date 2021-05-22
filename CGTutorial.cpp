/*
* Pflichtteil
Constantin Schulte-Kersmecke
572847
*/

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "objects.hpp"
#include "objloader.hpp"
#include "texture.hpp"

glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;
GLuint programID;

glm::vec3 camera(0);
glm::vec4 robot(0);

void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:

		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_X:
		camera.x += 2.0f;
		break;
	case GLFW_KEY_Y:
		camera.y += 2.0f;
		break;
	case GLFW_KEY_Z:
		camera.z += 2.0f;
		break;
	case GLFW_KEY_A:
		robot.x += 2.0f;
		break;
	case GLFW_KEY_S:
		robot.y += 2.0f;
		break;
	case GLFW_KEY_D:
		robot.z += 2.0f;
		break;
	case GLFW_KEY_F:
		robot.w += 2.0f;
		break;
	default:
		break;
	}
}

void sendMVP()
{
	glm::mat4 MVP = Projection * View * Model;
	glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "M"), 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "V"), 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "P"), 1, GL_FALSE, &Projection[0][0]);

}
void drawCS() {
	glm::mat4 Save = Model;
	Model = glm::scale(Save, glm::vec3(2, 0.01, 0.01));
	sendMVP();
	drawCube();
	Model = glm::scale(Save, glm::vec3(0.01, 2, 0.01));
	sendMVP();
	drawCube();
	Model = glm::scale(Save, glm::vec3(0.01, 0.01, 2));
	sendMVP();
	drawCube();
	Model = Save;
}

void drawSeg(float h)
{
	glm::mat4 Save = Model;
	Model = glm::translate(Model, glm::vec3(0, h, 0));
	Model = glm::scale(Model, glm::vec3(h / 3.0, h, h / 3.0));
	sendMVP();
	drawSphere(10, 10);
	Model = Save;
}

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}

	glfwSetErrorCallback(error_callback);

	GLFWwindow* window = glfwCreateWindow(1024, 768, "CG - Tutorial  - Constantin Schulte-Kersmecke (572847)", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glewExperimental = true;


	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);
	glClearColor(15.0f/255.0f, 105.0f / 255.0f, 166.0f / 255.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

	glUseProgram(programID);

	// LOAD MODEL
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	loadOBJ("teapot.obj", vertices, uvs, normals);

	GLuint Texture = loadBMP_custom("mandrill.bmp");

	GLuint VertexArrayIDTeapot;
	glGenVertexArrays(1, &VertexArrayIDTeapot);
	glBindVertexArray(VertexArrayIDTeapot);

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint normalBuffer;
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		View = glm::lookAt(glm::vec3(0, 0, -5),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0));

		Model = glm::mat4(1.0f);

		Model = glm::rotate(Model, camera.x, glm::vec3(1.0f, 0, 0));
		Model = glm::rotate(Model, camera.y, glm::vec3(0, 1.0f, 0));
		Model = glm::rotate(Model, camera.z, glm::vec3(0, 0, 1.0f));

		glm::mat4 Save = Model;

		Model = glm::translate(Model, glm::vec3(1.5, 0, 0));
		Model = glm::rotate(Model, -90.0f, glm::vec3(1, 0, 0));
		Model = glm::scale(Model, glm::vec3(1.0 / 1000.0, 1.0 / 1000.0, 1.0 / 1000.0));

		sendMVP();
		Model = Save;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);

		glBindVertexArray(VertexArrayIDTeapot);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		Model = glm::scale(Model, glm::vec3(0.5, 0.5, 0.5));
		Model = glm::rotate(Model, robot.x, glm::vec3(0, 0, 1.0f));
		Model = glm::rotate(Model, robot.w, glm::vec3(0, 1.0f, 0));
		drawSeg(0.5);
		Model = glm::translate(Model, glm::vec3(0, 1.0f, 0));
		Model = glm::rotate(Model, robot.y, glm::vec3(0, 0, 1.0f));
		drawSeg(0.4);
		Model = glm::translate(Model, glm::vec3(0, 0.8f, 0));
		Model = glm::rotate(Model, robot.z, glm::vec3(0, 0, 1.0f));
		drawSeg(0.3);

		glm::vec4 lightPos = Model * glm::vec4(0.0f, 0.6f, 0.0f, 1.0f);
		glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);

		drawCS();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteTextures(1, &Texture);

	glDeleteProgram(programID);

	glfwTerminate();
	return 0;
}

