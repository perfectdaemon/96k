#ifndef RESOURCES_H
#define RESOURCES_H


struct TextureRes {
	TexFormat format;
	void *data;
	int width, height;	

	TextureRes(Stream stream, TexExt ext);
	~TextureRes();
};

struct FontRes {
	TextureRes *texture;
	void *data;
	int charCount;

	FontRes(Stream stream);
	~FontRes();
};

char * loadText(const Stream stream);

//function LoadFontData(const Stream: TglrStream; out CharCount: LongWord): Pointer;

//function LoadText(const Stream: TglrStream): PAnsiChar;

#endif // RESOURCES_H