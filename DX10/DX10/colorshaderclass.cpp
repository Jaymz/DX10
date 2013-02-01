#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass() {
	m_effect = 0;
	m_technique = 0;
	m_layout = 0;

	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other) { }

ColorShaderClass::~ColorShaderClass() { }

bool ColorShaderClass::Initialize(ID3D10Device* device, HWND hWnd) {
	bool result;

	result = InitializeShader(device, hWnd, L"../DX10/color.fx");
	if (!result) {
		return false;
	}

	return true;
}

void ColorShaderClass::Shutdown() {
	ShutdownShader();

	return;
}

bool ColorShaderClass::Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix) {
	bool result;

	result = SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix);
	if (!result) {
		return false;
	}

	RenderShader(device, indexCount);

	return true;
}

	
bool ColorShaderClass::InitializeShader(ID3D10Device* device, HWND hWnd, WCHAR* filename) {
	HRESULT result;
	ID3D10Blob* errorMessage;
	/*ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;*/
	D3D10_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	//D3D10_BUFFER_DESC matrixBufferDesc;
	D3D10_PASS_DESC passDesc;

	errorMessage = 0;
	/*vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;*/

	result = D3DX10CreateEffectFromFile(filename, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, device, NULL, NULL, &m_effect, &errorMessage, NULL);

	if (FAILED(result)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hWnd, filename);
		} else {
			MessageBox(hWnd, filename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	m_technique = m_effect->GetTechniqueByName("ColorTechnique");
	if (!m_technique) { 
		return false;
	}

	/*result = D3DX11CompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	
	if (FAILED(result)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hWnd, psFilename);
		} else {
			MessageBox(hWnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), 
		NULL, &m_vertexShader);

	if (FAILED(result)) {
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
		NULL, &m_pixelShader);

	if (FAILED(result)) {
		return false;
	}*/

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	m_technique->GetPassByIndex(0)->GetDesc(&passDesc);
	
	result = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_layout);
	if (FAILED(result)) {
		return false;
	}

	m_worldMatrixPtr = m_effect->GetVariableByName("worldMatrix")->AsMatrix();
	m_viewMatrixPtr = m_effect->GetVariableByName("viewMatrix")->AsMatrix();
	m_projectionMatrixPtr = m_effect->GetVariableByName("projectionMatrix")->AsMatrix();

	/*vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) {
		return false;
	}*/

	return true;
}

void ColorShaderClass::ShutdownShader() {
	/*if(m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}*/

	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;

	if(m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	m_technique = 0;

	if (m_effect) { 
		m_effect->Release();
		m_effect = 0;
	}

	/*if(m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if(m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}*/

	return;
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hWnd, WCHAR* shaderFilename) {
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for(i = 0; i < bufferSize; i++) {
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hWnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);
	
	return;
}

bool ColorShaderClass::SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix) {
	/*HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	result = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	context->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	context->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);*/

	m_worldMatrixPtr->SetMatrix((float*)&worldMatrix);
	m_viewMatrixPtr->SetMatrix((float*)&viewMatrix);
	m_projectionMatrixPtr->SetMatrix((float*)&projectionMatrix);

	return true;
}

void ColorShaderClass::RenderShader(ID3D10Device* device, int indexCount) {
	/*context->IASetInputLayout(m_layout);

	context->VSSetShader(m_vertexShader, NULL, 0);
	context->PSSetShader(m_pixelShader, NULL, 0);

	context->DrawIndexed(indexCount, 0, 0);
*/
	D3D10_TECHNIQUE_DESC techniqueDesc;
	unsigned int i;

	device->IASetInputLayout(m_layout);

	m_technique->GetDesc(&techniqueDesc);

	for(i=0;i<techniqueDesc.Passes;++i) {
		m_technique->GetPassByIndex(i)->Apply(0);
		device->DrawIndexed(indexCount, 0, 0);
	}

	return;
}