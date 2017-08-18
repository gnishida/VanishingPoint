#include "MainWindow.h"
#include <QFileDialog>
#include "GLWidget3D.h"
#include <iostream>
#include "OptionDialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	QActionGroup* groupRendering = new QActionGroup(this);
	groupRendering->addAction(ui.actionRenderingLine);
	groupRendering->addAction(ui.actionRenderingSilhouette);
	groupRendering->addAction(ui.actionRenderingBasic);
	groupRendering->addAction(ui.actionRenderingSSAO);
	ui.actionRenderingLine->setChecked(true);

	QActionGroup* groupPen = new QActionGroup(this);
	groupPen->addAction(ui.actionPenVanishingLine);
	groupPen->addAction(ui.actionPenSilhouette);
	ui.actionPenVanishingLine->setChecked(true);

	QActionGroup* groupGramar = new QActionGroup(this);
	groupGramar->addAction(ui.actionGrammarMass);
	groupGramar->addAction(ui.actionGrammarFacade);
	ui.actionGrammarMass->setChecked(true);

	connect(ui.actionClearBackground, SIGNAL(triggered()), this, SLOT(onClearBackground()));
	connect(ui.actionOpenImage, SIGNAL(triggered()), this, SLOT(onOpenImage()));
	connect(ui.actionClearLines, SIGNAL(triggered()), this, SLOT(onClearLines()));
	connect(ui.actionLoadLines, SIGNAL(triggered()), this, SLOT(onLoadLines()));
	connect(ui.actionSaveLines, SIGNAL(triggered()), this, SLOT(onSaveLines()));
	connect(ui.actionClearSilhouette, SIGNAL(triggered()), this, SLOT(onClearSilhouette()));
	connect(ui.actionLoadSilhouette, SIGNAL(triggered()), this, SLOT(onLoadSilhouette()));
	connect(ui.actionSaveSilhouette, SIGNAL(triggered()), this, SLOT(onSaveSilhouette()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionUndo, SIGNAL(triggered()), this, SLOT(onUndo()));
	connect(ui.actionComputeCameraMatrix, SIGNAL(triggered()), this, SLOT(onComputeCameraMatrix()));
	connect(ui.action3DReconstruction, SIGNAL(triggered()), this, SLOT(on3DReconstruction()));
	connect(ui.action3DReconstructionAll, SIGNAL(triggered()), this, SLOT(on3DReconstructionAll()));
	connect(ui.actionRenderingLine, SIGNAL(triggered()), this, SLOT(onRenderingChanged()));
	connect(ui.actionRenderingSilhouette, SIGNAL(triggered()), this, SLOT(onRenderingChanged()));
	connect(ui.actionRenderingBasic, SIGNAL(triggered()), this, SLOT(onRenderingChanged()));
	connect(ui.actionRenderingSSAO, SIGNAL(triggered()), this, SLOT(onRenderingChanged()));
	connect(ui.actionTextureMapping, SIGNAL(triggered()), this, SLOT(onTextureMapping()));
	connect(ui.actionPenVanishingLine, SIGNAL(triggered()), this, SLOT(onPenChanged()));
	connect(ui.actionPenSilhouette, SIGNAL(triggered()), this, SLOT(onPenChanged()));
	connect(ui.actionGrammarMass, SIGNAL(triggered()), this, SLOT(onGrammarChanged()));
	connect(ui.actionGrammarFacade, SIGNAL(triggered()), this, SLOT(onGrammarChanged()));
	connect(ui.actionOption, SIGNAL(triggered()), this, SLOT(onOption()));

	glWidget = new GLWidget3D(this);
	this->setCentralWidget(glWidget);
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
	glWidget->keyPressEvent(e);
}

void MainWindow::keyReleaseEvent(QKeyEvent* e) {
	glWidget->keyReleaseEvent(e);
}

void MainWindow::onClearBackground() {
	glWidget->clearBackground();
}

void MainWindow::onOpenImage() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load image file..."), "", tr("Image Files (*.png *.jpg)"));
	if (filename.isEmpty()) return;

	glWidget->loadImage(filename);

	this->setWindowTitle("Vanishing Point - " + filename);
}

void MainWindow::onClearLines() {
	glWidget->clearLines();
}

void MainWindow::onLoadLines() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open lines..."), "", tr("Line Files (*.txt)"));
	if (filename.isEmpty()) return;

	glWidget->loadLines(filename);
}

void MainWindow::onSaveLines() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save lines..."), "", tr("Line Files (*.txt)"));
	if (filename.isEmpty()) return;

	glWidget->saveLines(filename);
}

void MainWindow::onClearSilhouette() {
	glWidget->clearSilhouette();
}

void MainWindow::onLoadSilhouette() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open silhouette..."), "", tr("Silhoutte Files (*.ctr)"));
	if (filename.isEmpty()) return;

	glWidget->loadSilhouette(filename);
}

void MainWindow::onSaveSilhouette() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save silhouette..."), "", tr("Silhoutte Files (*.ctr)"));
	if (filename.isEmpty()) return;

	glWidget->saveSilhouette(filename);
}

void MainWindow::onUndo() {
	glWidget->undo();
}

void MainWindow::onComputeCameraMatrix() {
	glWidget->computeCameraMatrix();
	glWidget->update();
}

void MainWindow::on3DReconstruction() {
	/*std::vector<float> params = glWidget->reconstruct3D();
	for (int k = 0; k < params.size(); ++k) {
		if (k > 0) std::cout << ", ";
		std::cout << params[k];
	}
	std::cout << std::endl;
	*/
	glWidget->update();
}

void MainWindow::on3DReconstructionAll() {
	//glWidget->reconstruct3DAll();
	glWidget->update();
}

void MainWindow::onRenderingChanged() {
	if (ui.actionRenderingLine->isChecked()) {
		glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_LINE;
	}
	else if (ui.actionRenderingSilhouette->isChecked()) {
		glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_CONTOUR;
	}
	else if (ui.actionRenderingBasic->isChecked()) {
		glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_BASIC;
	}
	else if (ui.actionRenderingSSAO->isChecked()) {
		glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_SSAO;
	}
	glWidget->updateGeometry();
	glWidget->update();
}

void MainWindow::onTextureMapping() {
	glWidget->textureMapping();
}

void MainWindow::onPenChanged() {
	if (ui.actionPenVanishingLine->isChecked()) {
		glWidget->pen_type = GLWidget3D::PEN_TYPE_VANISHING_LINE;
	}
	else if (ui.actionPenSilhouette->isChecked())  {
		glWidget->pen_type = GLWidget3D::PEN_TYPE_SILHOUETTE;
	}
}

void MainWindow::onGrammarChanged() {
	if (ui.actionGrammarMass->isChecked()) {
		glWidget->grammar_type = GLWidget3D::GRAMMAR_TYPE_MASS;
	}
	else if (ui.actionGrammarFacade->isChecked()) {
		glWidget->grammar_type = GLWidget3D::GRAMMAR_TYPE_FACADE;
	}
	glWidget->updateGeometry();
	glWidget->update();
}

void MainWindow::onOption() {
	OptionDialog dlg;
	dlg.setVanishingLineWidth(glWidget->lineWidth);
	dlg.setHorizontalLeftColor(glWidget->horizontalLeftColor);
	dlg.setHorizontalRightColor(glWidget->horizontalRightColor);
	dlg.setVerticalColor(glWidget->verticalColor);
	dlg.setSilhouetteWidth(glWidget->silhouetteWidth);
	dlg.setSilhouetteColor(glWidget->silhouetteColor);
	dlg.setMassGrammarId(glWidget->grammar_ids["mass"]);
	dlg.setFacadeGrammarId(glWidget->grammar_ids["facade"]);
	dlg.setWindowGrammarId(glWidget->grammar_ids["window"]);

	if (dlg.exec()) {
		glWidget->lineWidth = dlg.getVanishingLineWidth();
		glWidget->horizontalLeftColor = dlg.getHorizontalLeftColor();
		glWidget->horizontalRightColor = dlg.getHorizontalRightColor();
		glWidget->verticalColor = dlg.getVerticalColor();
		glWidget->silhouetteWidth = dlg.getSilhouetteWidth();
		glWidget->silhouetteColor = dlg.getSilhouetteColor();
		glWidget->grammar_ids["mass"] = dlg.getMassGrammarId();
		glWidget->grammar_ids["facade"] = dlg.getFacadeGrammarId();
		glWidget->grammar_ids["window"] = dlg.getWindowGrammarId();
		glWidget->pm_params["mass"].resize(glWidget->grammars["mass"][glWidget->grammar_ids["mass"]].attrs.size());
		glWidget->pm_params["facade"].resize(glWidget->grammars["facade"][glWidget->grammar_ids["facade"]].attrs.size());
		glWidget->pm_params["window"].resize(glWidget->grammars["window"][glWidget->grammar_ids["window"]].attrs.size());
		glWidget->updateGeometry();
	}
}