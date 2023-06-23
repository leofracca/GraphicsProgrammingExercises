#pragma once

#include <string>

class Texture
{
private:
	unsigned int m_rendererID;
	std::string m_filePath;
	unsigned char* m_localBuffer;
	int m_width, m_height, m_bitsPerPixel;

public:
	Texture(const std::string& filePath, int param, bool transparent = false);
	Texture(const int width, const int height);
	~Texture();

	unsigned int getRendererID() const { return m_rendererID; }
	void bind(unsigned int slot = 0) const;
	void unbind() const;
};