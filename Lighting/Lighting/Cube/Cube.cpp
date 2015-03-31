#include "Cube.h"

#include <glm\gtx\transform.hpp>


Cube::Cube(void)
{
}


Cube::~Cube(void)
{
}

void Cube::Initialize()
{

	mSquare = Model();
	mSquare.VertexData.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
	mSquare.VertexData.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
	mSquare.VertexData.push_back(glm::vec3( 1.0f,  1.0f, 0.0f));
	mSquare.VertexData.push_back(glm::vec3( -1.0f,  1.0f, 0.0f));
	mSquare.ColorData.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	mSquare.ColorData.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	mSquare.ColorData.push_back(glm::vec3(0.5f, 0.0f, 0.5f));
	mSquare.ColorData.push_back(glm::vec3(1.0f, 0.5f, 0.5f));
	//first.riangle.
	mSquare.IndicesData.push_back(0);
	mSquare.IndicesData.push_back(1);
	mSquare.IndicesData.push_back(3);
	//secon.triangle.
	mSquare.IndicesData.push_back(1);
	mSquare.IndicesData.push_back(2);
	mSquare.IndicesData.push_back(3);
	mSquare.Initialize();



	int numberOfFaces = 6;
	mFacesModelMatrices.resize(numberOfFaces);
	
	//bottom
	mFacesModelMatrices[0] = glm::translate(0.0f,-1.0f,0.0f)*glm::rotate(90.0f,glm::vec3(1.0f,0.0f,0.0f));
	//top
	mFacesModelMatrices[1] = glm::translate(0.0f,1.0f,0.0f)*glm::rotate(90.0f,glm::vec3(1.0f,0.0f,0.0f));
	//front
	mFacesModelMatrices[2] = glm::translate(0.0f,0.0f,1.0f);
	//back
	mFacesModelMatrices[3] = glm::translate(0.0f,0.0f,-1.0f);
	//left
	mFacesModelMatrices[4] = glm::translate(-1.0f,0.0f,0.0f)*glm::rotate(90.0f,glm::vec3(0.0f,1.0f,0.0f));
	//right
	mFacesModelMatrices[5] = glm::translate(1.0f,0.0f,0.0f)*glm::rotate(90.0f,glm::vec3(0.0f,1.0f,0.0f));

	//this transformation is applied on the whole cube!!!
	CubeModelMatrix = glm::scale(0.5f,0.5f,0.5f);
}

void Cube::Draw(GLuint mvpUniformMatrixID,glm::mat4 VP)
{

	for (int i = 0; i < mFacesModelMatrices.size(); i++)
	{
		glm::mat4 mvp = VP * CubeModelMatrix * mFacesModelMatrices[i];
		glUniformMatrix4fv(mvpUniformMatrixID, 1, GL_FALSE, &mvp[0][0]);
		mSquare.Draw();
	}
}
