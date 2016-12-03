#include "frustumclass.h"

FrustumClass::FrustumClass()
{
}


FrustumClass::FrustumClass(const FrustumClass& other)
{
}


FrustumClass::~FrustumClass()
{
}

void FrustumClass::ConstructFrustum(float screenDepth, XMMATRIX projectionMatrix, XMMATRIX viewMatrix)
{
	float zMinimum, r;
	XMFLOAT4X4 matrix;
	XMFLOAT4X4 tempProj;
	XMStoreFloat4x4(&tempProj, projectionMatrix);


	// Calculate the minimum Z distance in the frustum.
	zMinimum = -tempProj._43 / tempProj._33;
	r = screenDepth / (screenDepth - zMinimum);
	tempProj._33 = r;
	tempProj._43 = -r * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	XMStoreFloat4x4(&matrix, XMMatrixMultiply(viewMatrix, projectionMatrix));

	// Calculate near plane of frustum.
	m_planes[0].x = matrix._14 + matrix._13;
	m_planes[0].y = matrix._24 + matrix._23;
	m_planes[0].z = matrix._34 + matrix._33;
	m_planes[0].w = matrix._44 + matrix._43;
	XMPlaneNormalize(XMLoadFloat4(&m_planes[0]));

	// Calculate far plane of frustum.
	m_planes[1].x = matrix._14 - matrix._13;
	m_planes[1].y = matrix._24 - matrix._23;
	m_planes[1].z = matrix._34 - matrix._33;
	m_planes[1].w = matrix._44 - matrix._43;
	XMPlaneNormalize(XMLoadFloat4(&m_planes[1]));

	// Calculate left plane of frustum.
	m_planes[2].x = matrix._14 + matrix._11;
	m_planes[2].y = matrix._24 + matrix._21;
	m_planes[2].z = matrix._34 + matrix._31;
	m_planes[2].w = matrix._44 + matrix._41;
	XMPlaneNormalize(XMLoadFloat4(&m_planes[2]));

	// Calculate right plane of frustum.
	m_planes[3].x = matrix._14 - matrix._11;
	m_planes[3].y = matrix._24 - matrix._21;
	m_planes[3].z = matrix._34 - matrix._31;
	m_planes[3].w = matrix._44 - matrix._41;
	XMPlaneNormalize(XMLoadFloat4(&m_planes[3]));

	// Calculate top plane of frustum.
	m_planes[4].x = matrix._14 - matrix._12;
	m_planes[4].y = matrix._24 - matrix._22;
	m_planes[4].z = matrix._34 - matrix._32;
	m_planes[4].w = matrix._44 - matrix._42;
	XMPlaneNormalize(XMLoadFloat4(&m_planes[4]));

	// Calculate bottom plane of frustum.
	m_planes[5].x = matrix._14 + matrix._12;
	m_planes[5].y = matrix._24 + matrix._22;
	m_planes[5].z = matrix._34 + matrix._32;
	m_planes[5].w = matrix._44 + matrix._42;
	XMPlaneNormalize(XMLoadFloat4(&m_planes[5]));

	return;
}

bool FrustumClass::CheckPoint(float x, float y, float z)
{
	int i;


	// Check if the point is inside all six planes of the view frustum.
	for (i = 0; i<6; i++)
	{
		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat3(&XMFLOAT3(x, y, z))).m128_f32[0] < 0.0f)
		{
			return false;
		}
	}

	return true;
}

bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;
	XMVECTOR m_planesVX;

	// Check if any one point of the cube is in the view frustum.
	for (i = 0; i<6; i++)
	{
		m_planesVX = XMLoadFloat4(&m_planes[i]);
		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter - radius), (zCenter - radius)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter - radius), (zCenter - radius)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}
		
		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter + radius), (zCenter - radius)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter + radius), (zCenter - radius)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter - radius), (zCenter + radius)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter - radius), (zCenter + radius)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter + radius), (zCenter + radius)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter + radius), (zCenter + radius)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}

bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;


	// Check if the radius of the sphere is inside the view frustum.
	for (i = 0; i<6; i++)
	{
		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat3(&XMFLOAT3(xCenter, yCenter, zCenter))).m128_f32[0] < -radius)
		{
			return false;
		}
	}

	return true;
}

bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	int i;
	XMVECTOR m_planesVX;

	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for (i = 0; i<6; i++)
	{
		m_planesVX = XMLoadFloat4(&m_planes[i]);
		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter - xSize), (yCenter - ySize), (zCenter - zSize)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter + xSize), (yCenter - ySize), (zCenter - zSize)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter - xSize), (yCenter + ySize), (zCenter - zSize)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter - xSize), (yCenter - ySize), (zCenter + zSize)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter + xSize), (yCenter + ySize), (zCenter - zSize)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter + xSize), (yCenter - ySize), (zCenter + zSize)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter - xSize), (yCenter + ySize), (zCenter + zSize)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planesVX, XMLoadFloat3(&XMFLOAT3((xCenter + xSize), (yCenter + ySize), (zCenter + zSize)))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}
