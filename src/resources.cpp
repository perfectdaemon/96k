#include "resources.h"
#include "utils.h"
#include "render.h"

void TextureRes::loadFromBmp(Stream *stream) {	
	BITMAPFILEHEADER *fileHeader = (BITMAPFILEHEADER *) stream->getData(sizeof(BITMAPFILEHEADER));
	BITMAPINFOHEADER *infoHeader = (BITMAPINFOHEADER *) stream->getData(sizeof(BITMAPINFOHEADER));

	if (infoHeader->biClrUsed) {
		LOG("BMP load: color map is not supported\n");
		return;
	}

	width = infoHeader->biWidth;
	height = infoHeader->biHeight;

	// if height is negative then bitmap is upside down
	int absHeight = abs(height);
	bytesPP = infoHeader->biBitCount / 8;	
	switch (bytesPP) {
		case 3: format = TexFormat::TEX_BGR8; break;
		case 4: format = TexFormat::TEX_BGRA8; break;
	}

	size = width * absHeight * bytesPP;
	data = new char[size];

	int sLength = width * bytesPP;
	int fLength = 0;
	if ((sLength % 4) > 0)
		fLength = ((sLength / 4) + 1) * 4 - sLength;	
	
	int pos = 0;
	for (int i = 0; i < absHeight; i++) {
		memcpy(&data[pos], stream->getData(sLength), sLength);		
		stream->seek(fLength);
		pos += sLength;
	}
	
	// flip surface if needed
	if (height > 0) 
		flipImage();	
	else
		height = -height;
}

#pragma pack(push, 1)
typedef struct {  
	char IDLength;
	char ColorMapType;
	char ImageType;
	short int ColorMapOrigin;
	short int ColorMapLength;
	char ColorMapEntrySize;
	short int XOrigin;
	short int YOrigin;
	short Width;
	short Height;
	char PixelSize;
	char ImageDescriptor;
} TGAHeader;
#pragma pack(pop)

void TextureRes::loadFromTga(Stream *stream) {
	TGAHeader *header = (TGAHeader *)stream->getData(sizeof(TGAHeader));

	width = header->Width;
	height = header->Height;
	
	int colorDepth = header->PixelSize;

	if (colorDepth != 24 && colorDepth != 32) {
		LOG("TGA load: color depth differs from 24 or 32\n");
		return;
	}

	if (header->ColorMapType) {
		LOG("TGA load: ColorMap is not supported\n");
		return;
	}

	bytesPP = colorDepth / 8;

	switch (bytesPP) {
		case 3: format = TexFormat::TEX_RGB8; break;
		case 4: format = TexFormat::TEX_RGBA8; break;
	}
	
	size = width * height * bytesPP;
	data = new char[size];	

	switch (header->ImageType) {
		case 2: 
			loadUncompressedTga(stream);
			break;
		case 10: 
			loadRleCompressedTga(stream);
			break;
		default:
			LOG("TGA load: uncompressed and RLE-compressed files are only supported\n");
			return;
	}

	// flip
	flipImage();
}

void TextureRes::loadUncompressedTga(Stream *stream) {
	memcpy(data, stream->getData(size), size);	
	
	// flip bgr(a) to rgb(a)
	for (int i = 0; i < width * height; i++) {
		char *blue = &data[i * bytesPP + 0];
		char *red  = &data[i * bytesPP + 2];
		char tmp = *blue;
		*blue = *red;
		*red = tmp;
	}
}

void TextureRes::copySwapedPixelData(const unsigned char *src, char *dst) {
	dst[0] = src[2];
	dst[1] = src[1];
	dst[2] = src[0];
	dst[3] = src[3];
}

void TextureRes::flipImage() {
	int lineSize = bytesPP * width;
	int sliceSize = lineSize * height;
	char *tmpBuf = new char[lineSize];

	char *top = data;
	char *bottom = data;
	bottom += sliceSize - lineSize;

	for (int i = 0; i < (height / 2); i++) {
		// swap lines
		memcpy(tmpBuf, top,    lineSize);
		memcpy(top,    bottom, lineSize);
		memcpy(bottom, tmpBuf, lineSize);

		top    += lineSize;
		bottom -= lineSize;
	}
	delete [] tmpBuf;
}

void TextureRes::loadRleCompressedTga(Stream *stream) {
	int currentByte = 0;
	int currentPixel = 0;
	int bufferIndex = 0;

	unsigned char *compressed = new unsigned char[stream->size - sizeof(TGAHeader)];
	memcpy(compressed, 
		stream->getData(stream->size - sizeof(TGAHeader)), 
		stream->size - sizeof(TGAHeader));	

	do {
		unsigned char *first = compressed + bufferIndex++;
		if (*first < 128)  { // unpacked data
			for (int i = 0; i <= *first; i++) {
				copySwapedPixelData(compressed + bufferIndex + i * bytesPP, data + currentByte);
				currentByte += bytesPP;
				currentPixel++;
			}
			bufferIndex += (*first + 1) * bytesPP;
		}
		else { // packed data
			for (int i = 0; i <= *first - 128; i++) {
				copySwapedPixelData(compressed + bufferIndex, data + currentByte);
				currentByte += bytesPP;
				currentPixel++;
			}
			bufferIndex += bytesPP;
		}
	} while (currentPixel < width * height);

	delete [] compressed;
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
			LOG("Texture load: texture ext unrecognized\n");
			return;
	}
}

// FontRes ----------------------------------------------------
FontRes::FontRes(Stream *stream) : texture(NULL), charCount(0), data(NULL) {	
	// load texture
	this->texture = new TextureRes(stream, TexExt::extBmp);
	
	// make sure that it is a font file - check magic var
	stream->pos = 0;
	BITMAPFILEHEADER *fileHeader = (BITMAPFILEHEADER *) stream->getData(sizeof(BITMAPFILEHEADER));
	BITMAPINFOHEADER *infoHeader = (BITMAPINFOHEADER *) stream->getData(sizeof(BITMAPINFOHEADER));
	if (fileHeader->bfReserved1 != 0x0f86) {
		LOG("Font load: failed, reserved word is incorrect\n");
		return;
	}

	// ok, let's read font data
	stream->pos = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + infoHeader->biSizeImage;

	this->charCount = stream->getUInt();
	this->data = new CharData[charCount];

	memcpy(data, stream->getData(sizeof(CharData) * charCount), sizeof(CharData) * charCount);
}

// Default ----------------------------------------------
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
	blankTexture = Texture::init(1, 1, TexFormat::TEX_RGB8, (void *) data, 3);	

	spriteShader = new ShaderProgram();
	spriteShader->attach((void *) SHADER_SPRITE_VERT, ShaderType::stVertex);
	spriteShader->attach((void *) SHADER_SPRITE_FRAG, ShaderType::stFragment);
	spriteShader->link();

	spriteShader->addUniform(&SHADER_PARAM_INFO[spModelViewProj]);
	spriteShader->addUniform(&SHADER_PARAM_INFO[spColor]);	
}

void Default::deinit() {
	if (!isInited)
		return;
	delete blankTexture;
	delete spriteShader;
}