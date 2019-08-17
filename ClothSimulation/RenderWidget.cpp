#include "RenderWidget.h"
#include "Drag.h"
#include "SkyBox.h"
#include "Sphere.h"

RenderWidget::RenderWidget(QWidget *parent) : QGLWidget(parent)
{
	setFocusPolicy(Qt::StrongFocus);
	connect(&timer, SIGNAL(timeout()), this, SLOT(updatePosition()));
}

RenderWidget::~RenderWidget()
{
	if (simulation != nullptr) delete simulation;
	if (scene != nullptr)   delete scene;
	// if (mesh != nullptr)   delete mesh;
}

void RenderWidget::initializeGL()
{
	// load glad 
	if (!gladLoadGL()) {
		printf("Something went wrong!\n");
		exit(-1);
	}

	initializeMember();

	scene->InitBuffer();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

void RenderWidget::initializeMember()
{
	std::vector<DrawObj* > vec;

	// mesh
	mesh = new MyMesh();

	// scene
	vec.push_back(mesh);
	vec.push_back(new SkyBox());
	vec.push_back(new SphereDraw(glm::vec3(0.0,-1.0,0.0), glm::vec3(1.0,0.0,0.0),0.5));
	scene = new Scene(new Camera(glm::vec3(0.0f, 1.0f, 7.0f)), vec);

	// simulation
	simulation = new Simulation(mesh,scene);
	simulation->InitSimualtion();

	// clock init
	timer.start(1000.0/10.0);
	clock.start();
	tcloth = 0.0;
	trecord = 0.0;
	pick_pos = glm::vec3(0.0, 0.0, 0.0);
	wire_frame = true;
	first_pick = true;
}



void RenderWidget::paintGL()
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update Camera and Draw
	scene->UpdateCamera(this->width(),this->height());
	scene->Draw();
}

void RenderWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
	updateGL();
}


void RenderWidget::keyPressEvent(QKeyEvent *e)
{
	float deltaTime = 0.1f;

	if (e->key() == Qt::Key_W)
		scene->mycamera->ProcessKeyboard(FORWARD, deltaTime);
	if (e->key() == Qt::Key_S)
		scene->mycamera->ProcessKeyboard(BACKWARD, deltaTime);
	if (e->key() == Qt::Key_A)
		scene->mycamera->ProcessKeyboard(LEFT, deltaTime);
	if (e->key() == Qt::Key_D)
		scene->mycamera->ProcessKeyboard(RIGHT, deltaTime);
	if (e->key() == Qt::Key_Q)
		scene->mycamera->ProcessKeyboard(TOP, deltaTime);
	if (e->key() == Qt::Key_E)
		scene->mycamera->ProcessKeyboard(BOTTOM, deltaTime);

	updateGL();
}

void RenderWidget::mousePressEvent(QMouseEvent *e)
{
	if (e->buttons() == Qt::LeftButton) {
		lastPos = e->pos();
		startPos = e->pos();
	}
	else if (e->buttons() == Qt::MiddleButton)
	{
		float x_ = static_cast<float> ((e->pos()).x());
		float y_ = static_cast<float> ((e->pos()).y());
		float w_ = static_cast<float> (this->width());
		float h_ = static_cast<float> (this->height());

		getDragPoint(scene->proj_view, mesh, ( x_/w_ - 0.5) * 2, (y_/h_ - 0.5) * 2, pick_pos, pick_id);

		if (pick_id >= 0)
		{
			lastPos = e->pos();
			startPos = e->pos();
			scene->proj_view_inv = glm::inverse(scene->proj_view);
			first_pick = true;
		}
	}
}

void RenderWidget::mouseMoveEvent(QMouseEvent *e)
{
	if (e->buttons() == Qt::LeftButton) { // make sure that the leftButton is pressed
		curPos = e->pos();
		scene->mycamera->ProcessMouseMovement(static_cast<float>(curPos.x() - lastPos.x()), static_cast<float>(curPos.y() - lastPos.y()));
		lastPos = curPos;
		updateGL();
	}
	else if (e->buttons() == Qt::MiddleButton && pick_id >= 0)
	{
		curPos = e->pos();

		float x_ = static_cast<float> (curPos.x());
		float y_ = static_cast<float> (curPos.y());
		float w_ = static_cast<float> (this->width());
		float h_ = static_cast<float> (this->height());

		//printf("old pos : %f  %f  %f \n", pick_pos[0], pick_pos[1], pick_pos[2]);

		pick_pos[0] = (x_/w_-0.5)*2.0 ;
		pick_pos[1] = -(y_/h_-0.5)*2.0 ; 

		//printf("new pos : %f  %f  %f \n", pick_pos[0], pick_pos[1], pick_pos[2]);

		glm::vec4 new_pos = scene->proj_view_inv * glm::vec4(pick_pos, 1.0);

		mesh->mesh_position[3 * pick_id + 0] = new_pos[0] / new_pos[3];
		mesh->mesh_position[3 * pick_id + 1] = new_pos[1] / new_pos[3];
		mesh->mesh_position[3 * pick_id + 2] = new_pos[2] / new_pos[3];

		if (first_pick)
		{
			simulation->UpdateMass(pick_id,5000.0);
			simulation->CholeskyDecomp();
			first_pick = false;
		}

		lastPos = curPos;
		updateGL();
	}
}

void RenderWidget::mouseReleaseEvent(QMouseEvent * e)
{
	if (pick_id >= 0)
	{
		printf(" mouse release , pick_id %d , mass %f :", pick_id, mesh->mass[pick_id]);
		simulation->UpdateMass(pick_id, mesh->mass[pick_id]);
		simulation->CholeskyDecomp();
	
		pick_id = -1;
		updateGL();
	}
}


void RenderWidget::wheelEvent(QWheelEvent *e)
{
	scene->mycamera->ProcessMouseScroll(e->delta()*0.01);
	updateGL();
}

void RenderWidget::updatePosition()
{
	tcloth = trecord + clock.elapsed()*0.01;
	simulation->updateLocation(tcloth);
	updateGL();
}

