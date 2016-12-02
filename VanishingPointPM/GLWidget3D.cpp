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
#include "GrammarParser.h"
#include "Rectangle.h"
#include "Utils.h"
#include "CVUtils.h"
#include <time.h>

GLWidget3D::GLWidget3D(MainWindow *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers)) {
	this->mainWin = parent;
	ctrlPressed = false;
	shiftPressed = false;
	altPressed = false;

	// Grammarを読み込む
	{
		grammars["mass"].resize(10);
		cga::parseGrammar("cga/mass/contour_01.xml", grammars["mass"][0]);
		cga::parseGrammar("cga/mass/contour_02.xml", grammars["mass"][1]);
		cga::parseGrammar("cga/mass/contour_03.xml", grammars["mass"][2]);
		cga::parseGrammar("cga/mass/contour_04.xml", grammars["mass"][3]);
		cga::parseGrammar("cga/mass/contour_05.xml", grammars["mass"][4]);
		cga::parseGrammar("cga/mass/contour_06.xml", grammars["mass"][5]);
		cga::parseGrammar("cga/mass/contour_07.xml", grammars["mass"][6]);
		cga::parseGrammar("cga/mass/contour_08.xml", grammars["mass"][7]);
		cga::parseGrammar("cga/mass/contour_09.xml", grammars["mass"][8]);
		cga::parseGrammar("cga/mass/contour_10.xml", grammars["mass"][9]);

		grammars["facade"].resize(2);
		cga::parseGrammar("cga/facade/facade_01.xml", grammars["facade"][0]);
		cga::parseGrammar("cga/facade/facade_02.xml", grammars["facade"][1]);

		grammars["window"].resize(9);
		cga::parseGrammar("cga/window/window_01.xml", grammars["window"][0]);
		cga::parseGrammar("cga/window/window_02.xml", grammars["window"][1]);
		cga::parseGrammar("cga/window/window_03.xml", grammars["window"][2]);
		cga::parseGrammar("cga/window/window_04.xml", grammars["window"][3]);
		cga::parseGrammar("cga/window/window_05.xml", grammars["window"][4]);
		cga::parseGrammar("cga/window/window_06.xml", grammars["window"][5]);
		cga::parseGrammar("cga/window/window_07.xml", grammars["window"][6]);
		cga::parseGrammar("cga/window/window_08.xml", grammars["window"][7]);
		cga::parseGrammar("cga/window/window_09.xml", grammars["window"][8]);
	}

	grammar_type = GRAMMAR_TYPE_MASS;
	grammar_ids["mass"] = 0;
	pm_params["mass"].resize(grammars["mass"][0].attrs.size(), 0.5);
	grammar_ids["facade"] = 0;
	pm_params["facade"].resize(grammars["facade"][0].attrs.size(), 0.5);
	grammar_ids["window"] = 0;
	pm_params["window"].resize(grammars["window"][0].attrs.size(), 0.5);

	pen_type = PEN_TYPE_VANISHING_LINE;
	lineWidth = 3;
	horizontalLeftColor = QColor(0, 0, 192);
	horizontalRightColor = QColor(64, 64, 255);
	verticalColor = QColor(140, 140, 255);
	silhouetteWidth = 3;
	silhouetteColor = QColor(255, 0, 0);

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

void GLWidget3D::clearBackground() {
	image_loaded = false;
	mainWin->setWindowTitle("Vanishing Point");

	update();
}

void GLWidget3D::loadImage(const QString& filename) {
	image.load(filename);
	image_loaded = true;

	// scale the image such that it fits to the window
	float scale = std::min((float)width() / image.width(), (float)height() / image.height());
	QImage newImage = image.scaled(image.width() * scale, image.height() * scale);

	image = QImage(width(), height(), QImage::Format_RGB32);
	QPainter painter(&image);
	painter.fillRect(0, 0, image.width(), image.height(), QBrush(QColor(255, 255, 255)));
	painter.drawImage((width() - newImage.width()) / 2, (height() - newImage.height()) / 2, newImage);

	mainWin->setWindowTitle(QString("Vanishing Point - ") + filename);

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
			QStringList list = line.split(QRegExp("(\t| )"));

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

void GLWidget3D::clearSilhouette() {
	silhouette.clear();
	update();
}

void GLWidget3D::loadSilhouette(const QString& filename) {
	silhouette.clear();

	QFile file(filename);
	if (file.open(QIODevice::ReadOnly)) {
		QTextStream in(&file);

		while (true) {
			QString line = in.readLine();
			if (line.isNull()) break;
			QStringList list = line.split(QRegExp("(\t| )"));

			glm::dvec2 start(list[0].toFloat(), list[1].toFloat());
			glm::dvec2 end(list[2].toFloat(), list[3].toFloat());
			if (start != end) {
				silhouette.push_back(vp::VanishingLine(start.x, start.y, end.x, end.y, list[4].toInt()));
			}
		}

		file.close();
	}

	update();
}

void GLWidget3D::saveSilhouette(const QString& filename) {
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly)) {
		QTextStream out(&file);

		for (auto line : silhouette) {
			out << line.start.x << " " << line.start.y << " " << line.end.x << " " << line.end.y << " " << line.type << "\n";
		}
	}
	file.close();
}

void GLWidget3D::undo() {
	if (pen_type == PEN_TYPE_VANISHING_LINE) {
		if (lines.size() > 0) {
			lines.pop_back();
			update();
		}
	}
	else if (pen_type == PEN_TYPE_SILHOUETTE) {
		if (silhouette.size() > 0) {
			silhouette.pop_back();
			update();
		}
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
	std::vector<glm::dvec2> vps;
	vp::computeVanishingPoints(lines, vps);

	// convert the coordinates of vp to [-1, 1]
	for (int i = 0; i < vps.size(); ++i) {
		vps[i].x = vps[i].x / width() * 2.0 - 1;
		vps[i].y = 1 - vps[i].y / height() * 2.0;
	}

	double f, xrot, yrot, zrot;
	vp::extractCameraParameters(vps, f, xrot, yrot, zrot, camera.center);
	camera.fovy = vp::rad2deg(atan2(1.0, f) * 2);
	camera.xrot = vp::rad2deg(xrot);
	camera.yrot = vp::rad2deg(yrot);
	camera.zrot = vp::rad2deg(zrot);

	// compute T matrix
	glm::dvec3 T;
	double camera_distance = camera.distanceBase / tan(vp::deg2rad(camera.fovy * 0.5));
	vp::extractCameraMatrixT(f, glm::vec2(origin.x / width() * 2 - 1, 1 - origin.y / height() * 2) - camera.center, camera_distance, T);
	camera.pos = glm::vec3(-T.x, -T.y, -T.z);

	// update camera
	camera.updatePMatrix(width(), height());

	updateStatusBar();
	update();
}

#if 0
void GLWidget3D::reconstruct3DAll() {
	QDir data_dir("data/");
	QStringList files = data_dir.entryList(QDir::NoDotAndDotDot | QDir::Files);// , QDir::DirsFirst);//(QDir::Filter::Files,QDir::SortFlag::NoSort)
	for (auto file : files) {
		if (file.endsWith(".jpg") || file.endsWith(".png")) {
			std::cout << "------------------------------------------------" << std::endl;
			std::cout << file.toUtf8().constData() << " ..." << std::endl;

			QStringList list = file.split(".");
			int id = list[0].toInt();

			// determine the grammar id
			if (id == 36 || id == 37 || id == 39) {
				// cylinder
				grammar_id = 1;
			}
			else if (id == 13 || id == 15 || id == 16 || id == 22 || id == 30 || id == 31) {
				// corner cut
				grammar_id = 2;
			}
			else if (id == 2 || id == 24) {
				// curved corner
				grammar_id = 3;
			}


			// load vanishing lines
			loadLines(QString("data/lines_%1.txt").arg(id));

			// load silhouette
			loadSilhouette(QString("data/%1.ctr").arg(list[0]));

			std::vector<float> params = reconstruct3D();
			for (int k = 0; k < params.size(); ++k) {
				if (k > 0) std::cout << ", ";
				std::cout << params[k];
			}
			std::cout << std::endl;
		}
	}
}
#endif

#if 0
std::vector<float> GLWidget3D::reconstruct3D() {
	time_t start = clock();

	/////////////////////////////////////////////////////////////////////////
	// compute camera parameters
	std::vector<glm::dvec2> vps;
	vp::computeVanishingPoints(lines, vps);

	// convert the coordinates of vp to [-1, 1]
	for (int i = 0; i < vps.size(); ++i) {
		vps[i].x = vps[i].x / width() * 2.0 - 1;
		vps[i].y = 1 - vps[i].y / height() * 2.0;
	}

	double f, xrot, yrot, zrot;
	vp::extractCameraParameters(vps, f, xrot, yrot, zrot, camera.center);
	camera.fovy = vp::rad2deg(atan2(1.0, f) * 2);
	camera.xrot = vp::rad2deg(xrot);
	camera.yrot = vp::rad2deg(yrot);
	camera.zrot = vp::rad2deg(zrot);

	// compute T matrix
	/*
	glm::dvec3 T;
	double camera_distance = camera.distanceBase / tan(vp::deg2rad(camera.fovy * 0.5));
	vp::extractCameraMatrix(vps, f, glm::vec2(origin.x / width() * 2 - 1, 1 - origin.y / height() * 2) - camera.center, camera_distance, T);
	camera.pos = glm::vec3(-T.x, -T.y, -T.z);
	*/

	// update camera
	camera.updatePMatrix(width(), height());

	// create image of silhouette
	cv::Mat silhouette_image(height(), width(), CV_8UC1, cv::Scalar(255));
	for (auto line : silhouette) {
		cv::line(silhouette_image, cv::Point(line.start.x, line.start.y), cv::Point(line.end.x, line.end.y), cv::Scalar(0), 1);
	}

	// create distance map of silhouette
	cv::Mat silhouette_dist_map;
	cv::threshold(silhouette_image, silhouette_image, 254, 255, CV_THRESH_BINARY);
	cv::distanceTransform(silhouette_image, silhouette_dist_map, CV_DIST_L2, 3);
	silhouette_dist_map.convertTo(silhouette_dist_map, CV_64F);

	/////////////////////////////////////////////////////////////////////////
	// do random walk to find good initial PM parameter values
	renderManager.renderingMode = RenderManager::RENDERING_MODE_CONTOUR;

	std::vector<float> best_params(grammars["mass"][grammar_id].attrs.size() + 2);
	double diff_min = std::numeric_limits<double>::max();
	printf("find good values by random sampling: ");
	for (int iter = 0; iter < 500; ++iter) {
		printf("\rfind good initial values by random sampling: %d", iter + 1);

		// randomly pick the initial values
		std::vector<float> cur_params(best_params.size());
		for (int k = 0; k < cur_params.size(); ++k) {
			//cur_params[k] = utils::genRand(0.0, 1.0);
			cur_params[k] = utils::genRand(0.3, 0.7);
		}

		// compute the camera pos
		glm::dvec3 T;
		double camera_distance = camera.distanceBase / tan(vp::deg2rad(camera.fovy * 0.5));
		vp::extractCameraMatrixT(f, glm::vec2(cur_params[0] * 2 - 1, cur_params[1] * 2 - 1) - camera.center, camera_distance, T);
		camera.pos = glm::vec3(-T.x, -T.y, -T.z);

		// update camera
		camera.updatePMatrix(width(), height());
		
		cv::Mat rendered_image;
		renderImage(grammars["mass"][grammar_id], std::vector<float>(cur_params.begin() + 2, cur_params.end()), rendered_image);

		// compute the difference
		double diff = distanceMap(rendered_image, silhouette_dist_map);

		// coordinate descent
		float delta = 0.1;
		for (int iter2 = 0; iter2 < 20; ++iter2) {
			for (int k = 0; k < cur_params.size(); ++k) {
				// option 1
				std::vector<float> next_params1 = cur_params;
				next_params1[k] -= delta;
				if (k < 2) {
					glm::dvec3 T;
					double camera_distance = camera.distanceBase / tan(vp::deg2rad(camera.fovy * 0.5));
					vp::extractCameraMatrixT(f, glm::vec2(next_params1[0] * 2 - 1, next_params1[1] * 2 - 1) - camera.center, camera_distance, T);
					camera.pos = glm::vec3(-T.x, -T.y, -T.z);

					// update camera
					camera.updatePMatrix(width(), height());
				}
				cv::Mat rendered_image1;
				renderImage(grammars["mass"][grammar_id], std::vector<float>(next_params1.begin() + 2, next_params1.end()), rendered_image1);
				double diff1 = distanceMap(rendered_image1, silhouette_dist_map);

				// option 2
				std::vector<float> next_params2 = cur_params;
				next_params2[k] += delta;
				if (k < 2) {
					glm::dvec3 T;
					double camera_distance = camera.distanceBase / tan(vp::deg2rad(camera.fovy * 0.5));
					vp::extractCameraMatrixT(f, glm::vec2(next_params2[0] * 2 - 1, next_params2[1] * 2 - 1) - camera.center, camera_distance, T);
					camera.pos = glm::vec3(-T.x, -T.y, -T.z);

					// update camera
					camera.updatePMatrix(width(), height());
				}
				cv::Mat rendered_image2;
				renderImage(grammars["mass"][grammar_id], std::vector<float>(next_params2.begin() + 2, next_params2.end()), rendered_image2);
				double diff2 = distanceMap(rendered_image2, silhouette_dist_map);

				if (diff1 < diff2 && diff1 < diff) {
					diff = diff1;
					cur_params = next_params1;
				}
				else if (diff2 < diff1 && diff2 < diff) {
					diff = diff2;
					cur_params = next_params2;
				}
			}

			delta *= 0.8;
		}

		if (diff < diff_min) {
			diff_min = diff;
			best_params = cur_params;
		}
	}
	printf("\n");


	renderManager.renderingMode = RenderManager::RENDERING_MODE_LINE;

	// set the camera pos
	glm::dvec3 T;
	double camera_distance = camera.distanceBase / tan(vp::deg2rad(camera.fovy * 0.5));
	vp::extractCameraMatrixT(f, glm::vec2(best_params[0] * 2 - 1, best_params[1] * 2 - 1) - camera.center, camera_distance, T);
	camera.pos = glm::vec3(-T.x, -T.y, -T.z);
	camera.updatePMatrix(width(), height());

	// set PM parameter values
	pm_params.clear();
	for (int k = 2; k < best_params.size(); ++k) {
		pm_params.push_back(best_params[k]);
	}

	updateGeometry(grammars["mass"][grammar_id], pm_params);

	updateStatusBar();
	update();

	time_t end = clock();
	std::cout << (double)(end - start) / CLOCKS_PER_SEC << "sec." << std::endl;

	// return all the parameters
	std::vector<float> ret;
	ret.push_back(camera.xrot);
	ret.push_back(camera.yrot);
	ret.push_back(camera.zrot);
	ret.push_back(camera.fovy);
	ret.push_back(camera.pos.x);
	ret.push_back(camera.pos.y);
	ret.push_back(camera.pos.z);
	std::vector<std::string> keys;
	for (auto it = grammars["mass"][grammar_id].attrs.begin(); it != grammars["mass"][grammar_id].attrs.end(); ++it) {
		keys.push_back(it->first);
	}
	for (int k = 0; k < pm_params.size(); ++k) {
		ret.push_back(pm_params[k] * (grammars["mass"][grammar_id].attrs[keys[k]].range_end - grammars["mass"][grammar_id].attrs[keys[k]].range_start) + grammars["mass"][grammar_id].attrs[keys[k]].range_start);
	}

	return ret;
}
#endif

/**
 * Generate an image using the selected grammar and the PM parameter values.
 *
 * @param grammar			grammar
 * @param pm_params			PM parameter values
 * @param rendered_image	rendered image [OUT]
 */
/*
void GLWidget3D::renderImage(cga::Grammar& grammar, const std::vector<float>& pm_params, cv::Mat& rendered_image) {
	updateGeometry(grammar, pm_params);

	render();
	QImage img = grabFrameBuffer();
	rendered_image = cv::Mat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine()).clone();
	cv::cvtColor(rendered_image, rendered_image, cv::COLOR_BGRA2BGR);
}
*/

/**
 * Compute the distance between the rendered image and the target.
 *
 * @param rendered_image		rendered image
 * @param reference_dist_map	distance map of the target
 * @return						distance
 */
double GLWidget3D::distanceMap(cv::Mat rendered_image, const cv::Mat& reference_dist_map) {
	cv::cvtColor(rendered_image, rendered_image, CV_BGR2GRAY);

	// compute the distance map
	cv::Mat rendered_dist_map;
	cv::threshold(rendered_image, rendered_image, 254, 255, CV_THRESH_BINARY);
	cv::distanceTransform(rendered_image, rendered_dist_map, CV_DIST_L2, 3);
	rendered_dist_map.convertTo(rendered_dist_map, CV_64F);

	// compute the squared difference
	cv::Mat diff_mat;
	cv::reduce((reference_dist_map - rendered_dist_map).mul(reference_dist_map - rendered_dist_map), diff_mat, 0, CV_REDUCE_SUM);
	cv::reduce(diff_mat, diff_mat, 1, CV_REDUCE_SUM);

	return diff_mat.at<double>(0, 0);
}

/**
 * Update the geometry on GPU using the selected grammar and the PM parameter values.
 *
 * @param grammar		grammar
 * @param pm_params		PM parameter values
 */
void GLWidget3D::updateGeometry(cga::Grammar& grammar, const std::vector<float>& pm_params) {	
	// set param values
	cga::setParamValues(grammar, pm_params);

	// setup CGA
	cga::CGA cga;
	cga.modelMat = glm::rotate(glm::mat4(), -(float)vp::M_PI * 0.5f, glm::vec3(1, 0, 0));

	// set axiom
	boost::shared_ptr<cga::Shape> start = boost::shared_ptr<cga::Shape>(new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -(float)vp::M_PI * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1)));
	cga.stack.push_back(start);

	// generate 3d model
	//std::vector<boost::shared_ptr<glutils::Face>> faces;
	faces.clear();
	cga.derive(grammar, true);
	cga.generateGeometry(faces, true);
	renderManager.removeObjects();
	renderManager.addFaces(faces, true);

	// update shadow map
	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);
}

void GLWidget3D::updateGeometry() {
	// set param values
	cga::setParamValues(grammars["mass"][grammar_ids["mass"]], pm_params["mass"]);
	if (grammar_type == GRAMMAR_TYPE_FACADE) {
		cga::setParamValues(grammars["facade"][grammar_ids["facade"]], pm_params["facade"]);
		cga::setParamValues(grammars["window"][grammar_ids["window"]], pm_params["window"]);
	}

	// setup CGA
	cga::CGA cga;
	cga.modelMat = glm::rotate(glm::mat4(), -(float)vp::M_PI * 0.5f, glm::vec3(1, 0, 0));

	// set axiom
	boost::shared_ptr<cga::Shape> start = boost::shared_ptr<cga::Shape>(new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -(float)vp::M_PI * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1)));
	cga.stack.push_back(start);

	std::vector<cga::Grammar*> grammar_list;
	grammar_list.push_back(&grammars["mass"][grammar_ids["mass"]]);
	if (grammar_type == GRAMMAR_TYPE_FACADE) {
		grammar_list.push_back(&grammars["facade"][grammar_ids["facade"]]);
		grammar_list.push_back(&grammars["window"][grammar_ids["window"]]);
	}

	// generate 3d model
	faces.clear();
	cga.derive(grammar_list, true);
	cga.generateGeometry(faces, true);
	renderManager.removeObjects();
	renderManager.addFaces(faces, true);

	// update shadow map
	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);
}

void GLWidget3D::textureMapping() {
	// create texture folder
	if (!QDir("textures").exists()) QDir().mkdir("textures");

	// convert image to cv::Mat object
	cv::Mat imageMat = cv::Mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine()).clone();
	cv::cvtColor(imageMat, imageMat, cv::COLOR_BGRA2BGR);

	// rectify the facade image
	for (int fi = 0; fi < faces.size(); ++fi) {
		// non-quadratic face is not supported.
		if (faces[fi]->vertices.size() % 6 != 0) continue;

		std::vector<cv::Mat> rectified_images;
		std::vector<bool> visibilities;
		bool visible = false;

		// we create a texture for each quad because the face might be curved
		for (int qi = 0; qi < faces[fi]->vertices.size() / 6; ++qi) {
			// obtain the 2d/3d coordinates of the quad
			std::vector<cv::Point2f> pts;
			std::vector<glm::vec3> pts3d;
			for (int k = 0; k < 6; ++k) {
				if (k == 3 || k == 4) continue;

				int vi = qi * 6 + k;

				pts3d.push_back(faces[fi]->vertices[vi].position);

				glm::vec2 pp = utils::projectPoint(width(), height(), faces[fi]->vertices[vi].position, camera.mvpMatrix);
				pts.push_back(cv::Point2f(pp.x, pp.y));
			}

			// check if the quad is visible
			glm::vec3 normal = glm::cross(pts3d[1] - pts3d[0], pts3d[2] - pts3d[1]);
			normal = glm::vec3(camera.mvMatrix * glm::vec4(normal, 0));
			glm::vec3 view_dir = glm::vec3(camera.mvMatrix * glm::vec4(pts3d[0], 1));
			if (glm::dot(normal, view_dir) < 0) {
				visible = true;
				visibilities.push_back(true);

				// rectify the facade image
				cv::Mat transformMat;
				cv::Mat rectifiedImage = cvutils::rectify_image(imageMat, pts, transformMat, glm::length(pts3d[1] - pts3d[0]) / glm::length(pts3d[2] - pts3d[1]));
				rectifiedImage = cvutils::adjust_contrast(rectifiedImage);

				rectified_images.push_back(rectifiedImage);
			}
			else {
				visibilities.push_back(false);
				rectified_images.push_back(cv::Mat(glm::length(pts3d[2] - pts3d[1]) * 100, glm::length(pts3d[1] - pts3d[0]) * 100, CV_8UC3));
			}
		}

		// save the texture image
		if (visible) {
			// obtain the largest height of the images
			int height_max = 0;
			for (int i = 0; i < rectified_images.size(); ++i) {
				if (visibilities[i]) {
					if (rectified_images[i].rows > height_max) {
						height_max = rectified_images[i].rows;
					}
				}
			}

			// determine the size of the merged texture
			int width_total = 0;
			for (int i = 0; i < rectified_images.size(); ++i) {
				width_total += (float)rectified_images[i].cols * height_max / rectified_images[i].rows;
			}

			// initialize the texture
			cv::Mat textureImg(height_max, width_total, CV_8UC3, cv::Scalar(255, 255, 255));

			// merge the texture
			int offset = 0;
			for (int i = 0; i < rectified_images.size(); ++i) {
				int width = (float)rectified_images[i].cols * height_max / rectified_images[i].rows;

				if (visibilities[i]) {
					cv::Mat roi(textureImg, cv::Rect(offset, 0, width, height_max));
					
					cv::resize(rectified_images[i], rectified_images[i], cv::Size(width, height_max));
					rectified_images[i].copyTo(roi);
				}

				offset += width;
			}

			time_t now = clock();
			QString name = QString("textures/%1_%2_%3.png").arg(faces[fi]->name.c_str()).arg(fi).arg(now);
			cv::imwrite(name.toUtf8().constData(), textureImg);

			faces[fi]->texture = name.toUtf8().constData();
		}
	}

	renderManager.removeObjects();
	renderManager.addFaces(faces, true);
	renderManager.renderingMode = RenderManager::RENDERING_MODE_BASIC;
	update();
}

void GLWidget3D::updateStatusBar() {
	char buff[256];

	QString format("rot=(%.3f, %.3f, %.3f), fov=%.3f, O=(%.6f, %.6f), pos=(%.3f, %.3f, %.3f), PM=(");
	sprintf(buff, format.toUtf8().constData(), camera.xrot, camera.yrot, camera.zrot, camera.fovy, camera.center.x, camera.center.y, camera.pos.x, camera.pos.y, camera.pos.z);

	QString msg(buff);

	int k = 0;
	if (grammar_type == GRAMMAR_TYPE_MASS) {
		for (auto it = grammars["mass"][grammar_ids["mass"]].attrs.begin(); it != grammars["mass"][grammar_ids["mass"]].attrs.end(); ++it, ++k) {
			if (k > 0) msg += ", ";
			sprintf(buff, "%.3f", pm_params["mass"][k] * (it->second.range_end - it->second.range_start) + it->second.range_start);
			msg += buff;
		}
	}
	else if (grammar_type == GRAMMAR_TYPE_FACADE) {
		for (auto it = grammars["facade"][grammar_ids["facade"]].attrs.begin(); it != grammars["facade"][grammar_ids["facade"]].attrs.end(); ++it, ++k) {
			if (k > 0) msg += ", ";
			sprintf(buff, "%.3f", pm_params["facade"][k] * (it->second.range_end - it->second.range_start) + it->second.range_start);
			msg += buff;
		}
	}
	msg += ")";
	
	mainWin->statusBar()->showMessage(msg);
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
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 0) {
			pm_params["mass"][0] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 0) {
			pm_params["facade"][0] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_2:
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 1) {
			pm_params["mass"][1] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 1) {
			pm_params["facade"][1] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_3:
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 2) {
			pm_params["mass"][2] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 2) {
			pm_params["facade"][2] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_4:
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 3) {
			pm_params["mass"][3] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 3) {
			pm_params["facade"][3] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_5:
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 4) {
			pm_params["mass"][4] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 4) {
			pm_params["facade"][4] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_6:
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 5) {
			pm_params["mass"][5] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 5) {
			pm_params["facade"][5] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_7:
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 6) {
			pm_params["mass"][6] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 6) {
			pm_params["facade"][6] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_8:
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 7) {
			pm_params["mass"][7] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 7) {
			pm_params["facade"][7] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_9:
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 8) {
			pm_params["mass"][8] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 8) {
			pm_params["facade"][8] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_0:
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 9) {
			pm_params["mass"][9] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 9) {
			pm_params["facade"][9] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_Left:
		camera.pos.x += 0.5 * (ctrlPressed ? 0.1 : 1);
		camera.updateMVPMatrix();
		updateStatusBar();
		update();
		break;
	case Qt::Key_Right:
		camera.pos.x -= 0.5 * (ctrlPressed ? 0.1 : 1);
		camera.updateMVPMatrix();
		updateStatusBar();
		update();
		break;
	case Qt::Key_Up:
		camera.pos.y -= 0.5 * (ctrlPressed ? 0.1 : 1);
		camera.updateMVPMatrix();
		updateStatusBar();
		update();
		break;
	case Qt::Key_Down:
		camera.pos.y += 0.5 * (ctrlPressed ? 0.1 : 1);
		camera.updateMVPMatrix();
		updateStatusBar();
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
	default:
		break;
	}
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


	updateGeometry();// grammars["mass"][grammar_id], pm_params);
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

	QPainter painter(this);

	// draw background image
	if (image_loaded) {
		painter.setOpacity(0.5f);
		painter.drawImage(0, 0, image);
	}

	// draw lines
	painter.setOpacity(1.0f);
	for (auto line : lines) {
		if (line.type == vp::VanishingLine::TYPE_HORIZONTAL_LEFT) {
			painter.setPen(QPen(horizontalLeftColor, lineWidth));
		}
		else if (line.type == vp::VanishingLine::TYPE_HORIZONTAL_RIGHT) {
			painter.setPen(QPen(horizontalRightColor, lineWidth));
		}
		else {
			painter.setPen(QPen(verticalColor, lineWidth));
		}
		painter.drawLine(line.start.x, line.start.y, line.end.x, line.end.y);
	}

	// draw silhouette
	painter.setPen(QPen(silhouetteColor, silhouetteWidth));
	for (auto line : silhouette) {
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
				if (pen_type == PEN_TYPE_VANISHING_LINE) {
					lines.push_back(vp::VanishingLine(e->x(), e->y(), e->x(), e->y(), vp::VanishingLine::TYPE_HORIZONTAL_RIGHT));
				}
				else if (pen_type == PEN_TYPE_SILHOUETTE) {
					silhouette.push_back(vp::VanishingLine(e->x(), e->y(), e->x(), e->y(), vp::VanishingLine::TYPE_HORIZONTAL_RIGHT));
				}
			}
			else if (altPressed) {
				if (pen_type == PEN_TYPE_VANISHING_LINE) {
					lines.push_back(vp::VanishingLine(e->x(), e->y(), e->x(), e->y(), vp::VanishingLine::TYPE_VERTICAL));
				}
				else if (pen_type == PEN_TYPE_SILHOUETTE) {
					silhouette.push_back(vp::VanishingLine(e->x(), e->y(), e->x(), e->y(), vp::VanishingLine::TYPE_VERTICAL));
				}
			}
			else {
				if (pen_type == PEN_TYPE_VANISHING_LINE) {
					lines.push_back(vp::VanishingLine(e->x(), e->y(), e->x(), e->y(), vp::VanishingLine::TYPE_HORIZONTAL_LEFT));
				}
				else if (pen_type == PEN_TYPE_SILHOUETTE) {
					silhouette.push_back(vp::VanishingLine(e->x(), e->y(), e->x(), e->y(), vp::VanishingLine::TYPE_HORIZONTAL_LEFT));
				}
			}
		}
		update();
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
	else if (e->buttons() & Qt::MidButton) { // rotate around Z axis
		camera.rotateAroundZ(e->x(), e->y(), (ctrlPressed ? 0.1 : 1));
	}
	else if (e->buttons() & Qt::LeftButton) { // draw lines
		if (pen_type == PEN_TYPE_VANISHING_LINE) {
			if (lines.size() > 0) {
				lines.back().end = glm::vec2(e->x(), e->y());
			}
		}
		else if (pen_type == PEN_TYPE_SILHOUETTE) {
			if (silhouette.size() > 0) {
				silhouette.back().end = glm::vec2(e->x(), e->y());
			}
		}
	}
	updateStatusBar();
	update();
}

/**
 * This event handler is called when the mouse release events occur.
 */
void GLWidget3D::mouseReleaseEvent(QMouseEvent *e) {
	if (e->button() == Qt::LeftButton) {
		// remove the line if it is just a point
		if (pen_type == PEN_TYPE_VANISHING_LINE) {
			if (lines.size() > 0 && lines.back().start == lines.back().end) {
				lines.pop_back();
			}
		}
		else if (pen_type == PEN_TYPE_SILHOUETTE) {
			if (silhouette.size() > 0 && silhouette.back().start == silhouette.back().end) {
				silhouette.pop_back();
			}
		}
	}
}

void GLWidget3D::wheelEvent(QWheelEvent* e) {
	camera.changeFov(e->delta() * 0.05, (ctrlPressed ? 0.1 : 1), width(), height());
	updateStatusBar();
	update();
}
