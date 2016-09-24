#include "MainWindow.h"
#include <QFileDialog>
#include "GLWidget3D.h"
#include <iostream>
#include "OptionDialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.actionOpenImage, SIGNAL(triggered()), this, SLOT(onOpenImage()));
	connect(ui.actionClearLines, SIGNAL(triggered()), this, SLOT(onClearLines()));
	connect(ui.actionLoadLines, SIGNAL(triggered()), this, SLOT(onLoadLines()));
	connect(ui.actionSaveLines, SIGNAL(triggered()), this, SLOT(onSaveLines()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionUndo, SIGNAL(triggered()), this, SLOT(onUndo()));
	connect(ui.actionComputeVanishingPoint, SIGNAL(triggered()), this, SLOT(onComputeVanishingPoint()));
	connect(ui.actionComputeCameraMatrix, SIGNAL(triggered()), this, SLOT(onComputeCameraMatrix()));
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

void MainWindow::onOption() {
	OptionDialog dlg;
	dlg.setContourLineWidth(glWidget->contourLineWidth);
	dlg.setHorizontalLeftColor(glWidget->horizontalLeftColor);
	dlg.setHorizontalRightColor(glWidget->horizontalRightColor);
	dlg.setVerticalColor(glWidget->verticalColor);

	if (dlg.exec()) {
		glWidget->contourLineWidth = dlg.getContourLineWidth();
		glWidget->horizontalLeftColor = dlg.getHorizontalLeftColor();
		glWidget->horizontalRightColor = dlg.getHorizontalRightColor();
		glWidget->verticalColor = dlg.getVerticalColor();
	}
}