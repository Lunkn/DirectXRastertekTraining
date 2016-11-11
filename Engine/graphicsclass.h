#pragma once
#include <Windows.h>
#include "d3dclass.h"
#include "camerclass.h"
#include "modelclass.h"
#include "shaderclass.h"
#include "lightclass.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();
	bool RenderWithLight(float);

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	ShaderClass* m_Shader;
	LightClass* m_Light;
};