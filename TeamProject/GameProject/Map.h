#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include "std.h"
constexpr auto _Epsilon = 0.001f;

static std::ostream& operator<<(std::ostream& os, const std::wstring& str) {
	return os << std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{}.to_bytes(str);
}

static float	Lerp(float fStart, float fEnd, float fTangent)
{
	return fStart - (fStart * fTangent) + (fEnd * fTangent);
}
//enum class INPUT_LAYOUT
//{
//	PNCT = 0,
//	PNCTIW,
//};

static D3D11_INPUT_ELEMENT_DESC layoutPNCT[] =
{
	//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},		//POSITION0을 의미
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0},
};
static UINT size_layoutPNCT = ARRAYSIZE(layoutPNCT);

//static D3D11_INPUT_ELEMENT_DESC layoutPNCTIW[] =
//{
//	//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
//	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},		//POSITION0을 의미
//	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
//	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
//	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0},
//	{"INDEX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,0,D3D11_INPUT_PER_VERTEX_DATA, 0},
//	{"WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,16,D3D11_INPUT_PER_VERTEX_DATA, 0},
//};
//static UINT size_layoutPNCTIW = ARRAYSIZE(layoutPNCTIW);

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
		/*std::string line;
		std::getline(is, line);*/

		// pos 값을 추출합니다.
		size_t pos_start = is.str().find("position:") + strlen("position:");
		size_t pos_end = is.str().find(",", pos_start);
		std::string pos_str = is.str().substr(pos_start, pos_end - pos_start);
		std::istringstream pos_stream(pos_str);
		XMFLOAT3 position;
		pos_stream >> position.x >> position.y >> position.z;
		transform.position = XMLoadFloat3(&position);

		// rotation 값을 추출합니다.
		size_t rot_start = is.str().find("rotation:") + strlen("rotation:");
		size_t rot_end = is.str().find(",", rot_start);
		std::string rot_str = is.str().substr(rot_start, rot_end - rot_start);
		std::istringstream rot_stream(rot_str);
		XMFLOAT3 rotation;
		rot_stream >> rotation.x >> rotation.y >> rotation.z;
		transform.rotation = XMLoadFloat3(&rotation);

		// scale 값을 추출합니다.
		size_t scale_start = is.str().find("scale:") + strlen("scale:");
		size_t scale_end = is.str().find(",", scale_start);
		std::string normal_str = is.str().substr(scale_start, scale_end - scale_start);
		std::istringstream scale_stream(normal_str);
		XMFLOAT3 scale;
		scale_stream >> scale.x >> scale.y >> scale.z;
		transform.scale = XMLoadFloat3(&scale);

		return is;
	}
};


__declspec(align(16))
struct ConstantData_Transform
{
	XMMATRIX matWorld;
	XMMATRIX matView;
	XMMATRIX matProj;
};

__declspec(align(16))
struct ConstantData_Map
{
	XMFLOAT2 worldSize;
	float cellDistance;
	UINT tileCount;
};

__declspec(align(16))
struct ConstantData_Light
{
	XMFLOAT4 lightDirection;
	XMFLOAT4 cameraPosition;
};


struct PNCTVertex
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	XMFLOAT2 tex;
	bool operator==(const PNCTVertex& other) const
	{
		return  ((abs(pos.x - other.pos.x) <= _Epsilon) && (abs(pos.y - other.pos.y) <= _Epsilon) && (abs(pos.z - other.pos.z) <= _Epsilon)) &&
			((abs(normal.x - other.normal.x) <= _Epsilon) && (abs(normal.y - other.normal.y) <= _Epsilon) && (abs(normal.z - other.normal.z) <= _Epsilon)) &&
			((abs(color.x - other.color.x) <= _Epsilon) && (abs(color.y - other.color.y) <= _Epsilon) && (abs(color.z - other.color.z) <= _Epsilon) && (abs(color.w - other.color.w) <= _Epsilon)) &&
			((abs(tex.x - other.tex.x) <= _Epsilon) && (abs(tex.y - other.tex.y) <= _Epsilon));
	}
	friend std::ostream& operator<<(std::ostream& os, const PNCTVertex& pnctVertex)
	{
		os << "position:" << pnctVertex.pos.x << " " << pnctVertex.pos.y << " " << pnctVertex.pos.z << ", ";
		os << "normal:" << pnctVertex.normal.x << " " << pnctVertex.normal.y << " " << pnctVertex.normal.z << ", ";
		os << "color:" << pnctVertex.color.x << " " << pnctVertex.color.y << " " << pnctVertex.color.z << " " << pnctVertex.color.w << ", ";
		os << "texcoord:" << pnctVertex.tex.x << " " << pnctVertex.tex.y;
		return os;
	}

	friend std::istringstream& operator>>(std::istringstream& is, PNCTVertex& pnctVertex)
	{
		// "pos: x y z, tex: x y, normal: x y z, color: r g b a"와 같은 형태의 문자열에서 필드 값을 추출합니다.
		std::string line;
		std::getline(is, line);

		// pos 값을 추출합니다.
		size_t pos_start = line.find("position:") + strlen("position:");
		size_t pos_end = line.find(",", pos_start);
		std::string pos_str = line.substr(pos_start, pos_end - pos_start);
		std::istringstream pos_stream(pos_str);
		pos_stream >> pnctVertex.pos.x >> pnctVertex.pos.y >> pnctVertex.pos.z;

		// normal 값을 추출합니다.
		size_t normal_start = line.find("normal:") + strlen("normal:");
		size_t normal_end = line.find(",", normal_start);
		std::string normal_str = line.substr(normal_start, normal_end - normal_start);
		std::istringstream normal_stream(normal_str);
		normal_stream >> pnctVertex.normal.x >> pnctVertex.normal.y >> pnctVertex.normal.z;

		// color 값을 추출합니다.
		size_t color_start = line.find("color:") + strlen("color:");
		std::string color_str = line.substr(color_start);
		std::istringstream color_stream(color_str);
		color_stream >> pnctVertex.color.x >> pnctVertex.color.y >> pnctVertex.color.z >> pnctVertex.color.w;

		// tex 값을 추출합니다.
		size_t tex_start = line.find("texcoord:") + strlen("texcoord:");
		size_t tex_end = line.find(",", tex_start);
		std::string tex_str = line.substr(tex_start, tex_end - tex_start);
		std::istringstream tex_stream(tex_str);
		tex_stream >> pnctVertex.tex.x >> pnctVertex.tex.y;

		return is;
	}
};

struct CameraMove
{
	XMFLOAT3 camPos;
	float fYaw;
	float fPitch;
	float fRoll;
	friend std::ostream& operator<<(std::ostream& os, const CameraMove& camMove)
	{
		os << "camPos:" << camMove.camPos.x << " " << camMove.camPos.y << " " << camMove.camPos.z << ", ";
		os << "fYaw:" << camMove.fYaw << ", ";
		os << "fPitch:" << camMove.fPitch << ", ";
		os << "fRoll:" << camMove.fRoll;
		return os;
	}

	friend std::istringstream& operator>>(std::istringstream& is, CameraMove& camMove)
	{
		// "pos: x y z, tex: x y, normal: x y z, color: r g b a"와 같은 형태의 문자열에서 필드 값을 추출합니다.
		std::string line;
		std::getline(is, line);

		// camPos 값을 추출합니다.
		size_t pos_start = line.find("camPos:") + strlen("camPos:");
		size_t pos_end = line.find(",", pos_start);
		std::string pos_str = line.substr(pos_start, pos_end - pos_start);
		std::istringstream pos_stream(pos_str);
		pos_stream >> camMove.camPos.x >> camMove.camPos.y >> camMove.camPos.z;

		// fYaw 값을 추출합니다.
		size_t yaw_start = line.find("fYaw:") + strlen("fYaw:");
		size_t yaw_end = line.find(",", yaw_start);
		std::string yaw_str = line.substr(yaw_start, yaw_end - yaw_start);
		std::istringstream yaw_stream(yaw_str);
		yaw_stream >> camMove.fYaw;

		// fPitch 값을 추출합니다.
		size_t pitch_start = line.find("fPitch:") + strlen("fPitch:");
		size_t pitch_end = line.find(",", pitch_start);
		std::string pitch_str = line.substr(pitch_start, pitch_end - pitch_start);
		std::istringstream pitch_stream(pitch_str);
		pitch_stream >> camMove.fPitch;

		// fRoll 값을 추출합니다.
		size_t roll_start = line.find("fRoll:") + strlen("fRoll:");
		size_t roll_end = line.find(",", roll_start);
		std::string roll_str = line.substr(roll_start, roll_end - roll_start);
		std::istringstream roll_stream(roll_str);
		roll_stream >> camMove.fRoll;

		return is;
	}
};