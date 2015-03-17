#pragma once

#include <glm/glm.hpp>
#include <vector>

class MovableObject
{
public:
	MovableObject(glm::vec3 _pos, float sizeObject);
	~MovableObject();

	bool loadOBJ(const char * path);

	virtual glm::vec3 GetPosition();
	virtual void SetPosition(glm::vec3 _pos);

	virtual glm::vec3 GetVelocity();
	virtual void SetVelocity(glm::vec3 _vel);

	virtual float GetMass();
	virtual void SetMass(float _mass);

	virtual glm::vec3 GetForce();
	virtual void SetForce(glm::vec3 _force);

	virtual void AddForce(glm::vec3 _force);
	virtual float GetSize();
	virtual bool CheckHitObject(MovableObject *_object);

	int GetTriangleCount();
	std::vector<unsigned int> GetTriangleList();
	std::vector<glm::vec3> GetVertices();
	std::vector<glm::vec2> GetUVs();
	std::vector<glm::vec3> GetNormals();

protected:
	glm::vec3 m_Position;
	glm::vec3 m_Velocity;
	glm::vec3 m_Force;
	float m_Mass;

	float sizeObject;
	int triangleCount;
	std::vector<unsigned int> triangleList;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};

