#ifndef RESOURCES_H
#define RESOURCES_H

#include "utils.h"
#include "render.h"

struct TextureRes {
private:
	void loadFromBmp(Stream *stream);
	void loadFromTga(Stream *stream);
public:
	TexFormat format;
	void *data;
	int width, height;	

	TextureRes(Stream *stream, TexExt ext);
	~TextureRes();
};

struct FontRes {
	TextureRes *texture;
	void *data;
	int charCount;

	FontRes(Stream *stream);
	~FontRes();
};

struct Default {
public:
	static bool isInited;
	static ShaderProgram *spriteShader;
	static Texture *blankTexture;

	static void init();
	static void deinit();	
};

#endif // RESOURCES_H