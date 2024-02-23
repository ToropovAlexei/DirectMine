#pragma once

#include "TextureLoader.h"
#include "UVCoords.h"

class TextureAtlas
{
public:
	TextureAtlas() = delete;
	static void BuildAtlas(ID3D11Device* device, ID3D11DeviceContext* context);

	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Get3dTextureSRV() noexcept
	{
		return texture3dSRV;
	};

	static inline float GetTextureIdx(std::string_view texName) noexcept
	{
		return texturesIndices.find(texName)->second;
	};

private:
	struct string_hash {
		using is_transparent = void;
		[[nodiscard]] size_t operator()(const char* txt) const {
			return std::hash<std::string_view>{}(txt);
		}
		[[nodiscard]] size_t operator()(std::string_view txt) const {
			return std::hash<std::string_view>{}(txt);
		}
		[[nodiscard]] size_t operator()(const std::string& txt) const {
			return std::hash<std::string>{}(txt);
		}
	};

private:
	static Microsoft::WRL::ComPtr<ID3D11Texture2D> atlas3d;
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture3dSRV;
	static std::unordered_map<std::string, float, string_hash, std::equal_to<>> texturesIndices;
};

