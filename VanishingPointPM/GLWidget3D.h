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
	static enum { PEN_TYPE_VANISHING_LINE = 0, PEN_TYPE_SILHOUETTE };

public:
	MainWindow* mainWin;

	// camera
	Camera camera;
	glm::vec3 light_dir;
	glm::mat4 light_mvpMatrix;

	// rendering engine
	RenderManager renderManager;

	// key status
	bool shiftPressed;
	bool ctrlPressed;
	bool altPressed;

	// grammars
	std::vector<cga::Grammar> grammars;
	int grammar_id;
	std::vector<float> pm_params;

	// GUI options
	float opacityOfBackground;
	bool image_loaded;
	QImage image;
	int pen_type;
	std::vector<vp::VanishingLine> lines;
	glm::dvec2 origin;
	int lineWidth;
	QColor horizontalLeftColor;
	QColor horizontalRightColor;
	QColor verticalColor;
	std::vector<vp::VanishingLine> silhouette;
	int silhouetteWidth;
	QColor silhouetteColor;

public:
	GLWidget3D(MainWindow *parent = 0);

	void drawScene();
	void render();
	void clearBackground();
	void loadImage(const QString& filename);
	void clearLines();
	void loadLines(const QString& filename);
	void saveLines(const QString& filename);
	void clearSilhouette();
	void loadSilhouette(const QString& filename);
	void saveSilhouette(const QString& filename);
	void undo();
	void computeVanishingPoint();
	void computeCameraMatrix();
	void reconstruct3DAll();
	std::vector<float> reconstruct3D();
	void renderImage(cga::Grammar& grammar, const std::vector<float>& pm_params, cv::Mat& rendered_image);
	double distanceMap(cv::Mat rendered_image, const cv::Mat& reference_dist_map);
	void updateGeometry(cga::Grammar& grammar, const std::vector<float>& pm_params);
	void updateStatusBar();

	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintEvent(QPaintEvent* e);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent* e);
};

