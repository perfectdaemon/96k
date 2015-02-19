#ifndef RENDER2D_H
#define RENDER2D_H

#include "render.h"
#include "scene.h"

struct Sprite : public Node {
protected:
	float m_rot, m_width, m_height;
	vec2 m_pivot;
public:
	Vertex_PTC_324 vertices[4];

	Sprite(float width, float height, const vec2 &pivot) : m_width(width), m_height(height), m_pivot(pivot.x, pivot.y) {}
	Sprite() : m_width(1.0f), m_height(1.0f), m_pivot(0.5f, 0.5f) {}
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
	
	virtual void begin();
	virtual void end();

	virtual void render(Sprite *sprite);
	virtual void render(Sprite **sprites, int count);// { for (int i = 0; i < count; render(sprites[i++])) ; }
};

#endif // RENDER2D_H