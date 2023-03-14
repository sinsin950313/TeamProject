// Camera : Register 0
// Object : Register 1
// Material : Register 2
// Animation : Register 3
// Mesh : Register 4

cbuffer Camera : register(b0)
{
	matrix CameraPosition : packoffset(c0);
	matrix ViewMatrix : packoffset(c4);
	matrix ProjectionMatrix : packoffset(c8);
};


cbuffer Object : register(b1)
{
	matrix WorldTransformMatrix : packoffset(c0);
};

cbuffer Material : register(b2)
{
};

cbuffer Animation : register(b3)
{
	matrix BoneAnimationMatrix[255];
	matrix MeshAnimationMatrix[255];
};

cbuffer Mesh_Animatable : register(b4)
{
	int MeshIndex;
};

cbuffer Mesh_Skinning : register(b4)
{
	matrix ToBoneSpaceTransformMatrix[255];
	int SkinningMeshIndex;
	float SkinningMeshWeight;
	int2 Padding;
};

