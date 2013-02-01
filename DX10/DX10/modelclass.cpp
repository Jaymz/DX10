#include "modelclass.h"

ModelClass::ModelClass() {
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	m_Texture = 0;
}

ModelClass::ModelClass(const ModelClass& other) { }

ModelClass::~ModelClass() { }

bool ModelClass::Initialize(ID3D10Device* device, WCHAR* textureFilename) {
	bool result;

	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}

	result = LoadTexture(device, textureFilename);
	if (!result) {
		return false;
	}

	return true;
}

void ModelClass::Shutdown() {
	ReleaseTexture();

	ShutdownBuffers();

	return;
}

void ModelClass::Render(ID3D10Device* context) {
	RenderBuffers(context);

	return;
}

int ModelClass::GetIndexCount() {
	return m_indexCount;
}

ID3D10ShaderResourceView* ModelClass::GetTexture() {
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D10Device* device) {
	VertexType* vertices;
	unsigned long* indices;
	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = 5;
	m_indexCount = 5;

	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);

	vertices[1].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
	vertices[1].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	vertices[2].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[3].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
	vertices[3].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[4].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	vertices[4].texture = D3DXVECTOR2(0.0f, 1.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;

	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	//vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	//vertexData.SysMemPitch = 0;
	//vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result)) {
		return false;
	}

	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	//indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result)) {
		return false;
	}

	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

void ModelClass::ShutdownBuffers() {
	if(m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if(m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D10Device* context) {
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	return;
}

bool ModelClass::LoadTexture(ID3D10Device* device, WCHAR* filename) {
	bool result;

	m_Texture = new TextureClass;
	if (!m_Texture) {
		return false;
	}

	result = m_Texture->Initialize(device, filename);
	if (!result) {
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture() {
	if (m_Texture) {
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}