#ifndef Renderer_h__
#define Renderer_h__

#include <gl/glew.h>
#include <gl/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>


#include "Shaders/shader.hpp"
#include "Model/Model.h"
#include "EulerCamera/EulerCamera.h"
#include "Cube/Cube.h"
#include "Texture/texture.h"
#include "GraphicsDevice/ShaderProgram.h"
#include "Scene/Model3D.h"

class Renderer
{


    GLuint programID;
    ShaderProgram shader;
	// Transformation
	GLuint MatrixID;
	GLuint ModelMatrixID;
	GLuint AmbientLightID;
	GLuint LightPositionID;
	GLuint EyePositionID;

	std::unique_ptr<Model> myTriangle;
	std::unique_ptr<Model> mySquare;
	std::unique_ptr<Cube> myCube;
	std::unique_ptr<Model3D> mySpider;

	std::unique_ptr<EulerCamera> myCamera;

	glm::mat4 triangle1M;

	glm::mat4 floorM;

	std::unique_ptr<Model> myHouse;
	std::unique_ptr<Texture> houseTexture;
	glm::mat4 houseM;
	glm::mat4 spiderM;

	glm::vec3 ambientLight;
	glm::vec3 lightPosition;

public:
    Renderer();
    ~Renderer();
    
    void Initialize();
    void Draw();
	void HandleKeyboardInput(int key);
	void HandleMouse(double deltaX,double deltaY);
	void Update(double deltaTime);
    void Cleanup();
};

#endif // Renderer_h__

