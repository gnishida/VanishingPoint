#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

class GLWidget3D;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);

protected:
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

public slots:
	void onClearBackground();
	void onOpenImage();
	void onClearLines();
	void onLoadLines();
	void onSaveLines();
	void onClearSilhouette();
	void onLoadSilhouette();
	void onSaveSilhouette();
	void onUndo();
	void onComputeVanishingPoint();
	void onComputeCameraMatrix();
	void on3DReconstruction();
	void on3DReconstructionAll();
	void onPenChanged();
	void onOption();

private:
	Ui::MainWindowClass ui;
	GLWidget3D* glWidget;
};

#endif // MAINWINDOW_H
