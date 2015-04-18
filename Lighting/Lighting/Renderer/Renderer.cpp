#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include "OBJLoader/objloader.hpp"

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

	//calculate the normal of the triangle.
	glm::vec3 edge1 = myTriangle->VertexData[1] - myTriangle->VertexData[0];
	glm::vec3 edge2 = myTriangle->VertexData[2] - myTriangle->VertexData[0];
	glm::vec3 triangleNormal = glm::cross(edge1,edge2);
	triangleNormal = glm::normalize(triangleNormal);

	//since the triangle is not connected to anything else, so the normal is constant on all the vertices.
	myTriangle->NormalsData.push_back(triangleNormal);
	myTriangle->NormalsData.push_back(triangleNormal);
	myTriangle->NormalsData.push_back(triangleNormal);

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
	glm::vec3 squareNormal = glm::vec3(0.0,0.0,1.0);
	mySquare->NormalsData.push_back(squareNormal);
	mySquare->NormalsData.push_back(squareNormal);
	mySquare->NormalsData.push_back(squareNormal);
	mySquare->NormalsData.push_back(squareNormal);
	mySquare->Initialize();

	//////////////////////////////////////////////////////////////////////////
	myCube = std::unique_ptr<Cube>(new Cube());
	myCube->Initialize();
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	//Load model from OBJ file.
	std::vector<glm::vec3> houseVertices;
	std::vector<glm::vec3> houseNormals;
	std::vector<glm::vec2> houseUVs;
	if(loadOBJ("data/models/house/house.obj",houseVertices,houseUVs,houseNormals)){
	myHouse = std::unique_ptr<Model>(new Model());
	myHouse->VertexData = houseVertices;
	myHouse->UVData = houseUVs;
	myHouse->NormalsData = houseNormals;
	myHouse->Initialize();
	houseTexture = std::unique_ptr<Texture>(new Texture("data/models/house/house.jpg",1));
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//load model.
	mySpider = std::unique_ptr<Model3D>(new Model3D());
	//read model and it's textures from HDD.
	mySpider->LoadFromFile("data/models/Spider/spider.obj",true);
	//send the meshes to the GPU.
	mySpider->Initialize();
	//////////////////////////////////////////////////////////////////////////

	// Create and compile our GLSL program from the shaders
	//programID = LoadShaders( "SimpleTransformWithColor.vertexshader", "MultiColor.fragmentshader" );
	shader.LoadProgram();
	MatrixID = glGetUniformLocation(shader.programID, "MVP");
	ModelMatrixID = glGetUniformLocation(shader.programID, "ModelMatrix");
	// Use our shader
	//glUseProgram(programID);
	shader.UseProgram();

	//////////////////////////////////////////////////////////////////////////
	// Configure the light.
	//setup the light position.
	LightPositionID = glGetUniformLocation(shader.programID,"LightPosition_worldspace");
	lightPosition = glm::vec3(1.0,0.25,0.0);
	glUniform3fv(LightPositionID,1, &lightPosition[0]);
	//setup the ambient light component.
	AmbientLightID = glGetUniformLocation(shader.programID,"ambientLight");
	ambientLight = glm::vec3(0.1,0.1,0.1);
	glUniform3fv(AmbientLightID,1, &ambientLight[0]);
	//setup the eye position.
	EyePositionID = glGetUniformLocation(shader.programID,"EyePosition_worldspace");
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Projection matrix : 
	myCamera->SetPerspectiveProjection(45.0f,4.0f/3.0f,0.1f,100.0f);

	// View matrix : 
	myCamera->Reset(0.0,1.0,5.0,
					0,0,0,
					0,1,0);
	
	//send the eye position to the shaders.
	glUniform3fv(EyePositionID,1, &myCamera->GetEyePosition()[0]);
	//////////////////////////////////////////////////////////////////////////

	triangle1M  = glm::translate(-1.0f, 0.5f,1.0f) * glm::rotate(-30.0f, glm::vec3(0,1,0)) * glm::scale(0.25f,0.25f,0.25f);
	
	floorM =  glm::scale(2.0f,2.0f,2.0f)*glm::rotate(-90.0f,glm::vec3(1.0f,0.0f,0.0f));
	houseM = glm::scale(0.1f,0.1f,0.1f);
	spiderM = glm::scale(0.01f,0.01f,0.01f);
}

void Renderer::Draw()
{		
		shader.UseProgram();

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glm::mat4 VP = myCamera->GetProjectionMatrix() * myCamera->GetViewMatrix();
		shader.BindVPMatrix(&VP[0][0]);
		//1st triangle

		/*glm::mat4 triangle1MVP =   VP * triangle1M; 
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &triangle1MVP[0][0]);*/

		//we need to send the model matrix to transform the normals too.
		//glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &triangle1M[0][0]);
		shader.BindModelMatrix(&triangle1M[0][0]);
		myTriangle->Draw();
		//the floor
		//we need to send the model matrix to transform the normals too.
		//glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &floorM[0][0]);
		shader.BindModelMatrix(&floorM[0][0]);
		mySquare->Draw();


		houseTexture->Bind();
	//	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &houseM[0][0]);
		shader.BindModelMatrix(&houseM[0][0]);
		myHouse->Draw();

		
		mySpider->Render(&shader,spiderM);
		////////////////////////////////////////////////////////////////////////////
		////Draw the cube.
		//glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &myCube->CubeModelMatrix[0][0]);
		//myCube->Draw(MatrixID,VP);
		////////////////////////////////////////////////////////////////////////////
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

	//update the eye position uniform.
	glUniform3fv(EyePositionID,1, &myCamera->GetEyePosition()[0]);

}

void Renderer::HandleMouse(double deltaX,double deltaY)
{	
	myCamera->Yaw(deltaX);
	myCamera->Pitch(deltaY);
	myCamera->UpdateViewMatrix();
	//update the eye position uniform.
	glUniform3fv(EyePositionID,1, &myCamera->GetEyePosition()[0]);
}

