#pragma once

#include <glew.h>
#include "Shader.h"
#include "Vertex.h"
#include <QGLWidget>
#include <QMouseEvent>
#include <QTimer>
#include "Camera.h"
#include "ShadowMapping.h"
#include "RenderManager.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QTextStream>
#include <vector>
#include "VanishingPoint.h"
#include "CGA.h"

class MainWindow;

class GLWidget3D : public QGLWidget {
public:
	GLWidget3D(MainWindow *parent = 0);

	void updateStatusBar();
	void drawScene();
	void render();
	void loadImage(const QString& filename);
	void clearLines();
	void loadLines(const QString& filename);
	void saveLines(const QString& filename);
	void loadSilhouette(const QString& filename);
	void saveSilhouette(const QString& filename);
	void undo();
	void computeVanishingPoint();
	void computeCameraMatrix();
	void reconstruct3D();
	void renderImage(cga::CGA& cga, cga::Grammar& grammar, const std::vector<float>& pm_params, cv::Mat& rendered_image);
	double distanceMap(cv::Mat rendered_image, const cv::Mat& reference_dist_map);
	void setupGeometry(cga::Grammar& grammar, const std::vector<float>& pm_params, std::vector<boost::shared_ptr<glutils::Face>>& faces);
	void updateGeometry();

	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

protected:
	void initializeGL();
	void resizeGL(int width, int height);
	//void paintGL();
	void paintEvent(QPaintEvent* e);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent* e);

public:
	static enum { RENDERING_MODE_REGULAR = 0, RENDERING_MODE_LINE };
	static enum { PEN_TYPE_VANISHING_LINE = 0, PEN_TYPE_SILHOUETTE };

public:
	MainWindow* mainWin;
	Camera camera;
	glm::vec3 light_dir;
	glm::mat4 light_mvpMatrix;
	bool shiftPressed;
	bool ctrlPressed;
	bool altPressed;

	RenderManager renderManager;
	std::vector<cga::Grammar> grammars;
	int grammar_id;
	std::vector<float> pm_params;
	//std::vector<float> cuboid_size;
	std::vector<vp::VanishingLine> lines;
	std::vector<vp::VanishingLine> silhouette;
	glm::dvec2 origin;

	QImage image;

	int pen_type;
	int contourLineWidth;
	QColor horizontalLeftColor;
	QColor horizontalRightColor;
	QColor verticalColor;
	int silhouetteWidth;
	QColor silhouetteColor;
};

