#include "graphicsclass.h"

GraphicsClass::GraphicsClass() { 
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other) { }

GraphicsClass::~GraphicsClass() { }

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hWnd) { 
	bool result;

	m_D3D = new D3DClass;
	if (!m_D3D) {
		return false;
	}

	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hWnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(hWnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	m_Camera = new CameraClass;
	if (!m_Camera) {
		return false;
	}

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	m_Model = new ModelClass;
	if (!m_Model) {
		return false;
	}

	result = m_Model->Initialize(m_D3D->GetDevice(), L"../DX10/data/seafloor.dds");
	if (!result) {
		MessageBox(hWnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader) {
		return false;
	}

	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hWnd);
	if (!result) {
		MessageBox(hWnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown() {
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	if (m_Model) {
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	if (m_Camera) {
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_D3D) {
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}

bool GraphicsClass::Frame() { 
	bool result;

	result = Render();
	if (!result) {
		return false;
	}

	return true;
}

bool GraphicsClass::Render() {
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result;

	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_Model->Render(m_D3D->GetDevice());

	m_TextureShader->Render(m_D3D->GetDevice(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	
	m_D3D->EndScene();

	return true;
}