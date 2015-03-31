#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>

Renderer::Renderer()
{
    
}

Renderer::~Renderer()
{
    Cleanup();
}

void Renderer::Initialize()
{
	//myCamera = std::unique_ptr<FPCamera>(new FPCamera());
	myCamera = std::unique_ptr<EulerCamera>(new EulerCamera());

	myTriangle = std::unique_ptr<Model>(new Model());

	myTriangle->VertexData.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
	myTriangle->VertexData.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
	myTriangle->VertexData.push_back(glm::vec3( 0.0f,  1.0f, 0.0f));

	myTriangle->ColorData.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	myTriangle->ColorData.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	myTriangle->ColorData.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	myTriangle->UVData.push_back(glm::vec2(0.0f,0.0f));
	myTriangle->UVData.push_back(glm::vec2(1.0f,0.0f));
	myTriangle->UVData.push_back(glm::vec2(0.0f,1.0f));
	myTriangle->Initialize();

	//drawing a square.
	mySquare = std::unique_ptr<Model>(new Model());

	mySquare->VertexData.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
	mySquare->VertexData.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
	mySquare->VertexData.push_back(glm::vec3( 1.0f,  1.0f, 0.0f));
	mySquare->VertexData.push_back(glm::vec3( -1.0f,  1.0f, 0.0f));
	mySquare->ColorData.push_back(glm::vec3(1.0f, 0.0f, 0.5f));
	mySquare->ColorData.push_back(glm::vec3(0.0f, 0.0f, 0.5f));
	mySquare->ColorData.push_back(glm::vec3(0.0f, 0.0f, 0.5f));
	mySquare->ColorData.push_back(glm::vec3(1.0f, 0.0f, 0.5f));

	//first triangle.
	mySquare->IndicesData.push_back(0);
	mySquare->IndicesData.push_back(1);
	mySquare->IndicesData.push_back(3);

	//second triangle.
	mySquare->IndicesData.push_back(1);
	mySquare->IndicesData.push_back(2);
	mySquare->IndicesData.push_back(3);

	mySquare->Initialize();



	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "SimpleTransformWithColor.vertexshader", "MultiColor.fragmentshader" );
	

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Transformations
	////////////////////
	// Get a handle for our "MVP" uniform the holder we created in the vertex shader)
	// GLint glGetUniformLocation(GLuint program​, const GLchar *name​);
	// program:  Specifies the program objec(t to be queried.
	// name: Points to a null terminated string containing the name of the uniform variable whose location is to be queried. 
	MatrixID = glGetUniformLocation(programID, "MVP");
	
	//////////////////////////////////////////////////////////////////////////

	// Projection matrix : 
	myCamera->SetPerspectiveProjection(45.0f,4.0f/3.0f,0.1f,100.0f);

	// View matrix : 
	myCamera->Reset(0.0,0.0,5.0,
					0,0,0,
					0,1,0);
	//////////////////////////////////////////////////////////////////////////

	triangle1M  = glm::translate(-1.0f, -0.25f,1.0f) * glm::rotate(-30.0f, glm::vec3(0,1,0)) * glm::scale(0.25f,0.25f,0.25f);
	triangle2M  = glm::translate(-1.0f, -0.25f,-0.75f) * glm::rotate(30.0f, glm::vec3(0,1,0)) * glm::scale(0.25f,0.25f,0.25f);
	triangle3M  = glm::translate(+1.0f, -0.25f,1.0f) * glm::rotate(-30.0f, glm::vec3(0,1,0)) * glm::scale(0.25f,0.25f,0.25f);
	triangle4M  = glm::translate(+1.0f, -0.25f,-0.75f) * glm::rotate(30.0f, glm::vec3(0,1,0)) * glm::scale(0.25f,0.25f,0.25f);

	floorM =  glm::scale(2.0f,2.0f,2.0f)*glm::translate(0.0f,-0.25f,0.0f)*glm::rotate(90.0f,glm::vec3(1.0f,0.0f,0.0f));
}

void Renderer::Draw()
{		
		// Use our shader
		glUseProgram(programID);

			// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glm::mat4 VP = myCamera->GetProjectionMatrix() * myCamera->GetViewMatrix();
		//1st triangle
		glm::mat4 triangle1MVP =   VP * triangle1M; 
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &triangle1MVP[0][0]);
		myTriangle->Draw();

		//2nd triangle
		glm::mat4 triangle2MVP =   VP * triangle2M; 
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &triangle2MVP[0][0]);
		myTriangle->Draw();

		//3rd triangle
		glm::mat4 triangle3MVP =   VP * triangle3M; 
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &triangle3MVP[0][0]);
		myTriangle->Draw();


		//4th triangle
		glm::mat4 triangle4MVP =   VP * triangle4M; 
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &triangle4MVP[0][0]);
		myTriangle->Draw();

		//the floor
		glm::mat4 floorMVP =  VP * floorM; 
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &floorMVP[0][0]);
		mySquare->Draw();
}

void Renderer::Cleanup()
{
    glDeleteProgram(programID);
}

void Renderer::Update(double deltaTime)
{
	double triangle1RotationSpeed = 0.06;
	double trianglesRotationAngle = triangle1RotationSpeed*deltaTime;
	glm::mat4 rotationMat = glm::rotate((float)trianglesRotationAngle,glm::vec3(0.0,1.0,0.0));
	triangle1M = triangle1M * rotationMat;
	triangle2M = triangle2M * rotationMat;
	triangle3M = triangle3M * rotationMat;
	triangle4M = triangle4M * rotationMat;

	//double squareRotationSpeed = 0.2;
	//double squareRotationAngle = squareRotationSpeed*deltaTime;
	//floorM =  glm::rotate((float)squareRotationAngle,glm::vec3(0.0,1.0,0.0))* floorM ;
}

void Renderer::HandleKeyboardInput(int key)
{
	switch (key)
	{
		//Moving forward
	case GLFW_KEY_UP:
	case GLFW_KEY_W:
		myCamera->Walk(0.5);
		break;

		//Moving backword
	case GLFW_KEY_DOWN:
	case GLFW_KEY_S:
		myCamera->Walk(-0.5);
		break;

		// Moving right
	case GLFW_KEY_RIGHT:
	case GLFW_KEY_D:
		myCamera->Strafe(0.1);
		break;

		// Moving left
	case GLFW_KEY_LEFT:
	case GLFW_KEY_A:
		myCamera->Strafe(-0.1);
		break;

		// Moving up
	case GLFW_KEY_SPACE:
	case GLFW_KEY_R:
		myCamera->Fly(0.1);
		break;

		// Moving down
	case GLFW_KEY_LEFT_CONTROL:
	case GLFW_KEY_F:
		myCamera->Fly(-0.1);
		break;
	default:
		break;
	}


	//continue the remaining movements.

	myCamera->UpdateViewMatrix();
}

void Renderer::HandleMouse(double deltaX,double deltaY)
{	
	myCamera->Yaw(deltaX);
	myCamera->Pitch(deltaY);
	myCamera->UpdateViewMatrix();

}
