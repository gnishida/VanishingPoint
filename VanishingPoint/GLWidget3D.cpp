#include "GLWidget3D.h"
#include "MainWindow.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <map>
#include "GLUtils.h"
#include <QDir>
#include <QTextStream>
#include <iostream>
#include <opencv2/core.hpp>
#include <QFileDialog>

GLWidget3D::GLWidget3D(MainWindow *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers)) {
	this->mainWin = parent;
	shiftPressed = false;
	ctrlPressed = false;
	altPressed = false;
	cuboid_size.resize(3);
	cuboid_size[0] = 20;
	cuboid_size[1] = 20;
	cuboid_size[2] = 10;
	image_offset = glm::dvec2(0, 0);

	// This is necessary to prevent the screen overdrawn by OpenGL
	setAutoFillBackground(false);

	// 光源位置をセット
	// ShadowMappingは平行光源を使っている。この位置から原点方向を平行光源の方向とする。
	light_dir = glm::normalize(glm::vec3(-4, -5, -8));

	// シャドウマップ用のmodel/view/projection行列を作成
	glm::mat4 light_pMatrix = glm::ortho<float>(-50, 50, -50, 50, 0.1, 200);
	glm::mat4 light_mvMatrix = glm::lookAt(-light_dir * 50.0f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	light_mvpMatrix = light_pMatrix * light_mvMatrix;
}

void GLWidget3D::updateStatusBar() {
	QString msg = QString("xrot=%1, yrot=%2, zrot=%3, pos=(%4, %5, %6), fov=%7, size=(%8, %9, %10)").arg(camera.xrot).arg(camera.yrot).arg(camera.zrot).arg(camera.pos.x).arg(camera.pos.y).arg(camera.pos.z).arg(camera.fovy).arg(cuboid_size[0]).arg(cuboid_size[1]).arg(cuboid_size[2]);
	mainWin->statusBar()->showMessage(msg);
}

/**
 * This event handler is called when the mouse press events occur.
 */
void GLWidget3D::mousePressEvent(QMouseEvent *e) {
	if (e->buttons() & Qt::RightButton) { // rotate camera
		camera.mousePress(e->x(), e->y());
	}
	else if (e->buttons() & Qt::MidButton) { // rotate around viewing direction
		camera.mousePress(e->x(), e->y());
	}
	else if (e->buttons() & Qt::LeftButton) { // draw lines
		if (ctrlPressed) {
			origin = glm::vec2(e->x(), e->y());
		}
		else {
			if (shiftPressed) {
				lines.push_back(vp::VanishingLine(e->x(), e->y(), e->x(), e->y(), vp::VanishingLine::TYPE_HORIZONTAL_RIGHT));
			}
			else if (altPressed) {
				lines.push_back(vp::VanishingLine(e->x(), e->y(), e->x(), e->y(), vp::VanishingLine::TYPE_VERTICAL));
			}
			else {
				lines.push_back(vp::VanishingLine(e->x(), e->y(), e->x(), e->y(), vp::VanishingLine::TYPE_HORIZONTAL_LEFT));
			}
		}
		update();
	}
}

/**
 * This event handler is called when the mouse release events occur.
 */
void GLWidget3D::mouseReleaseEvent(QMouseEvent *e) {
	// remove the line if it is just a point
	if (lines.size() > 0 && lines.back().start == lines.back().end) {
		lines.pop_back();
	}
}

/**
 * This event handler is called when the mouse move events occur.
 */
void GLWidget3D::mouseMoveEvent(QMouseEvent *e) {
	if (e->buttons() & Qt::RightButton) { // rotate camera
		if (shiftPressed) { // Move
			camera.move(e->x(), e->y());
		}
		else {
			camera.rotate(e->x(), e->y(), (ctrlPressed ? 0.1 : 1));
		}
	}
	else if (e->buttons() & Qt::MidButton) { // rotate around viewing direction
		camera.rotateAroundZ(e->x(), e->y(), (ctrlPressed ? 0.1 : 1));
	}
	else if (e->buttons() & Qt::LeftButton) { // draw lines
		if (lines.size() > 0) {
			lines.back().end = glm::vec2(e->x(), e->y());
		}
	}
	updateStatusBar();
	update();
}

void GLWidget3D::wheelEvent(QWheelEvent* e) {
	camera.changeFov(e->delta() * 0.05, (ctrlPressed ? 0.1 : 1), width(), height());
	/*
	camera.fovy += e->delta() * 0.1;
	camera.pos.z = cameraDistanceBase / tanf((float)camera.fovy * 0.5 / 180.0f * M_PI);
	camera.updatePMatrix(width(), height());
	*/
	updateStatusBar();
	update();
}

/**
 * This function is called once before the first call to paintGL() or resizeGL().
 */
void GLWidget3D::initializeGL() {
	// init glew
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
	}

	if (glewIsSupported("GL_VERSION_4_2"))
		printf("Ready for OpenGL 4.2\n");
	else {
		printf("OpenGL 4.2 not supported\n");
		exit(1);
	}
	const GLubyte* text = glGetString(GL_VERSION);
	printf("VERSION: %s\n", text);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_3D);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glDisable(GL_TEXTURE_3D);

	glEnable(GL_TEXTURE_2D_ARRAY);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glDisable(GL_TEXTURE_2D_ARRAY);

	////////////////////////////////
	renderManager.init("", "", "", true, 8192);
	renderManager.resize(this->width(), this->height());
	renderManager.renderingMode = RenderManager::RENDERING_MODE_LINE;

	glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex0"), 0);//tex0: 0

	camera.changeFov(0, 1, width(), height());



	//////////////// DEBUG ///////////////////////////////////////////////////////
	camera.pos = glm::vec3(0, 0, 50);
	camera.xrot = -10;
	camera.yrot = 30;
	camera.zrot = 0;
	camera.fovy = 60;
	//////////////// DEBUG ///////////////////////////////////////////////////////



	updateGeometry();

	updateStatusBar();
}

/**
 * This function is called whenever the widget has been resized.
 */
void GLWidget3D::resizeGL(int width, int height) {
	height = height ? height : 1;
	glViewport(0, 0, width, height);
	camera.updatePMatrix(width, height);

	renderManager.resize(width, height);
}

/**
 * This function is called whenever the widget needs to be painted.
 */
void GLWidget3D::paintEvent(QPaintEvent *event) {
	// OpenGLで描画
	makeCurrent();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	render();

	// REMOVE
	glActiveTexture(GL_TEXTURE0);

	// OpenGLの設定を元に戻す
	glShadeModel(GL_FLAT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// draw background image
	QPainter painter(this);
	painter.setOpacity(0.5f);
	painter.drawImage(image_offset.x, image_offset.y, image);

	// draw lines
	painter.setOpacity(1.0f);
	for (auto line : lines) {
		if (line.type == vp::VanishingLine::TYPE_HORIZONTAL_LEFT) {
			painter.setPen(QPen(QColor(0, 0, 192), 3, Qt::SolidLine, Qt::RoundCap));
		}
		else if (line.type == vp::VanishingLine::TYPE_HORIZONTAL_RIGHT) {
			painter.setPen(QPen(QColor(64, 64, 255), 3, Qt::SolidLine, Qt::RoundCap));
		}
		else {
			painter.setPen(QPen(QColor(140, 140, 255), 3, Qt::SolidLine, Qt::RoundCap));
		}
		painter.drawLine(line.start.x, line.start.y, line.end.x, line.end.y);
	}

	// draw origin
	painter.setPen(QPen(QColor(255, 255, 0), 1, Qt::SolidLine));
	painter.setBrush(QBrush(QColor(255, 255, 0)));
	painter.drawEllipse(origin.x - 3, origin.y - 3, 7, 7);

	// draw the center of the building
	glm::vec2 pp = vp::projectPoint(camera.mvpMatrix, glm::dvec3(0, 0, 0));
	painter.setPen(QPen(QColor(255, 0, 0), 1, Qt::SolidLine));
	painter.setBrush(QBrush(QColor(255, 0, 0)));
	painter.drawEllipse((pp.x + 1) * 0.5 * width() - 3, (1 - pp.y) * 0.5 * height() - 3, 7, 7);

	painter.end();

	glEnable(GL_DEPTH_TEST);










#if 0
	/////////////////////// DEBUG //////////////////////////////////////

	glm::vec3 p0(0, 0, 0);
	glm::vec3 p1(-10, 0, 10);
	glm::vec3 p2(10, 0, 10);
	glm::vec3 p3(10, 10, 10);
	glm::vec3 p4(-10, 10, 10);
	glm::vec3 p5(-10, 0, -10);
	glm::vec3 p6(-10, 10, -10);

	glm::vec2 pp0 = vp::projectPoint(camera.mvpMatrix, p0);
	glm::vec2 pp1 = vp::projectPoint(camera.mvpMatrix, p1);
	glm::vec2 pp2 = vp::projectPoint(camera.mvpMatrix, p2);
	glm::vec2 pp3 = vp::projectPoint(camera.mvpMatrix, p3);
	glm::vec2 pp4 = vp::projectPoint(camera.mvpMatrix, p4);
	glm::vec2 pp5 = vp::projectPoint(camera.mvpMatrix, p5);
	glm::vec2 pp6 = vp::projectPoint(camera.mvpMatrix, p6);

	std::vector<vp::VanishingLine> lines;
	lines.push_back(vp::VanishingLine(pp1.x, pp1.y, pp2.x, pp2.y, vp::VanishingLine::TYPE_HORIZONTAL_RIGHT));
	lines.push_back(vp::VanishingLine(pp4.x, pp4.y, pp3.x, pp3.y, vp::VanishingLine::TYPE_HORIZONTAL_RIGHT));
	lines.push_back(vp::VanishingLine(pp1.x, pp1.y, pp5.x, pp5.y, vp::VanishingLine::TYPE_HORIZONTAL_LEFT));
	lines.push_back(vp::VanishingLine(pp4.x, pp4.y, pp6.x, pp6.y, vp::VanishingLine::TYPE_HORIZONTAL_LEFT));
	lines.push_back(vp::VanishingLine(pp5.x, pp5.y, pp6.x, pp6.y, vp::VanishingLine::TYPE_VERTICAL));
	lines.push_back(vp::VanishingLine(pp2.x, pp2.y, pp3.x, pp3.y, vp::VanishingLine::TYPE_VERTICAL));

	std::vector<glm::dvec2> vps;
	vp::computeVanishingPoints(lines, vps);
	
	glm::dmat3 K, R;
	glm::dvec3 T;
	vp::extractCameraMatrix(vps, K, R, T);
	std::cout << "K: " << glm::to_string(K) << std::endl;
	std::cout << "R: " << glm::to_string(R) << std::endl;
	std::cout << "T: " << glm::to_string(T) << std::endl;

	double xrot, yrot, zrot;
	vp::decomposeRotation(R, xrot, yrot, zrot);
	camera.xrot = vp::rad2deg(xrot);
	camera.yrot = vp::rad2deg(yrot);
	camera.zrot = vp::rad2deg(zrot);

	glm::mat3 hoge = vp::composeRotation(0, 30.0 / 180.0 * 3.14159265, 0);
	vp::decomposeRotation(hoge, xrot, yrot, zrot);
	std::cout << "OK" << std::endl;





	glm::mat4 XXX = glm::mat4();
	XXX = glm::rotate(XXX, 30 * (float)M_PI / 180.0f, glm::vec3(0, 1, 0));
	XXX = glm::rotate(XXX, -10 * (float)M_PI / 180.0f, glm::vec3(1, 0, 0));
	std::cout << glm::to_string(XXX) << std::endl;

	/////////////////////// DEBUG //////////////////////////////////////
#endif
}

/**
 * Draw the scene.
 */
void GLWidget3D::drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(true);

	renderManager.renderAll();
}

void GLWidget3D::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PASS 1: Render to texture
	glUseProgram(renderManager.programs["pass1"]);

	glBindFramebuffer(GL_FRAMEBUFFER, renderManager.fragDataFB);
	glClearColor(0.95, 0.95, 0.95, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderManager.fragDataTex[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, renderManager.fragDataTex[1], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, renderManager.fragDataTex[2], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, renderManager.fragDataTex[3], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderManager.fragDepthTex, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, DrawBuffers); // "3" is the size of DrawBuffers
	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("+ERROR: GL_FRAMEBUFFER_COMPLETE false\n");
		exit(0);
	}

	glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["pass1"], "mvpMatrix"), 1, false, &camera.mvpMatrix[0][0]);
	glUniform3f(glGetUniformLocation(renderManager.programs["pass1"], "lightDir"), light_dir.x, light_dir.y, light_dir.z);
	glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["pass1"], "light_mvpMatrix"), 1, false, &light_mvpMatrix[0][0]);

	glUniform1i(glGetUniformLocation(renderManager.programs["pass1"], "shadowMap"), 6);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, renderManager.shadow.textureDepth);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	drawScene();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PASS 2: Create AO
	if (renderManager.renderingMode == RenderManager::RENDERING_MODE_SSAO) {
		glUseProgram(renderManager.programs["ssao"]);
		glBindFramebuffer(GL_FRAMEBUFFER, renderManager.fragDataFB_AO);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderManager.fragAOTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderManager.fragDepthTex_AO, 0);
		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Always check that our framebuffer is ok
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			printf("++ERROR: GL_FRAMEBUFFER_COMPLETE false\n");
			exit(0);
		}

		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);

		glUniform2f(glGetUniformLocation(renderManager.programs["ssao"], "pixelSize"), 2.0f / this->width(), 2.0f / this->height());

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex0"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[0]);

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex1"), 2);
		glActiveTexture(GL_TEXTURE2);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[1]);

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex2"), 3);
		glActiveTexture(GL_TEXTURE3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[2]);

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "depthTex"), 8);
		glActiveTexture(GL_TEXTURE8);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "noiseTex"), 7);
		glActiveTexture(GL_TEXTURE7);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragNoiseTex);

		{
			glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["ssao"], "mvpMatrix"), 1, false, &camera.mvpMatrix[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["ssao"], "pMatrix"), 1, false, &camera.pMatrix[0][0]);
		}

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "uKernelSize"), renderManager.uKernelSize);
		glUniform3fv(glGetUniformLocation(renderManager.programs["ssao"], "uKernelOffsets"), renderManager.uKernelOffsets.size(), (const GLfloat*)renderManager.uKernelOffsets.data());

		glUniform1f(glGetUniformLocation(renderManager.programs["ssao"], "uPower"), renderManager.uPower);
		glUniform1f(glGetUniformLocation(renderManager.programs["ssao"], "uRadius"), renderManager.uRadius);

		glBindVertexArray(renderManager.secondPassVAO);

		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
		glDepthFunc(GL_LEQUAL);
	}
	else if (renderManager.renderingMode == RenderManager::RENDERING_MODE_LINE || renderManager.renderingMode == RenderManager::RENDERING_MODE_HATCHING) {
		glUseProgram(renderManager.programs["line"]);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);

		glUniform2f(glGetUniformLocation(renderManager.programs["line"], "pixelSize"), 1.0f / this->width(), 1.0f / this->height());
		glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["line"], "pMatrix"), 1, false, &camera.pMatrix[0][0]);
		if (renderManager.renderingMode == RenderManager::RENDERING_MODE_LINE) {
			glUniform1i(glGetUniformLocation(renderManager.programs["line"], "useHatching"), 0);
		}
		else {
			glUniform1i(glGetUniformLocation(renderManager.programs["line"], "useHatching"), 1);
		}

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex0"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[0]);

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex1"), 2);
		glActiveTexture(GL_TEXTURE2);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[1]);

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex2"), 3);
		glActiveTexture(GL_TEXTURE3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[2]);

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex3"), 4);
		glActiveTexture(GL_TEXTURE4);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[3]);

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "depthTex"), 8);
		glActiveTexture(GL_TEXTURE8);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "hatchingTexture"), 5);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_3D, renderManager.hatchingTextures);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindVertexArray(renderManager.secondPassVAO);

		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
		glDepthFunc(GL_LEQUAL);
	}
	else if (renderManager.renderingMode == RenderManager::RENDERING_MODE_CONTOUR) {
		glUseProgram(renderManager.programs["contour"]);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);

		glUniform2f(glGetUniformLocation(renderManager.programs["contour"], "pixelSize"), 1.0f / this->width(), 1.0f / this->height());

		glUniform1i(glGetUniformLocation(renderManager.programs["contour"], "depthTex"), 8);
		glActiveTexture(GL_TEXTURE8);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

		glBindVertexArray(renderManager.secondPassVAO);

		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
		glDepthFunc(GL_LEQUAL);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Blur

	if (renderManager.renderingMode == RenderManager::RENDERING_MODE_BASIC || renderManager.renderingMode == RenderManager::RENDERING_MODE_SSAO) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		qglClearColor(QColor(0xFF, 0xFF, 0xFF));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);

		glUseProgram(renderManager.programs["blur"]);
		glUniform2f(glGetUniformLocation(renderManager.programs["blur"], "pixelSize"), 2.0f / this->width(), 2.0f / this->height());
		//printf("pixelSize loc %d\n", glGetUniformLocation(vboRenderManager.programs["blur"], "pixelSize"));

		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex0"), 1);//COLOR
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[0]);

		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex1"), 2);//NORMAL
		glActiveTexture(GL_TEXTURE2);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[1]);

		/*glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex2"), 3);
		glActiveTexture(GL_TEXTURE3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[2]);*/

		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "depthTex"), 8);
		glActiveTexture(GL_TEXTURE8);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex3"), 4);//AO
		glActiveTexture(GL_TEXTURE4);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragAOTex);

		if (renderManager.renderingMode == RenderManager::RENDERING_MODE_SSAO) {
			glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "ssao_used"), 1); // ssao used
		}
		else {
			glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "ssao_used"), 0); // no ssao
		}

		glBindVertexArray(renderManager.secondPassVAO);

		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
		glDepthFunc(GL_LEQUAL);

	}

	// REMOVE
	glActiveTexture(GL_TEXTURE0);
}

void GLWidget3D::loadImage(const QString& filename) {
	image.load(filename);

	// scale the image such that it fits to the window
	float scale = std::min((float)width() / image.width(), (float)height() / image.height());
	QImage newImage = image.scaled(image.width() * scale, image.height() * scale);

	image = QImage(width(), height(), QImage::Format_RGB32);
	QPainter painter(&image);
	painter.fillRect(0, 0, image.width(), image.height(), QBrush(QColor(255, 255, 255)));
	painter.drawImage((width() - newImage.width()) / 2, (height() - newImage.height()) / 2, newImage);

	// clear the image offset
	image_offset = glm::dvec2(0, 0);

	update();
}

void GLWidget3D::clearLines() {
	lines.clear();
	update();
}

void GLWidget3D::loadLines(const QString& filename) {
	lines.clear();

	QFile file(filename);
	if (file.open(QIODevice::ReadOnly)) {
		QTextStream in(&file);

		while (true) {
			QString line = in.readLine();
			if (line.isNull()) break;
			QStringList list = line.split(" ");
			
			if (list.size() == 2) {
				origin = glm::dvec2(list[0].toFloat(), list[1].toFloat());
			}
			else {
				glm::dvec2 start(list[0].toFloat(), list[1].toFloat());
				glm::dvec2 end(list[2].toFloat(), list[3].toFloat());
				if (start != end) {
					lines.push_back(vp::VanishingLine(start.x, start.y, end.x, end.y, list[4].toInt()));
				}
			}
		}

		file.close();
	}
	update();
}

void GLWidget3D::saveLines(const QString& filename) {
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly)) {
		QTextStream out(&file);

		out << origin.x << " " << origin.y << "\n";
		for (auto line : lines) {
			out << line.start.x << " " << line.start.y << " " << line.end.x << " " << line.end.y << " " << line.type << "\n";
		}
	}
	file.close();
}

void GLWidget3D::undo() {
	if (lines.size() > 0) {
		lines.pop_back();
		update();
	}
}

void GLWidget3D::computeVanishingPoint() {
	std::vector<glm::dvec2> vps;
	vp::computeVanishingPoints(lines, vps);

	for (int i = 0; i < vps.size(); ++i) {
		std::cout << "vp" << (i + 1) << ": " << vps[i].x << ", " << vps[i].y << std::endl;
	}

	// determine the bounding box of the resulting image
	const int margin = 50;
	int x1 = 0;
	int y1 = 0;
	int x2 = width() - 1;
	int y2 = height() - 1;
	for (int i = 0; i < vps.size(); ++i) {
		if (vps[i].x - margin < x1) x1 = vps[i].x - margin;
		if (vps[i].x + margin > x2) x2 = vps[i].x + margin;
		if (vps[i].y - margin < y1) y1 = vps[i].y - margin;
		if (vps[i].y + margin> y2) y2 = vps[i].y + margin;
	}
	std::cout << "Rect: " << x1 << ", " << y1 << ", " << x2 << ", " << y2 << std::endl;

	// create a resulting image
	QImage result(x2 - x1 + 1, y2 - y1 + 1, QImage::Format_RGB32);
	QPainter painter(&result);
	painter.fillRect(0, 0, result.width(), result.height(), QBrush(QColor(255, 255, 255)));
	painter.drawImage(-x1, -y1, image);
	painter.setPen(QPen(QColor(0, 0, 255), 1, Qt::SolidLine, Qt::RoundCap));
	for (auto line : lines) {
		double dist0 = glm::length(line.start - line.end);
		double dist1 = glm::length(line.start - vps[line.type]);
		double dist2 = glm::length(line.end - vps[line.type]);
		if (dist1 < dist2) {
			line.start = line.start + (line.start - line.end) * (dist1 + margin) / dist0;
		}
		else {
			line.end = line.end + (line.end - line.start) * (dist2 + margin) / dist0;
		}
		painter.drawLine(line.start.x - x1, line.start.y - y1, line.end.x - x1, line.end.y - y1);
	}
	painter.setPen(QPen(QColor(255, 0, 0), 10, Qt::SolidLine, Qt::RoundCap));
	for (int i = 0; i < vps.size(); ++i) {
		painter.drawEllipse(vps[i].x - x1, vps[i].y - y1, 10, 10);
	}
	painter.end();
	result.save("vanishing_points.png");
}

void GLWidget3D::computeCameraMatrix() {
	// rotate the image and lines so that the building stands exactly upright
	//rotateAll();

	// center the image and lines
	//centerAll();

	std::vector<glm::dvec2> vps;
	vp::computeVanishingPoints(lines, vps);

	// convert the coordinates of vp to [-1, 1]
	for (int i = 0; i < vps.size(); ++i) {
		vps[i].x = vps[i].x / width() * 2.0 - 1;
		vps[i].y = 1 - vps[i].y / height() * 2.0;
	}
	
	// compute K and R matrices
	glm::dmat3 K, R;
	vp::extractCameraMatrix(vps, K, R);
	std::cout << "K: " << glm::to_string(K) << std::endl;
	std::cout << "R: " << glm::to_string(R) << std::endl;
	double f = K[0][0];
	camera.fovy = vp::rad2deg(atan2(1.0, f) * 2);
	double xrot, yrot, zrot;
	vp::decomposeRotation(R, xrot, yrot, zrot);
	camera.xrot = vp::rad2deg(xrot);
	camera.yrot = vp::rad2deg(yrot);
	camera.zrot = vp::rad2deg(zrot);

	// compute T matrix
	glm::dvec3 T;
	double camera_distance = camera.distanceBase / tan(vp::deg2rad(camera.fovy * 0.5));
	std::cout << "camera_distance: " << camera_distance << std::endl;
	vp::extractCameraMatrix(vps, f, glm::vec2(origin.x / width() * 2 - 1, 1 - origin.y / height() * 2), camera_distance, T);
	std::cout << "T: " << glm::to_string(T) << std::endl;
	camera.pos = glm::vec3(-T.x, -T.y, T.z);

	// update camera
	camera.updatePMatrix(width(), height());

	updateStatusBar();
	update();
}

void GLWidget3D::rotateAll() {

}

void GLWidget3D::centerAll() {
	// compute the bounding box of lines
	double x1 = std::numeric_limits<double>::max();
	double x2 = -std::numeric_limits<double>::max();
	double y1 = std::numeric_limits<double>::max(); 
	double y2 = -std::numeric_limits<double>::max();
	for (auto line : lines) {
		if (line.start.x < x1) x1 = line.start.x;
		if (line.end.x < x1) x1 = line.end.x;
		if (line.start.y < y1) y1 = line.start.y;
		if (line.end.y < y1) y1 = line.end.y;
		if (line.start.x > x2) x2 = line.start.x;
		if (line.end.x > x2) x2 = line.end.x;
		if (line.start.y > y2) y2 = line.start.y;
		if (line.end.y > y2) y2 = line.end.y;
	}
	if (origin.x < x1) x1 = origin.x;
	if (origin.y < y1) y1 = origin.y;
	if (origin.x > x2) x2 = origin.x;
	if (origin.y > y2) y2 = origin.y;

	// compute the offset of the image
	glm::dvec2 offset = glm::dvec2(width() * 0.5 - (x1 + x2) * 0.5, height() * 0.5 - (y1 + y2) * 0.5);
	image_offset += offset;
	std::cout << glm::to_string(image_offset) << std::endl;

	// shift the lines and origin
	for (int i = 0; i < lines.size(); ++i) {
		lines[i].start += offset;
		lines[i].end += offset;
	}
	origin += offset;
}

void GLWidget3D::updateGeometry() {
	std::vector<Vertex> vertices;
	//glutils::drawBox(cuboid_size[0], cuboid_size[1], cuboid_size[2], glm::vec4(1, 1, 1, 1), glm::translate(glm::rotate(glm::mat4(), -(float)vp::M_PI * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(cuboid_size[0] * 0.5, cuboid_size[1] * 0.5, cuboid_size[2] * 0.5)), vertices);
	glutils::drawBox(cuboid_size[0], cuboid_size[1], cuboid_size[2], glm::vec4(1, 1, 1, 1), glm::translate(glm::rotate(glm::mat4(), -(float)vp::M_PI * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), vertices);
	renderManager.removeObjects();
	renderManager.addObject("box", "", vertices, true);
}

void GLWidget3D::keyPressEvent(QKeyEvent *e) {
	ctrlPressed = false;

	if (e->modifiers() == Qt::ControlModifier) {
		ctrlPressed = true;
	}

	switch (e->key()) {
	case Qt::Key_Shift:
		shiftPressed = true;
		break;
	case Qt::Key_Alt:
		altPressed = true;
		break;
	case Qt::Key_1:
		cuboid_size[0] += 0.5 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_2:
		cuboid_size[1] += 0.5 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_3:
		cuboid_size[2] += 0.5 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_Left:
		image_offset.x -= (ctrlPressed ? 1 : 10);
		update();
		break;
	case Qt::Key_Right:
		image_offset.x += (ctrlPressed ? 1 : 10);
		update();
		break;
	case Qt::Key_Up:
		image_offset.y -= (ctrlPressed ? 1 : 10);
		update();
		break;
	case Qt::Key_Down:
		image_offset.y += (ctrlPressed ? 1 : 10);
		update();
		break;
	default:
		break;
	}
}

void GLWidget3D::keyReleaseEvent(QKeyEvent* e) {
	switch (e->key()) {
	case Qt::Key_Shift:
		shiftPressed = false;
		break;
	case Qt::Key_Control:
		ctrlPressed = false;
		break;
	case Qt::Key_Alt:
		altPressed = false;
		break;
	case Qt::Key_1:
	case Qt::Key_2:
	case Qt::Key_3:
		break;
	default:
		break;
	}
}

