#include "Sources/MovableObject.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

MovableObject::MovableObject(glm::vec3 _pos, float sizeObject) : 
m_Position(_pos),
sizeObject(sizeObject),
m_Force(glm::vec3(0.f)),
m_Velocity(glm::vec3(0.f)),
m_Mass(1.f)
{
}


MovableObject::~MovableObject()
{
}

bool MovableObject::loadOBJ(const char * path)
{
	std::ifstream file(path);
	if (file.is_open())
	{
		std::string line;
		std::string word;
		while (getline(file, line))
		{
			if (line[0] == 'v' && line[1] == ' ')
			{
				//std::cout << "v:	";
				line = line.substr(2);

				std::istringstream isline(line);
				glm::vec3 vertex; 

				isline >> word;
				//std::cout << "x: " << word;
				vertex.x = atof(word.c_str());

				isline >> word;
				//std::cout << ", y: " << word;
				vertex.y = atof(word.c_str());

				isline >> word;
				//std::cout << ", z: " << word;
				vertex.z = atof(word.c_str());

				vertices.push_back(vertex);
			}
			else if (line[0] == 'v'  && line[1] == 't')
			{
				//std::cout << "vt:	";
				line = line.substr(3);

				std::istringstream isline(line);
				glm::vec3 uv;

				isline >> word;
				//std::cout << "x: " << word;
				uv.x = atof(word.c_str());

				isline >> word;
				//std::cout << ", y: " << word;
				uv.y = atof(word.c_str());

				isline >> word;
				//std::cout << ", z: " << word;
				uv.z = atof(word.c_str());

				uvs.push_back(uv);
			}
			else if (line[0] == 'v'  && line[1] == 'n')
			{
				//std::cout << "vn:	";
				line = line.substr(3);

				std::istringstream isline(line);
				glm::vec3 normal;

				isline >> word;
				//std::cout << "x: " << word;
				normal.x = atof(word.c_str());

				isline >> word;
				//std::cout << ", y: " << word;
				normal.y = std::stof(word);

				isline >> word;
				//std::cout << ", z: " << word;
				normal.z = atof(word.c_str());

				normals.push_back(normal);
			}
			else if (line[0] == 'f')
			{
				//std::cout << "f:	";
				line = line.substr(2);

				std::istringstream isline(line);
				char* split;
				unsigned int value;

				isline >> word;
				split = strtok((char*)word.c_str(), "/");
				value = atoi(split) - 1;
				//std::cout << value << ", ";
				triangleList.push_back(value);
				
				isline >> word;
				split = strtok((char*)word.c_str(), "/");
				value = atoi(split) - 1;
				//std::cout << value << ", ";
				triangleList.push_back(value);
				
				isline >> word;
				split = strtok((char*)word.c_str(), "/");
				value = atoi(split) - 1;
				//std::cout << value << std::endl;
				triangleList.push_back(value);
			}
		}
		file.close();

		triangleCount = triangleList.size() / 3;
		//std::cout << triangleCount << std::endl;
		
		return true;
	}
	else 
	{
		std::cout << "Impossible to open the file !\n" << std::endl;
		return false;
	}
}

glm::vec3 MovableObject::GetPosition()
{
	return m_Position;
}

void MovableObject::SetPosition(glm::vec3 _pos)
{
	m_Position = _pos;
}

glm::vec3 MovableObject::GetVelocity()
{
	return m_Velocity;
}

void MovableObject::SetVelocity(glm::vec3 _vel)
{
	m_Velocity = _vel;
}

float MovableObject::GetMass()
{
	return m_Mass;
}

void MovableObject::SetMass(float _mass)
{
	m_Mass = _mass;
}

glm::vec3 MovableObject::GetForce()
{
	return m_Force;
}

void MovableObject::SetForce(glm::vec3 _force)
{
	m_Force = _force;
}

void MovableObject::AddForce(glm::vec3 _force)
{
	m_Force += _force;
}

float MovableObject::GetSize()
{
	return sizeObject;
}

bool MovableObject::CheckHitObject(MovableObject *_object)
{
	if (glm::distance(_object->GetPosition(), m_Position) < (sizeObject + _object->GetSize()) / 2)
	{
		return true;
	}
	return false;
}

int MovableObject::GetTriangleCount()
{
	return triangleCount;
}

std::vector<unsigned int> MovableObject::GetTriangleList()
{
	return triangleList;
}

std::vector<glm::vec3> MovableObject::GetVertices()
{
	return vertices;
}

std::vector<glm::vec3> MovableObject::GetUVs()
{
	return uvs;
}

std::vector<glm::vec3> MovableObject::GetNormals()
{
	return normals;
}