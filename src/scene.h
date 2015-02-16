#ifndef SCENE_H
#define SCENE_H

#include "utils.h"

struct Node {
protected:
	vec3 m_dir, m_right, m_up;
	Node *m_parent;
	bool m_visible;	

	virtual void doRender() { }
	virtual void setVectorsFromModel();
	virtual void setModelFromVectors(const vec3 &up, const vec3 &right, const vec3 &dir);
public:
	mat4 model;
	vec3 position;

	Node();
	~Node() { }

	virtual Node * parent() { return m_parent; }
	virtual void setParent(const Node* node) { m_parent = (Node *)node; }

	virtual bool visible() { return m_visible; }
	virtual void setVisible(bool value) { m_visible = value; }

	virtual vec3 dir() { return m_dir; }
	virtual vec3 right() { return m_right; }
	virtual vec3 up() { return m_up; }
	
	virtual void setDir(vec3 value);
	virtual void setRight(vec3 value);	
	virtual void setUp(vec3 value);

	virtual mat4 absMatrix();	

	virtual void render();
};

enum CameraProjectionMode { pmOrtho, pmPerspective };
enum CameraPivotMode { pTopLeft, pCenter, pBottomRight };

struct Camera : Node {
protected:
	CameraPivotMode pivotMode;
	CameraProjectionMode projMode;
	int m_x, m_y, m_w, m_h;
	float m_FOV, m_zNear, m_zFar, m_scale;
public:
	Camera();
	~Camera();
	
	virtual void setProjParams(int x, int y, int w, int h);
	virtual void setProjParams(int x, int y, int w, int h, 
		float FOV, float zNear, float zFar, 
		CameraProjectionMode projMode,
		CameraPivotMode pivotMode);

	virtual void setViewParams(vec3 selfPos, vec3 targetPos, vec3 up);

	virtual void translate(float alongUp, float alongRight, float anlongDir);
	virtual void rotate(float delta, vec3 axis);
	
	virtual float scale() { return m_scale; }
	virtual void setScale();

	virtual vec2 screenToScene(vec2 screenPosition);

	virtual void update();

	void render();
};

#endif // SCENE_H