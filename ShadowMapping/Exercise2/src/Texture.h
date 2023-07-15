#pragma once

#include <string>

class PlaneTexture
{
private:
	unsigned int m_rendererID;
	std::string m_filePath;
	unsigned char* m_localBuffer;
	int m_width, m_height, m_bitsPerPixel;

public:
	PlaneTexture(const std::string& filePath, int param, bool transparent = false);
	PlaneTexture(const int width, const int height);
	~PlaneTexture();

	unsigned int getRendererID() const { return m_rendererID; }
	void bind(unsigned int slot = 0) const;
	void unbind() const;
};