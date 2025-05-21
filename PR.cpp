/*
Proyecto
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//Variables

//Variables animacion 


//Ciclo dia y noche
float solDirZ;							//Direccion en Z del sol
float solDirY;							//Direccion en Y del sol
float solDirYOffset;
bool invierteCiclo;						//Para cambiar entre noche y dia
bool esDeDia;							//Verificar si es de dia

// Animación del humo
float humoTime = 0.0f;
float humoDuration = 80.0f; // Duración de un ciclo completo de humo
float humoScale = 50.0f; // Escala inicial del humo
float humoMaxScale = 50.0f; // Escala máxima del humo
float humoHeight = 700.0f; // Altura inicial del humo
float humoMaxHeight = 705.0f; // Altura máxima del humo
float humoOpacity = 0.7f; // Opacidad inicial

// Variables para la puerta
bool puertaAbierta = false;
float anguloPuerta = 0.0f; // Ángulo de rotación de la puerta
const float velocidadPuerta = 1.0f; // Velocidad de apertura/cierre

//Animacion para DADOS
float animDurationDados = 0.2f;
float velocidadRotacion = 90.0f;  //  Una rotación más controlada
float alturaMaxima = 1.5f;        // Salto más pequeño

// Posiciones fijas de los dados sobre la mesa (ajustadas para que estén centradas)
const glm::vec3 posDado1 = glm::vec3(-120.0f, 3.0f, -20.0f);
const glm::vec3 posDado2 = glm::vec3(-110.0f, 3.0f, -20.0f);

// Rotaciones finales
static glm::vec3 rotFinalDado1 = glm::vec3(0.0f);
static glm::vec3 rotFinalDado2 = glm::vec3(0.0f);
float animTimeDados = 0.0f;    // Tiempo acumulado de animación
float alturaDados = 0.0f;


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

//Camaras
Camera camera;
Camera finn;
Camera sonic;

//Texturas a utilizar en entorno opengl
Texture pisoTexture;	//Textura de piso (pavimento)
Texture HumoTexture;

//Modelos a utilizar en entorno opengl
Model Casa_Phineas;
Model Puerta;
Model Dado1;
Model Dado2;


//Skybox a utilizar en entorno opengl
//Dos tipos para el dia y la noche
Skybox skybox;
Skybox skybox2;

//Materiales a utilizar en entorno opengl
Material Material_brillante;
Material Material_opaco;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//funci�n de calculo de normales por promedio de v�rtices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int floorIndicesM[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVerticesM[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};

	unsigned int dialogoIndices[] = {
   0, 1, 2,
   0, 2, 3,
  
	};

	GLfloat dialogoVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(floorVerticesM, floorIndicesM, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(dialogoVertices, dialogoIndices, 32, 6);
	meshList.push_back(obj6);


	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);



}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 400.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), -90.0f, -90.0f, 0.3f, 0.5f);
	finn = Camera(glm::vec3(-220.0f, 9.0f, 61.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.3f, 0.5f);
	sonic = Camera(glm::vec3(201.0f, 10.0f, 27.0f), glm::vec3(0.0f, 1.0f, 0.0f), 180.0f, 0.0f, 0.3f, 0.5f);

	//********************************CARGA DE TEXTURAS*************************************
	pisoTexture = Texture("Textures/piso_phineas.png");
	pisoTexture.LoadTextureA();
	HumoTexture = Texture("Textures/humo.png");
	HumoTexture.LoadTextureA();


	//********************************CARGA DE MODELOS*************************************

	Casa_Phineas = Model();
	Casa_Phineas.LoadModel("Models/casa_Phineas.obj");
	Puerta = Model();
	Puerta.LoadModel("Models/puerta.obj");
	Dado1 = Model();
	Dado1.LoadModel("Models/dado1.obj");
	Dado2 = Model();
	Dado2.LoadModel("Models/dado2.obj");



	
	std::vector<std::string> skyboxFaces;
	std::vector<std::string> skyboxFaces2;

	skyboxFaces.push_back("Textures/Skybox/skybox_3.tga"); //right
	skyboxFaces.push_back("Textures/Skybox/skybox_1.tga"); //left
	skyboxFaces.push_back("Textures/Skybox/skybox_6.tga"); //down
	skyboxFaces.push_back("Textures/Skybox/skybox_5.tga"); //up
	skyboxFaces.push_back("Textures/Skybox/skybox_2.tga"); //front
	skyboxFaces.push_back("Textures/Skybox/skybox_4.tga"); //bh

	skyboxFaces2.push_back("Textures/Skybox/skybox_3_noche.tga"); //right
	skyboxFaces2.push_back("Textures/Skybox/skybox_1_noche.tga"); //left
	skyboxFaces2.push_back("Textures/Skybox/skybox_6_noche.tga"); //down
	skyboxFaces2.push_back("Textures/Skybox/skybox_5_noche.tga"); //up
	skyboxFaces2.push_back("Textures/Skybox/skybox_2_noche.tga"); //front
	skyboxFaces2.push_back("Textures/Skybox/skybox_4_noche.tga"); //bh

	skybox = Skybox(skyboxFaces);
	skybox2 = Skybox(skyboxFaces2);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);

	//LUCES PUNTUALES
	//Contador de luces puntuales
	

	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f, //LAMPARA
		0.8f, 1.0f,
		80.0f, 6.0f, -2.5f,
		0.5f, 0.2f, 0.1f);
	pointLightCount++;

	//Declaración de segunda luz puntual
	pointLights[1] = PointLight(0.68f, 1.0f, 0.18f, //INATOR
		0.8f, 1.0f,
		65.0f, 15.0f, -23.0f,
		1.0f, 0.09f, 0.032f);
	pointLightCount++;

	PointLight pointLightsBackup[2];  // Copia de seguridad de las luces originales
	pointLightsBackup[0] = pointLights[0];  // Luz 0 original
	pointLightsBackup[1] = pointLights[1];  // Luz 1 original

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f, //LUZ DELANTERA
		1.0f, 2.0f, //atenuacion
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0,//xDir, yDir, zDir
		1.0f, 0.0f, 0.0f,
		25.0f);
	spotLightCount++;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	//Loop mientras no se cierra la ventana




	//ciclo de dia y de noche
	solDirZ = -1.0f;
	solDirY = 0.0f;
	solDirYOffset = 0.1f;
	invierteCiclo = true;
	esDeDia = true;


	lastTime = glfwGetTime(); //Para empezar lo m�s cercano posible a 0

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//luz del sol
		if (esDeDia == true) { //es de d�a 
			if (invierteCiclo == true)
			{
				solDirY -= solDirYOffset * deltaTime;
				solDirZ = -1 - (solDirY / 100);
				if (solDirY <= -100.0f)
				{
					invierteCiclo = false;
				}
			}
			else
			{
				solDirY += solDirYOffset * deltaTime;
				solDirZ = (-1 - (solDirY / 100)) * -1;
				if (solDirY >= 0.0f)
				{
					invierteCiclo = true;
					esDeDia = false;
				}
			}
		}
		else { //es de noche
			if (invierteCiclo == true)
			{
				solDirY += solDirYOffset * deltaTime;
				solDirZ = 1 - (solDirY / 100);
				if (solDirY >= 100.0f)
				{
					invierteCiclo = false;
				}
			}
			else
			{
				solDirY -= solDirYOffset * deltaTime;
				solDirZ = (1 - (solDirY / 100)) * -1;
				if (solDirY <= 0.0f)
				{
					invierteCiclo = true;
					esDeDia = true;
				}
			}
		}

		//Animacion Humo
		humoTime += deltaTime;
		if (humoTime > humoDuration) {
			humoTime = 0.0f;
		}

		float humoProgress = humoTime / humoDuration;

		// Interpolación no lineal 
		humoScale = humoMaxScale * (1.0f - pow(1.0f - humoProgress, 2.0f));
		humoHeight = humoMaxHeight * humoProgress;
		humoOpacity = 0.7f * (1.0f - humoProgress * 0.8f); // El humo se desvanece

		// En el bucle principal (antes del renderizado)
		if (puertaAbierta) {
			if (anguloPuerta < 90.0f) {
				anguloPuerta += velocidadPuerta * deltaTime;
				if (anguloPuerta > 90.0f) anguloPuerta = 90.0f;
			}
		}
		else {
			if (anguloPuerta > 0.0f) {
				anguloPuerta -= velocidadPuerta * deltaTime;
				if (anguloPuerta < 0.0f) anguloPuerta = 0.0f;
			}
		}

		//Animacion Dados
		if (mainWindow.animacionDadosActiva) {
			animTimeDados += deltaTime;

			if (animTimeDados <= animDurationDados) {
				float progress = animTimeDados / animDurationDados;

				// Movimiento vertical suave 
				alturaDados = sin(progress * 3.14159265f) * alturaMaxima;
			}
			else {
				// Finalizar animación
				mainWindow.animacionDadosActiva = false;
				animTimeDados = 0.0f;
				alturaDados = -1.0f;  // Volver a la altura de la mesa

				// Rotaciones finales aleatorias (múltiplos de 90°)
				rotFinalDado1.x = 0.0f;                    // Sin rotación en X
				rotFinalDado1.y = 90.0f * (rand() % 4);    // Rotación en Y 
				rotFinalDado1.z = 0.0f;                    // Sin rotación en Z

				rotFinalDado2.x = 0.0f;
				rotFinalDado2.y = 90.0f * (rand() % 4);
				rotFinalDado2.z = 0.0f;
			}
		}
		
		//Recibir eventos del usuario
		glfwPollEvents();

		//Camaras y controles asignados
		if (mainWindow.getopcion() == 0.0f)
		{
			finn.keyControlDep(mainWindow.getsKeys(), deltaTime);
			finn.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		}
		else if (mainWindow.getopcion() == 1.0f)
		{
			sonic.keyControlSon(mainWindow.getsKeys(), deltaTime);
			sonic.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		else if (mainWindow.getopcion() == 2.0f)
		{
			//Para solo mover con teclado sin mouse
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
		}

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Para decidir que ven las camaras
		if (esDeDia) {
			if (mainWindow.getopcion() == 0.0f)
			{
				skybox.DrawSkybox(finn.calculateViewMatrix(), projection);
			}
			else if (mainWindow.getopcion() == 1.0f)
			{
				skybox.DrawSkybox(sonic.calculateViewMatrix(), projection);
			}
			else if (mainWindow.getopcion() == 2.0f)
			{
				skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
			}
		}
		else {
			if (mainWindow.getopcion() == 0.0f)
			{
				skybox2.DrawSkybox(finn.calculateViewMatrix(), projection);
			}
			else if (mainWindow.getopcion() == 1.0f)
			{
				skybox2.DrawSkybox(sonic.calculateViewMatrix(), projection);
			}
			else if (mainWindow.getopcion() == 2.0f)
			{
				skybox2.DrawSkybox(camera.calculateViewMatrix(), projection);
			}
		}


		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		//Cambiar entre camaras y guardar su posicion
		if (mainWindow.getopcion() == 0.0f)
		{
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(finn.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, finn.getCameraPosition().x, finn.getCameraPosition().y, finn.getCameraPosition().z);
		}
		else if (mainWindow.getopcion() == 1.0f)
		{
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(sonic.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, sonic.getCameraPosition().x, sonic.getCameraPosition().y, sonic.getCameraPosition().z);
		}
		else if (mainWindow.getopcion() == 2.0f)
		{

			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		}

	

		//Establecer direccion del sol 
		mainLight.SetDir(glm::vec3(0.0f, solDirY / 100, solDirZ));

		

		//informaci�n al shader de fuentes de iluminaci�n
		shaderList[0].SetDirectionalLight(&mainLight);

		int activePointLights = pointLightCount; // Inicialmente todas activas

		

		

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		//**************************************************************************PISO**************************************************************************
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(60.0f, 30.0f, 60.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//****************************************************CASA**************************************************************************

		
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-268.737f, 0.0f, 155.656f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Casa_Phineas.RenderModel();
		glDisable(GL_BLEND);

		//--------------- Humo ---------
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-210.0f, humoHeight, -10.0f)); // Misma posición que el puesto
		model = glm::scale(model, glm::vec3(humoScale, humoScale, humoScale));
		model = glm::rotate(model, (float)glfwGetTime() * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación lenta

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(uniformSpecularIntensity, 0.1f); // Bajo brillo
		glUniform1f(uniformShininess, 4.0f);

		HumoTexture.UseTexture();
		meshList[3]->RenderMesh(); // Usa el mesh de vegetación que ya tienes


		glDisable(GL_BLEND);

		//Puerta
	
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-210.0f, 0.0f, -10.0f)); // Ajusta posición según tu escena
		model = glm::rotate(model, mainWindow.anguloPuerta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Puerta.RenderModel();


		//--------------- Dado1---------
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dado1.RenderModel();

		//--------------- Dado2---------
		model = glm::mat4(1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		Dado2.RenderModel();


		
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}