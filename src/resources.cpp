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
			loadFromBmp(stream);
			break;
		case TexExt::extTga :
			loadFromTga(stream);
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


char * loadText(Stream *stream) {
	return stream->getAnsi();
}

bool Default::isInited;
ShaderProgram *Default::spriteShader;
Texture *Default::blankTexture;

void Default::init() {
	const char *SHADER_SPRITE_VERT = 
		"attribute vec3 vaCoord;"
		"attribute vec2 vaTexCoord0;"
		"attribute vec4 vaColor;"
		"varying vec2 texCoord0;" 
		"varying vec4 vColor;"
		"uniform mat4 uModelViewProj;"

		"void main(void)"
		"{"
		"  texCoord0 = vaTexCoord0;"
		"  vColor = vaColor;"
		"  gl_Position = uModelViewProj * vec4(vaCoord, 1.0);"
		"}";

	const char *SHADER_SPRITE_FRAG = 
		"varying vec2 texCoord0;"
		"varying vec4 vColor;"
		"uniform sampler2D uDiffuse;"
		"uniform vec4 uColor;"

		"void main(void)"
		"{"
		"  gl_FragColor = uColor * vColor * texture2D( uDiffuse, texCoord0 );"
		"  if (gl_FragColor.a < 0.001)"
		"    discard;"
		"}";

	isInited = true;
	
	char *data = new char[3];
	memset(data, 0xFF, 3);
	blankTexture = Texture::init(1, 1, TexFormat::TEX_RGB8, (void *) data);	

	spriteShader = new ShaderProgram();
	spriteShader->attach((void *) SHADER_SPRITE_VERT, ShaderType::stVertex);
	spriteShader->attach((void *) SHADER_SPRITE_FRAG, ShaderType::stFragment);
	spriteShader->link();

	// We don't need to store index
	int index;	
	spriteShader->setUniform(utMat4, 1, &Render::params.mModelViewProj, "uModelViewProj", index = -1);	
	spriteShader->setUniform(utVec4, 1, &Render::params.color, "uColor", index = -1);
}

void Default::deinit() {
	if (!isInited)
		return;
	delete blankTexture;
	delete spriteShader;
}