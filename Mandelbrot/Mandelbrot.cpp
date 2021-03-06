//by Eemeli Lehtonen

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Windows.h>

#include "shader.h"
#include "arrayBuffer.h"
#include "vertexArray.h"
#include "gameloop.h"

#define WIDTH 1920
#define HEIGHT 1080
#define ASPECT WIDTH / (float)HEIGHT

double zoom = 1;
glm::dvec2 offset = glm::dvec2(0.0);
short iterations = 148;

double speed = 0.02;

GLFWwindow *window;
VertexArray *vao;
Shader *shader;
logic::GameLoop *loop;
int upcount = 0;

bool keys[2048];
void key(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) keys[key] = true;
	else if (action == GLFW_RELEASE) keys[key] = false;
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT);
	vao->bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void update() {
	if (glfwWindowShouldClose(window)) loop->stop();

	if (keys[GLFW_KEY_W]) offset.y -= speed * zoom;
	if (keys[GLFW_KEY_S]) offset.y += speed * zoom;
	if (keys[GLFW_KEY_A]) offset.x -= speed * zoom;
	if (keys[GLFW_KEY_D]) offset.x += speed * zoom;
	
	if (keys[GLFW_KEY_Q]) zoom *= 1.01;
	if (keys[GLFW_KEY_E]) zoom *= 0.99;
	
	if (keys[GLFW_KEY_R]) { iterations += 1; if (iterations < 0) iterations = 0; std::cout << "Iterations: " << iterations << std::endl; }
	if (keys[GLFW_KEY_F]) { iterations -= 1; if (iterations < 0) iterations = 0; std::cout << "Iterations: " << iterations << std::endl; }
	if (keys[GLFW_KEY_ESCAPE]) { glfwDestroyWindow(window); glfwTerminate(); exit(0); }
	if (keys[GLFW_KEY_TAB]) { iterations = 127; offset = glm::highp_vec2(0.0); zoom = 1; }

	upcount++;
	if (upcount > 100) {
		std::cout << "FPS: " << loop->getFPS() << "\nUPS: " << loop->getUPS() << std::endl;
		upcount = 0;
	}
	
	shader->setUniform1d("unif_zoom", zoom);
	shader->setUniform2d("unif_offset", offset);
	shader->setUniform1i("unif_iterations", iterations);
}

int main()
{
	//::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	for (int i = 0; i < 2048; i++) keys[i] = false;

	if (!glfwInit()) { return -1; }
	window = glfwCreateWindow(WIDTH, HEIGHT, "Mandelbrot set", glfwGetPrimaryMonitor(), NULL);
	if (!window) { glfwTerminate(); return -2; }
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { glfwTerminate(); return -3; }
	glfwSetKeyCallback(window, key);
	glfwSwapInterval(1);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	shader = new Shader("shaders/basic.vert.glsl", "shaders/basic.frag.glsl");
	glm::mat4 orthographic = glm::ortho(-ASPECT, ASPECT, 1.0f, -1.0f);
	shader->enable(); shader->setUniformMat4("pr_matrix", orthographic);

	GLfloat vertices[] = {
		-ASPECT, -1.0f,
		-ASPECT,  1.0f,
		 ASPECT,  1.0f,
		-ASPECT, -1.0f,
		 ASPECT,  1.0f,
		 ASPECT, -1.0f
	};

	vao = new VertexArray();
	ArrayBuffer *vbo = new ArrayBuffer(vertices, 12, 2);
	vao->addBuffer(vbo, 0);

	loop = new logic::GameLoop(render, update, 10000);
	loop->start();
	return 0;
}