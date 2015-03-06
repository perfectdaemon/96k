#include "render2d.h"
#include "resources.h"

Sprite::Sprite(float width, float height, const vec2 &pivot) 
	: m_width(width), m_height(height), m_pivot(pivot.x, pivot.y), m_rot(0) { 
	
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
			setSize((region->tw) * (region->texture->width), (region->th) * (region->texture->height));
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
	
	FontRes *fRes = new FontRes(stream);
	TextureRes *tRes = fRes->texture;

	f->texture = Texture::init(tRes->width, tRes->height, tRes->format, tRes->data, tRes->size);
	f->material = Material::init(Default::isInited ? Default::spriteShader : NULL);
	f->material->addTexture(f->texture, "uDiffuse", 0);

	f->m_charCount = fRes->charCount;
	f->charData = new CharData[f->m_charCount];
	memcpy(f->charData, fRes->data, sizeof(CharData) * f->m_charCount);

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

Quad_PTC_324 Font::getCharQuad(uchar c, float scale) {
	Quad_PTC_324 q;
	memset(&q, 0, sizeof(Quad_PTC_324));
	CharData *cd = table[c];
	if (!cd)
		return q;	
	
	q.v[0].pos = vec3(cd->w, cd->py + cd->h, 0) * scale;
	q.v[1].pos = vec3(cd->w, cd->py,		 0) * scale;
	q.v[2].pos = vec3(0,	 cd->py,		 0) * scale;
	q.v[3].pos = vec3(0,	 cd->py + cd->h, 0) * scale;

	q.v[0].tc = vec2(cd->tx + cd->tw, cd->ty + cd->th);
	q.v[1].tc = vec2(cd->tx + cd->tw, cd->ty		 );
	q.v[2].tc = vec2(cd->tx,		  cd->ty		 );
	q.v[3].tc = vec2(cd->tx,		  cd->ty + cd->th);

	return q;
}


// FontBatch ---------------------------------------------------

vec2 FontBatch::getTextOrigin(const Text *text) {
	float maxWidth = 0.0f;
	vec2 textSize(0.0f, m_font->maxCharHeight + text->lineSpacing);

	for (int i = 0; i < text->text.length(); i++) {
		unsigned char ch = text->text[i];
		CharData *c = m_font->table[ch];
		
		if (c->id == '\n') {
			textSize.y += m_font->maxCharHeight + text->lineSpacing;
			if (maxWidth > textSize.x)
				textSize.x = maxWidth;
			maxWidth = 0.0f;
			continue;
		}

		maxWidth += c->w + text->letterSpacing;
	}
	if (textSize.x < maxWidth)
		textSize.x = maxWidth;

	textSize *= text->scale;

	return -textSize * text->pivot;
}

void FontBatch::wordWrapText(Text *text) {
	int lastSpace = 0;
	float width = 0.0f;
	int i = 0;
	while (i < text->text.length()) {
		if (!m_font->table[text->text[i]])
			continue;
		
		if (text->text[i] == '\n')
			text->text[i] = ' ';

		if (text->text[i] == ' ')
			lastSpace = i;

		width += m_font->table[text->text[i]]->w * text->scale + text->letterSpacing;
		if (width > text->textWidth() && lastSpace > 0) {
			text->text[lastSpace] = '\n';
			i = lastSpace + 1;
			width = 0.0f;
			lastSpace = 0;
		}
		else
			i++;
	}
}

void FontBatch::begin() {
	m_count = 0;
}

void FontBatch::end() {
	if (m_count == 0)
		return;

	vb->update(&m_vData[0], 0, m_count * 4);
	ib->update(&m_iData[0], 0, m_count * 6);
	Render::params.mModelViewProj = Render::params.mViewProj;
	this->m_font->material->bind();
	Render::drawTriangles(ib, vb, 0, m_count * 6);
}

void FontBatch::render(Text *text) {
	static const unsigned int spriteIndices[6] = { 0, 1, 2, 2, 3, 0 };

	if (!text->visible() || text->text == "")
		return;

	vec2 origin = getTextOrigin(text);
	vec2 start = origin;

	if (text->textWidth() > 0.0f && text->textWidthChanged) {
		wordWrapText(text);
		text->textWidthChanged = false;
	}

	for (int i = 0; i < text->text.length(); i++) {		
		uchar c = text->text[i];
		if (c == '\n') {
			start.x = origin.x;
			start.y += (text->lineSpacing + m_font->maxCharHeight) * text->scale;
			continue;
		}

		if (!m_font->table[c])
			continue;

		Quad_PTC_324 quad = m_font->getCharQuad(c, text->scale);
		mat4 absMatrix = text->absMatrix();
		for (int j = 0; j < 4; j++) {
			m_vData[m_count * 4 + j] = quad.v[j];
			m_vData[m_count * 4 + j].pos += vec3(start.x, start.y, 0);
			m_vData[m_count * 4 + j].pos = absMatrix * m_vData[m_count * 4 + j].pos;
			m_vData[m_count * 4 + j].col = text->color;
		}

		for (int j = 0; j < 6; j++)
			m_iData[m_count * 6 + j] = spriteIndices[j] + m_count * 4;

		start.x += quad.v[0].pos.x + text->letterSpacing;
		m_count++;
	}		
}