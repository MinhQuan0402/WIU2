#include "LoadPNG.h"
#include <iostream>

#pragma warning(push)
#pragma warning(disable : 6262)
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>
#pragma warning(pop)

GLuint LoadPNG(const char* file_path)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	unsigned char* img = stbi_load(file_path, &width, &height, &channels, 0);
	if (img == nullptr)
	{
		throw "Error in loading the image";
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if(channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(img);
	glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}
