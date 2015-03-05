#ifndef RESOURCES_H
#define RESOURCES_H

#include "utils.h"
#include "render.h"
#include "render2d.h"

struct TextureRes {
private:
	void loadFromBmp(Stream *stream);
	void loadFromTga(Stream *stream);
	void loadUncompressedTga(Stream *stream, int bytesPP);
	void loadRleCompressedTga(Stream *stream, int bytesPP);

	void copySwapedPixelData(const unsigned char *src, char *dst);
public:
	TexFormat format;
	char *data;
	int width, height, size;	

	TextureRes(Stream *stream, TexExt ext);
	~TextureRes();
};

struct FontRes {
	TextureRes *texture;
	CharData *data;
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