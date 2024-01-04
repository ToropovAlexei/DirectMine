#pragma once

#include <d3dcompiler.h>

class ShadersLoader
{
public:
    static Microsoft::WRL::ComPtr<ID3D11VertexShader> LoadVertexShader(ID3D11Device* device, const wchar_t* filename)
    {
        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
        HRESULT result = D3DReadFileToBlob(filename, &shaderBlob);
        if (result != S_OK)
        {
            throw std::runtime_error("Failed to read vertex shader"); // TODO add filename
            return nullptr;
        }

        Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
        result = device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &vertexShader);
        if (result != S_OK)
        {
            throw std::runtime_error("Failed to create vertex shader"); // TODO add filename
            return nullptr;
        }

        return vertexShader;
    }

    static Microsoft::WRL::ComPtr<ID3D11PixelShader> LoadPixelShader(ID3D11Device* device, const wchar_t* filename)
    {
        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
        HRESULT result = D3DReadFileToBlob(filename, &shaderBlob);
        if (result != S_OK)
        {
            throw std::runtime_error("Failed to read pixel shader"); // TODO add filename
            return nullptr;
        }

        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
        result = device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pixelShader);
        if (result != S_OK)
        {
            throw std::runtime_error("Failed to create pixel shader"); // TODO add filename
            return nullptr;
        }

        return pixelShader;
    }
};

