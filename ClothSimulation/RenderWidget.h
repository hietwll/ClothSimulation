#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <glad/glad.h>
#include <QtOpenGL/QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QTime> 

#include "Scene.h"
#include "Mesh.h"
#include "Simulation.h"


class RenderWidget : public QGLWidget
{
	Q_OBJECT
public:
	explicit RenderWidget(QWidget *parent = 0);
	~RenderWidget();

	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);
	void updateCamera();

	void initializeMember();

	MyMesh* mesh;
	Simulation* simulation;
	Scene* scene;

	QTimer timer;
	QTime  clock;
	float tcloth, trecord;

	QPoint lastPos, curPos, startPos;

	bool wire_frame = false;

	void keyPressEvent(QKeyEvent *e);
	void wheelEvent(QWheelEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent * e);

	glm::vec3 pick_pos;
	int pick_id;
	bool first_pick;
	float fix_release_time;


private slots:
	void updatePosition();
};


#endif // !RENDERWIDGET_H
