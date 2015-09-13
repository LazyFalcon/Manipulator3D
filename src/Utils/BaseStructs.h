#pragma once
struct Image
{
	uint32_t ID;
	int w;
	int h;
};
struct Icon
{
	glm::vec4 uvs;
	glm::vec4 rect;
	uint32_t color;
};
struct ImageSet
{
	uint32_t ID;
	int w;
	int h;
	std::map <std::string, Icon> set;
};
struct Atlas
{
	uint32_t ID;
	int layers;
};
struct PointLamp
{
	glm::vec4 position;
	glm::vec4 color;
	float falloffDistance;
	float energy;
};
struct PointLight
{
	glm::vec4 position;
	glm::vec4 color;
	float size;
};
struct SpotLight
{
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec4 color;
	glm::vec3 size; // cos, H, R
};
struct ArrayData
{
	std::vector <unsigned char> data;
	int size;
	float operator ()(int x, int y){
		return data[x+y*size]/256.f;
	}
	float operator ()(glm::vec2 uv){
		return data[(int)uv.x+(int)(uv.y*size)]/256.f;
	}
};
struct Material
{
	glm::vec4 color;
};
struct Mesh
{
	int begin;
	int end;
	int count;
	int ID;
	void *offset;
	int32_t VAO;
	// ~Mesh(){
		// cerr<<"~Mesh"<<endl;
	// }
};
struct Entity
{
	uint16_t ID;
	Mesh *mesh;
	Material material;
	glm::vec4 position;
	glm::quat quat;
	btRigidBody *rgBody;
	bool isOutlined;
	btRigidBody& rgBodyRef(){
		return *rgBody;
	}
};
enum class OwnerType
{
    None, Robot, Static, SceneElement
};
struct EntityPayload
{
    OwnerType ownerType;
    Entity *backPointer;
    void *owner;
};
