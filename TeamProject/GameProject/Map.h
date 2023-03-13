#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include "std.h"

static std::ostream& operator<<(std::ostream& os, const std::wstring& str) {
	return os << std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{}.to_bytes(str);
}

struct Transform
{
	XMVECTOR position;
	XMVECTOR rotation;
	XMVECTOR scale;
	friend std::ostream& operator<<(std::ostream& os, const Transform& transform)
	{
		os << "position:" << XMVectorGetX(transform.position) << " " << XMVectorGetY(transform.position) << " " << XMVectorGetZ(transform.position) << ", ";
		os << "rotation:" << XMVectorGetX(transform.rotation) << " " << XMVectorGetY(transform.rotation) << " " << XMVectorGetZ(transform.rotation) << ", ";
		os << "scale:" << XMVectorGetX(transform.scale) << " " << XMVectorGetY(transform.scale) << " " << XMVectorGetZ(transform.scale);
		return os;
	}
	friend std::istringstream& operator>>(std::istringstream& is, Transform& transform)
	{
		// "pos: x y z, tex: x y, normal: x y z, color: r g b a"와 같은 형태의 문자열에서 필드 값을 추출합니다.
		std::string line;
		std::getline(is, line);

		// pos 값을 추출합니다.
		size_t pos_start = line.find("position:") + strlen("position:");
		size_t pos_end = line.find(",", pos_start);
		std::string pos_str = line.substr(pos_start, pos_end - pos_start);
		std::istringstream pos_stream(pos_str);
		XMFLOAT3 position;
		pos_stream >> position.x >> position.y >> position.z;
		transform.position = XMLoadFloat3(&position);

		// rotation 값을 추출합니다.
		size_t rot_start = line.find("rotation:") + strlen("rotation:");
		size_t rot_end = line.find(",", rot_start);
		std::string rot_str = line.substr(rot_start, rot_end - rot_start);
		std::istringstream rot_stream(rot_str);
		XMFLOAT3 rotation;
		rot_stream >> rotation.x >> rotation.y >> rotation.z;
		transform.rotation = XMLoadFloat3(&rotation);

		// scale 값을 추출합니다.
		size_t scale_start = line.find("scale:") + strlen("scale:");
		size_t scale_end = line.find(",", scale_start);
		std::string normal_str = line.substr(scale_start, scale_end - scale_start);
		std::istringstream scale_stream(normal_str);
		XMFLOAT3 scale;
		scale_stream >> scale.x >> scale.y >> scale.z;
		transform.scale = XMLoadFloat3(&scale);

		return is;
	}

	friend std::stringstream& operator>>(std::stringstream& is, Transform& transform)
	{
		// "pos: x y z, tex: x y, normal: x y z, color: r g b a"와 같은 형태의 문자열에서 필드 값을 추출합니다.
		std::string line;
		std::getline(is, line);

		// pos 값을 추출합니다.
		size_t pos_start = line.find("position:") + strlen("position:");
		size_t pos_end = line.find(",", pos_start);
		std::string pos_str = line.substr(pos_start, pos_end - pos_start);
		std::istringstream pos_stream(pos_str);
		XMFLOAT3 position;
		pos_stream >> position.x >> position.y >> position.z;
		transform.position = XMLoadFloat3(&position);

		// rotation 값을 추출합니다.
		size_t rot_start = line.find("rotation:") + strlen("rotation:");
		size_t rot_end = line.find(",", rot_start);
		std::string rot_str = line.substr(rot_start, rot_end - rot_start);
		std::istringstream rot_stream(rot_str);
		XMFLOAT3 rotation;
		rot_stream >> rotation.x >> rotation.y >> rotation.z;
		transform.rotation = XMLoadFloat3(&rotation);

		// scale 값을 추출합니다.
		size_t scale_start = line.find("scale:") + strlen("scale:");
		size_t scale_end = line.find(",", scale_start);
		std::string normal_str = line.substr(scale_start, scale_end - scale_start);
		std::istringstream scale_stream(normal_str);
		XMFLOAT3 scale;
		scale_stream >> scale.x >> scale.y >> scale.z;
		transform.scale = XMLoadFloat3(&scale);

		return is;
	}
};


__declspec(align(16))
struct constant
{
	XMMATRIX matWorld;
	XMMATRIX matView;
	XMMATRIX matProj;
	XMFLOAT4 m_light_direction;
	XMFLOAT4 m_camera_position;
};

__declspec(align(16))
struct constant_map
{
	XMMATRIX matWorld;
	XMMATRIX matView;
	XMMATRIX matProj;
	XMFLOAT4 m_light_direction;
	XMFLOAT4 m_camera_position;
	XMFLOAT2 m_world_size;
	float m_cell_distance;
};


struct PTNC
{
	XMFLOAT3 pos;
	XMFLOAT2 tex;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	friend std::ostream& operator<<(std::ostream& os, const PTNC& PTNC)
	{
		os << "position:" << PTNC.pos.x << " " << PTNC.pos.y << " " << PTNC.pos.z << ", ";
		os << "texcoord:" << PTNC.tex.x << " " << PTNC.tex.y << ", ";
		os << "normal:" << PTNC.normal.x << " " << PTNC.normal.y << " " << PTNC.normal.z << ", ";
		os << "color:" << PTNC.color.x << " " << PTNC.color.y << " " << PTNC.color.z << " " << PTNC.color.w;
		return os;
	}

	friend std::istringstream& operator>>(std::istringstream& is, PTNC& PTNC)
	{
		// "pos: x y z, tex: x y, normal: x y z, color: r g b a"와 같은 형태의 문자열에서 필드 값을 추출합니다.
		std::string line;
		std::getline(is, line);

		// pos 값을 추출합니다.
		size_t pos_start = line.find("position:") + strlen("position:");
		size_t pos_end = line.find(",", pos_start);
		std::string pos_str = line.substr(pos_start, pos_end - pos_start);
		std::istringstream pos_stream(pos_str);
		pos_stream >> PTNC.pos.x >> PTNC.pos.y >> PTNC.pos.z;

		// tex 값을 추출합니다.
		size_t tex_start = line.find("texcoord:") + strlen("texcoord:");
		size_t tex_end = line.find(",", tex_start);
		std::string tex_str = line.substr(tex_start, tex_end - tex_start);
		std::istringstream tex_stream(tex_str);
		tex_stream >> PTNC.tex.x >> PTNC.tex.y;

		// normal 값을 추출합니다.
		size_t normal_start = line.find("normal:") + strlen("normal:");
		size_t normal_end = line.find(",", normal_start);
		std::string normal_str = line.substr(normal_start, normal_end - normal_start);
		std::istringstream normal_stream(normal_str);
		normal_stream >> PTNC.normal.x >> PTNC.normal.y >> PTNC.normal.z;

		// color 값을 추출합니다.
		size_t color_start = line.find("color:") + strlen("color:");
		std::string color_str = line.substr(color_start);
		std::istringstream color_stream(color_str);
		color_stream >> PTNC.color.x >> PTNC.color.y >> PTNC.color.z >> PTNC.color.w;

		return is;
	}
};