#include "MainWindow.h"
#include <QFileDialog>
#include "GLWidget3D.h"
#include <iostream>
#include "OptionDialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	QActionGroup* group = new QActionGroup(this);
	group->addAction(ui.actionPenVanishingLine);
	group->addAction(ui.actionPenSilhouette);

	ui.actionPenVanishingLine->setChecked(true);

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
	connect(ui.actionComputeVanishingPoint, SIGNAL(triggered()), this, SLOT(onComputeVanishingPoint()));
	connect(ui.actionComputeCameraMatrix, SIGNAL(triggered()), this, SLOT(onComputeCameraMatrix()));
	connect(ui.action3DReconstruction, SIGNAL(triggered()), this, SLOT(on3DReconstruction()));
	connect(ui.action3DReconstructionAll, SIGNAL(triggered()), this, SLOT(on3DReconstructionAll()));
	connect(ui.actionPenVanishingLine, SIGNAL(triggered()), this, SLOT(onPenChanged()));
	connect(ui.actionPenSilhouette, SIGNAL(triggered()), this, SLOT(onPenChanged()));
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

void MainWindow::onComputeVanishingPoint() {
	glWidget->computeVanishingPoint();
}

void MainWindow::onComputeCameraMatrix() {
	glWidget->computeCameraMatrix();
	glWidget->update();
}

void MainWindow::on3DReconstruction() {
	std::vector<float> params = glWidget->reconstruct3D();
	for (int k = 0; k < params.size(); ++k) {
		if (k > 0) std::cout << ", ";
		std::cout << params[k];
	}
	std::cout << std::endl;
	glWidget->update();
}

void MainWindow::on3DReconstructionAll() {
	glWidget->reconstruct3DAll();
	glWidget->update();
}

void MainWindow::onPenChanged() {
	if (ui.actionPenVanishingLine->isChecked()) {
		glWidget->pen_type = GLWidget3D::PEN_TYPE_VANISHING_LINE;
	}
	else if (ui.actionPenSilhouette->isChecked())  {
		glWidget->pen_type = GLWidget3D::PEN_TYPE_SILHOUETTE;
	}
}

void MainWindow::onOption() {
	OptionDialog dlg;
	dlg.setContourLineWidth(glWidget->lineWidth);
	dlg.setHorizontalLeftColor(glWidget->horizontalLeftColor);
	dlg.setHorizontalRightColor(glWidget->horizontalRightColor);
	dlg.setVerticalColor(glWidget->verticalColor);
	dlg.setSilhouetteWidth(glWidget->silhouetteWidth);
	dlg.setSilhouetteColor(glWidget->silhouetteColor);
	dlg.setGrammarId(glWidget->grammar_id);

	if (dlg.exec()) {
		glWidget->lineWidth = dlg.getContourLineWidth();
		glWidget->horizontalLeftColor = dlg.getHorizontalLeftColor();
		glWidget->horizontalRightColor = dlg.getHorizontalRightColor();
		glWidget->verticalColor = dlg.getVerticalColor();
		glWidget->silhouetteWidth = dlg.getSilhouetteWidth();
		glWidget->silhouetteColor = dlg.getSilhouetteColor();
		if (glWidget->grammar_id != dlg.getGrammarId()) {
			glWidget->grammar_id = dlg.getGrammarId();
			glWidget->updateGeometry(glWidget->grammars[glWidget->grammar_id], glWidget->pm_params);
		}
	}
}