#include "scene.h"
#include "render.h"

// Node -------------------------------------

void Node::setVectorsFromModel() {
	m_dir = vec3(model.e20, model.e21, model.e22);
	m_up = vec3(model.e10, model.e11, model.e12);
	m_right = vec3(model.e00, model.e01, model.e02);
}

void Node::setModelFromVectors(const vec3 &up, const vec3 &right, const vec3 &dir) {  
	model.e00 = right.x; model.e01 = right.y; model.e02 = right.z; model.e03 = position.dot(right);
	model.e10 = up.x;    model.e11 = up.y;    model.e12 = up.z;    model.e13 = position.dot(up);
	model.e20 = dir.x;   model.e21 = dir.y;   model.e22 = dir.z;   model.e23 = position.dot(dir);
	model.e30 = 0;       model.e31 = 0;       model.e32 = 0;       model.e33 = 1;
  
	m_right = right;
	m_up    = up;
	m_dir   = dir;
}

Node::Node() {
	m_parent = NULL;
	m_visible = true;	
	setVectorsFromModel();
}

void Node::setDir(vec3 value) {
	value.normalize();
	if (m_dir == value)
		return;
	
	vec3 newRight = - m_up.cross(value).normal();	
	vec3 newUp = value.cross(newRight).normal();
	setModelFromVectors(newUp, newRight, value);
}

void Node::setRight(vec3 value) {
	value.normalize();
	if (m_right == value)
		return;
	
	vec3 newDir = value.cross(m_up).normal();	
	vec3 newUp = newDir.cross(value).normal();
	setModelFromVectors(newUp, value, newDir);
}

void Node::setUp(vec3 value) {
	value.normalize();
	if (m_up == value)
		return;
	
	vec3 newRight = - value.cross(m_dir).normal();	
	vec3 newDir = newRight.cross(value).normal();
	setModelFromVectors(value, newRight, newDir);
}

mat4 Node::absMatrix() {
	model.setPos(position);
	if (m_parent)
		return m_parent->absMatrix() * model;
	else
		return model;
}
void Node::render() {
	if (!m_visible)
		return;

	Render::params.mModel = absMatrix();
	Render::params.calculateMVP();
	
	doRender();
}
