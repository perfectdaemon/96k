#ifndef RESOURCES_H
#define RESOURCES_H

#include "utils.h"
#include "render.h"
#include "render2d.h"

struct TextureRes {
private:
	void loadFromBmp(Stream *stream);
	void loadFromTga(Stream *stream);
	void loadUncompressedTga(Stream *stream);
	void loadRleCompressedTga(Stream *stream);

	void flipImage();
	static void copySwapedPixelData(const unsigned char *src, char *dst);
public:
	TexFormat format;
	char *data;
	int width, height, size, bytesPP;	

	TextureRes(Stream *stream, TexExt ext);
	~TextureRes() { delete [] data; }
};

struct FontRes {
	TextureRes *texture;
	CharData *data;
	unsigned int charCount;

	FontRes(Stream *stream);
	~FontRes() { delete [] data; delete texture; }
};

struct MeshRes {

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