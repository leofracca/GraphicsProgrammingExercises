#include "Texture.h"

#include "stb_image/stb_image.h"

#include <glad/glad.h>
#include <iostream>

Texture::Texture(const std::string& filePath, int param, bool transparent)
	: m_filePath(filePath), m_localBuffer(nullptr), m_width(0), m_height(0), m_bitsPerPixel(0)
{
    stbi_set_flip_vertically_on_load(1);
    m_localBuffer = stbi_load(m_filePath.c_str(), &m_width, &m_height, &m_bitsPerPixel, 0);

    // std::cout << m_width << " " << m_height << std::endl;

	glGenTextures(1, &m_rendererID);
	glBindTexture(GL_TEXTURE_2D, m_rendererID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (m_localBuffer)
    {
        if (transparent)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_localBuffer);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(m_localBuffer);
    }
    else
        std::cout << "Failed to load texture" << std::endl;
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_rendererID);
}

void Texture::bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_rendererID);
}

void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
