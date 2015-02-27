#include "render2d.h"
#include "resources.h"

Sprite::Sprite(float width, float height, const vec2 &pivot) 
	: m_width(width), m_height(height), m_pivot(pivot.x, pivot.y) { 
	
	resetVertices();  
	resetTexCoords();
	setVerticesColor(vec4(1.0f));
}

void Sprite::setRotation(float rotation) {
	if (!_equalf(rotation, m_rot)) {
		m_rot = rotation;
		model.identity();
		model.rotate(m_rot * _DEG2RAD, vec3(0, 0, 1));
		if (m_rot > 360)
			m_rot -= 360;
		else if (m_rot < -360)
			m_rot += 360;
	}
}

void Sprite::setPivot(vec2 &pivot) {
	if (pivot != m_pivot) {
		m_pivot = pivot;
		resetVertices();
	}
}

void Sprite::setWidth(float width) {
	if (width > 0 && !_equalf(m_width, width)) {
		m_width = width;
		resetVertices();
	}
}

void Sprite::setHeight(float height) {
	if (height > 0 && !_equalf(m_height, height)) {
		m_height = height;
		resetVertices();
	}
}

void Sprite::setSize(float width, float height) {
	m_width = width;
	m_height = height;
	resetVertices();
}

void Sprite::resetVertices() {
	vertices[0].pos = vec3((vec2(1.0f, 1.0f) - m_pivot) * vec2(m_width, m_height), 0);
	vertices[1].pos = vec3((vec2(1.0f, 0.0f) - m_pivot) * vec2(m_width, m_height), 0);
	vertices[2].pos = vec3((                 - m_pivot) * vec2(m_width, m_height), 0);
	vertices[3].pos = vec3((vec2(0.0f, 1.0f) - m_pivot) * vec2(m_width, m_height), 0);
}

void Sprite::resetTexCoords() {	
	vertices[0].tc = vec2(1, 1);
	vertices[1].tc = vec2(1, 0);
	vertices[2].tc = vec2(0, 0);
	vertices[3].tc = vec2(0, 1);   
}

void Sprite::setTextureRegion(TextureRegion *region, bool adjustSpriteSize) {
	if (!region->rotated) {
		vertices[0].tc = vec2(region->tx + region->tw, region->ty + region->th);
		vertices[1].tc = vec2(region->tx + region->tw, region->ty);
		vertices[2].tc = vec2(region->tx,			   region->ty);
		vertices[3].tc = vec2(region->tx,              region->ty + region->th);
		if (adjustSpriteSize)
			setSize(region->tw * region->texture->width, region->th * region->texture->height);
	}
	else {
		vertices[0].tc = vec2(region->tx,              region->ty + region->th);
		vertices[1].tc = vec2(region->tx + region->tw, region->ty + region->th);
		vertices[2].tc = vec2(region->tx + region->tw, region->ty);
		vertices[3].tc = vec2(region->tx,              region->ty);
		if (adjustSpriteSize)
			setSize(region->th * region->texture->height, region->tw * region->texture->width);
	}
}


// SpriteBatch ----------------------------------------------
void SpriteBatch::begin() {
	m_count = 0;
}

void SpriteBatch::end() {
	if (m_count == 0)
		return;

	vb->update(&m_vData[0], 0, m_count * 4);
	ib->update(&m_iData[0], 0, m_count * 6);
	Render::params.mModelViewProj = Render::params.mViewProj;
	Render::drawTriangles(ib, vb, 0, m_count * 6);
}

void SpriteBatch::render(Sprite *sprite) {
	static const unsigned int spriteIndices[6] = { 0, 1, 2, 2, 3, 0 };

	if (!sprite->visible())
		return;
	
	memcpy(&m_vData[m_count * 4], &sprite->vertices[0], sizeof(Vertex_PTC_324) * 4);
	mat4 absMatrix = sprite->absMatrix();
	for (int i = 0; i < 4; i++)
		m_vData[m_count * 4 + i].pos = absMatrix * m_vData[m_count * 4 + i].pos;

	for (int i = 0; i < 6; i++)
		m_iData[m_count * 6 + i] = spriteIndices[i] + m_count * 4;

	m_count++;
}

// Font ------------------------------------------------------

Font* Font::init(Stream *stream, bool freeStreamOnFinish) {
	Font *f = new Font();	
	f->material = Material::init(Default::isInited ? Default::spriteShader : NULL);
	
	FontRes *fRes = new FontRes(stream);
	TextureRes *tRes = fRes->texture;

	f->texture = Texture::init(tRes->width, tRes->height, tRes->format, tRes->data);

	f->charData = (CharData *)fRes->data;
	f->m_charCount = fRes->charCount;

	for (int i = 0; i < f->m_charCount; i++) {
		f->table[f->charData[i].id] = &f->charData[i];
		if (f->charData[i].h > f->maxCharHeight)
			f->maxCharHeight = f->charData[i].h;
	}

	delete fRes;
	
	if (freeStreamOnFinish)
		delete stream;

	return f;
}