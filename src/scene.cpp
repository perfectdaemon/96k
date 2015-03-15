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
	model.identity();
	position = vec3(0.0f);
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

// Camera --------------------------------------------------

void Camera::rebuildProjMatrix() {
	m_projMatrix.identity();
	switch (m_projMode) {
		case pmPerspective:	
			m_projMatrix.perspective(m_FOV, (float)m_w / m_h, m_zNear, m_zFar);
			break;
		case pmOrtho:
			switch (m_pivotMode) {
				case pTopLeft:
					m_projMatrix.ortho(0, m_w / m_scale, m_h / m_scale, 0, m_zNear, m_zFar);
					break;
				case pCenter:
					m_projMatrix.ortho(-m_w / (2 * m_scale), m_w / (2 * m_scale), m_h / (2 * m_scale), -m_h / (2 * m_scale), m_zNear, m_zFar);
					break;
				case pBottomRight:
					m_projMatrix.ortho(-m_w / (2 * m_scale), 0, 0, -m_h / (2 * m_scale), m_zNear, m_zFar);
					break;
			}
			break;
	}
}

Camera::Camera() : Node() {
	m_scale = 1.0f;
	setProjParams(0, 0, Render::width, Render::height, 45.0f, 0.01f, 100.0f, pmOrtho, pTopLeft);
	setViewParams(vec3(0.0f, 0.0f, 100.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
}

	
void Camera::setProjParams(int x, int y, int w, int h) {
	m_x = x;
	m_y = y;
	m_w = w > 0 ? w : 1;
	m_h = h > 0 ? h : 1;
	rebuildProjMatrix();
}

void Camera::setProjParams(int x, int y, int w, int h, 
	float FOV, float zNear, float zFar, 
	CameraProjectionMode projMode,
	CameraPivotMode pivotMode) {
	
	m_FOV = FOV;
	m_zNear = zNear;
	m_zFar = zFar;

	m_projMode = projMode;
	m_pivotMode = pivotMode;

	setProjParams(x, y, w, h);
}

void Camera::setViewParams(vec3 &selfPos, vec3 &targetPos, vec3 &up) {
	model.identity();
	m_up = up.normal();
	m_dir = (targetPos - selfPos).normal();
	m_right = m_dir.cross(m_up).normal();
	m_up = m_right.cross(m_dir).normal();		// wtf?
	position = selfPos;

	m_dir = -m_dir;

	model.e00 = m_right.x; model.e10 = m_right.y; model.e20 = m_right.z; model.e30 = -m_right.dot(position);
	model.e01 = m_up.x;    model.e11 = m_up.y;    model.e21 = m_up.z;    model.e31 = -m_up.dot(position);
	model.e02 = m_dir.x;   model.e12 = m_dir.y;   model.e22 = m_dir.z;   model.e32 = -m_dir.dot(position);
	model.e03 = 0;		   model.e13 = 0;         model.e23 = 0;         model.e33 = 1;

	model = model.transpose();

	setVectorsFromModel();
}

void Camera::translate(float alongUp, float alongRight, float alongDir) {
	position += m_up * alongUp + 
		m_right * alongRight +
		m_dir * alongDir;	
}

void Camera::rotate(float delta, vec3 &axis) {
	model.rotate(delta, axis);
	setVectorsFromModel();
}
	
void Camera::setScale(float scale) {
	if (_equalf(scale, m_scale))
		return;
	m_scale = scale;
	rebuildProjMatrix();
}

vec2 Camera::screenToScene(vec2 &screenPosition) {
	vec2 result;
	switch (m_projMode) {
		case pmOrtho:			
			result = vec2(position.x, position.y) + screenPosition / m_scale;
			switch (m_pivotMode) {				
				case pCenter:		result -= vec2(m_w / 2, m_h / 2) / m_scale; break;
				case pBottomRight:	result -= vec2(m_w,     m_h)     / m_scale; break;
				case pTopLeft:													break;
			}
			
			return result;

		case pmPerspective:
			LOG("camera: screenToScene with perpective camera is not implemented");
			return vec2(0.0f);
		default:
			return vec2(0.0f);
	}
}

void Camera::update() {
	model.setPos(vec3(0.0f));
	model.setPos(model * (-position) );
	Render::params.mViewProj = m_projMatrix * model;
	Render::params.mModelViewProj = Render::params.mViewProj;

	// Do we need this?
	if (Render::width != m_w || Render::height != m_h)
		this->setProjParams(0, 0, Render::width, Render::height);
}