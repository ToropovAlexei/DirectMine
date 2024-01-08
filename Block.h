#pragma once

class Block
{
public:
	enum class BlockFaces
	{
		Front,
		Back,
		Top,
		Bottom,
		Left,
		Right
	};
public:
	Block() = default;
	Block(std::string name, std::vector<std::string> textures) noexcept;

	std::string& GetName();
	std::string& GetFaceTexture(BlockFaces face);

private:
	void BuildTextures(std::vector<std::string>) noexcept;

private:
	std::string m_name;
	std::array<std::string, 6u> m_textures; // Front, Back, Top, Bottom, Left, Right
};

