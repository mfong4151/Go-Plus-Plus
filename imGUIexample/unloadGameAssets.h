//Unused header file for uploading game assets, images, sounds, etc. To be expanded
#pragma once
#include <stb_image.h>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION

//Helper function for loading images into OpenGL texture
// see https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples#Example-for-OpenGL-users 
// for an explaination and original code

bool LoadtextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;
	//Create an OpenGl texture identifier

	Glunit image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	//Setup filter paramters for display

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EM SCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

#endif

	glTexImage2d(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}