#include "resources.h"
#include "utils.h"
#include "render.h"

void TextureRes::loadFromBmp(Stream *stream) {
}

void TextureRes::loadFromTga(Stream *stream) {
}

TextureRes::TextureRes(Stream *stream, TexExt ext) {
	switch (ext) {
		case TexExt::extBmp :
			break;
		case TexExt::extTga :
			break;
		default:
			return;
	}
}

TextureRes::~TextureRes() {
}




FontRes::FontRes(Stream *stream) {

}

FontRes::~FontRes() {

}


char * loadText(const Stream *stream) {
	return NULL;
}

bool Default::isInited;
ShaderProgram *Default::spriteShader;

void Default::init() {
	isInited = true;
}

void Default::deinit() {
	if (!isInited)
		return;
}