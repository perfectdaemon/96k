#ifndef RENDER2D_H
#define RENDER2D_H

#include "render.h"
#include "scene.h"
//#include <string>

//using namespace std;

struct Sprite : public Node {
protected:
	float m_rot, m_width, m_height;
	vec2 m_pivot;
public:
	Vertex_PTC_324 vertices[4];

	Sprite(float width, float height, const vec2 &pivot);
	Sprite() :m_width(1.0f), m_height(1.0f), m_pivot(0.5f, 0.5f), m_rot(0) {}
	~Sprite() {}

	virtual float width() { return m_width; }
	virtual float height() { return m_height; }
	virtual float rotation() { return m_rot; }
	virtual vec2& pivot() { return m_pivot; }
	
	virtual void setRotation(float rotation);	
	virtual void setPivot(vec2 &pivot);
	virtual void setWidth(float width);
	virtual void setHeight(float height);
	virtual void setSize(float width, float height);
	virtual void setSize(vec2 &size) { setSize(size.x, size.y); }

	virtual void resetVertices();
	virtual void resetTexCoords();

	virtual void setVerticesColor(vec4 &color) { for (int i = 0; i < 4; vertices[i++].col = color) ; }

	virtual void setTextureRegion(TextureRegion *region, bool adjustSpriteSize = true);

	virtual void render() { }
};

struct SpriteBatch {
private:
	VertexBuffer *vb;
	IndexBuffer *ib;
	int m_count;
	Vertex_PTC_324 m_vData[65536];
	unsigned short m_iData[65536];
public:
	SpriteBatch() : 
		vb(new VertexBuffer(NULL, 65536, VertexFormat::VF_PTC_324)), 
		ib(new IndexBuffer(NULL, 65536, IndexFormat::IF_SHORT)),
		m_count(0) { }
	~SpriteBatch() { delete vb; delete ib; }
	
	void begin();
	void end();

	void render(Sprite *sprite);
	void render(Sprite **sprites, int count) { for (int i = 0; i < count; render(sprites[i++])) ; }
};

struct CharData {
	float tx, ty, tw, th;
	unsigned short py, w, h;
	unsigned char id;
	char _pad;
};

struct Font {
private:
	CharData *charData;
	unsigned int m_charCount;	
	Texture *texture;	
public:
	unsigned int maxCharHeight;
	CharData *table[256];
	Material *material;

	Font() : maxCharHeight(0), m_charCount(0), charData(NULL), texture(NULL), material(NULL) { }
	~Font() { 
		delete [] charData;
		delete texture;
		delete material;
	}

	static Font* init(Stream *stream, bool freeStreamOnFinish = true);

	Quad_PTC_324 getCharQuad(uchar c, float scale);
};

struct Text : public Node {
private:	
	float m_textWidth;	
public:
	Text() :
		lineSpacing(2.0f), 
		letterSpacing(1.0f), 
		color(vec4(1.0f)), 
		pivot(vec2(0.0f)),
		m_textWidth(-1.0f),
		textWidthChanged(false),
		scale(1.0f) { 		
			
		}
	~Text() {}

	ShortString text;

	float letterSpacing, lineSpacing;
	vec4 color;
	float scale;
	vec2 pivot;	

	float textWidth() { return m_textWidth; }
	void setTextWidth(float width) {
		if (_equalf(width, m_textWidth))
			return;
		m_textWidth = width;
		textWidthChanged = true;
	}
	bool textWidthChanged;	
};

struct FontBatch {
private:
	VertexBuffer *vb;
	IndexBuffer *ib;
	int m_count;
	Vertex_PTC_324 m_vData[65536];
	unsigned short m_iData[65536];
	Font *m_font;

	vec2 getTextOrigin(const Text *text);
	void wordWrapText(Text *text);
public:
	FontBatch(Font *font) : m_font(font),
		vb(new VertexBuffer(NULL, 65536, VertexFormat::VF_PTC_324)), 
		ib(new IndexBuffer(NULL, 65536, IndexFormat::IF_SHORT)),
		m_count(0) { }
	~FontBatch() { delete vb; delete ib; }
	
	void begin();
	void end();

	void render(Text *text);
	void render(Text **texts, int count) { for (int i = 0; i < count; render(texts[i++])) ; }
};

#endif // RENDER2D_H