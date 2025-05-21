#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	

	bool puertaAbierta;  // Estado de la puerta (abierta/cerrada)
	float anguloPuerta;  // Ángulo actual de la puerta
	//Camara
	GLfloat getopcion() { return opcion; }

	//Orbe
	GLfloat getOrbePrendido() { return orbePrendido;  }

	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	// Variables para animación de dados
	bool monedaInsertada;
	bool animacionDadosActiva;
	float animTimeDados;
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	

	//Movimiento Personajes
	//Depresso
	GLfloat giroIzDepress;
	GLfloat giroDeDepress;
	GLfloat movDepress;
	//Sonic
	GLfloat giroIzSonic;
	GLfloat giroDeSonic;
	GLfloat movSonic;
	//Camara opcion
	GLfloat opcion;
	//Prendido del orbe
	bool orbePrendido = true; //el orbe comienza prendido

	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

