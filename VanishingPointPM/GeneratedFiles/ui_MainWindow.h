/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QAction *actionExit;
    QAction *actionOpenImage;
    QAction *actionUndo;
    QAction *actionSaveLines;
    QAction *actionComputeVanishingPoint;
    QAction *actionLoadLines;
    QAction *actionComputeCameraMatrix;
    QAction *actionClearLines;
    QAction *actionCentering;
    QAction *actionOption;
    QAction *action3DReconstruction;
    QAction *actionLoadSilhouette;
    QAction *actionSaveSilhouette;
    QAction *actionPenVanishingLine;
    QAction *actionPenSilhouette;
    QAction *action3DReconstructionAll;
    QAction *actionClearSilhouette;
    QAction *actionClearBackground;
    QAction *actionTextureMapping;
    QAction *actionRenderingLine;
    QAction *actionRenderingSilhouette;
    QAction *actionRenderingBasic;
    QAction *actionGrammarMass;
    QAction *actionGrammarFacade;
    QAction *actionRenderingSSAO;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuTool;
    QMenu *menuPen;
    QMenu *menuRendering;
    QMenu *menuGrammar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName(QStringLiteral("MainWindowClass"));
        MainWindowClass->resize(800, 853);
        actionExit = new QAction(MainWindowClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionOpenImage = new QAction(MainWindowClass);
        actionOpenImage->setObjectName(QStringLiteral("actionOpenImage"));
        actionUndo = new QAction(MainWindowClass);
        actionUndo->setObjectName(QStringLiteral("actionUndo"));
        actionSaveLines = new QAction(MainWindowClass);
        actionSaveLines->setObjectName(QStringLiteral("actionSaveLines"));
        actionComputeVanishingPoint = new QAction(MainWindowClass);
        actionComputeVanishingPoint->setObjectName(QStringLiteral("actionComputeVanishingPoint"));
        actionLoadLines = new QAction(MainWindowClass);
        actionLoadLines->setObjectName(QStringLiteral("actionLoadLines"));
        actionComputeCameraMatrix = new QAction(MainWindowClass);
        actionComputeCameraMatrix->setObjectName(QStringLiteral("actionComputeCameraMatrix"));
        actionClearLines = new QAction(MainWindowClass);
        actionClearLines->setObjectName(QStringLiteral("actionClearLines"));
        actionCentering = new QAction(MainWindowClass);
        actionCentering->setObjectName(QStringLiteral("actionCentering"));
        actionOption = new QAction(MainWindowClass);
        actionOption->setObjectName(QStringLiteral("actionOption"));
        action3DReconstruction = new QAction(MainWindowClass);
        action3DReconstruction->setObjectName(QStringLiteral("action3DReconstruction"));
        actionLoadSilhouette = new QAction(MainWindowClass);
        actionLoadSilhouette->setObjectName(QStringLiteral("actionLoadSilhouette"));
        actionSaveSilhouette = new QAction(MainWindowClass);
        actionSaveSilhouette->setObjectName(QStringLiteral("actionSaveSilhouette"));
        actionPenVanishingLine = new QAction(MainWindowClass);
        actionPenVanishingLine->setObjectName(QStringLiteral("actionPenVanishingLine"));
        actionPenVanishingLine->setCheckable(true);
        actionPenSilhouette = new QAction(MainWindowClass);
        actionPenSilhouette->setObjectName(QStringLiteral("actionPenSilhouette"));
        actionPenSilhouette->setCheckable(true);
        action3DReconstructionAll = new QAction(MainWindowClass);
        action3DReconstructionAll->setObjectName(QStringLiteral("action3DReconstructionAll"));
        actionClearSilhouette = new QAction(MainWindowClass);
        actionClearSilhouette->setObjectName(QStringLiteral("actionClearSilhouette"));
        actionClearBackground = new QAction(MainWindowClass);
        actionClearBackground->setObjectName(QStringLiteral("actionClearBackground"));
        actionTextureMapping = new QAction(MainWindowClass);
        actionTextureMapping->setObjectName(QStringLiteral("actionTextureMapping"));
        actionRenderingLine = new QAction(MainWindowClass);
        actionRenderingLine->setObjectName(QStringLiteral("actionRenderingLine"));
        actionRenderingLine->setCheckable(true);
        actionRenderingSilhouette = new QAction(MainWindowClass);
        actionRenderingSilhouette->setObjectName(QStringLiteral("actionRenderingSilhouette"));
        actionRenderingSilhouette->setCheckable(true);
        actionRenderingBasic = new QAction(MainWindowClass);
        actionRenderingBasic->setObjectName(QStringLiteral("actionRenderingBasic"));
        actionRenderingBasic->setCheckable(true);
        actionGrammarMass = new QAction(MainWindowClass);
        actionGrammarMass->setObjectName(QStringLiteral("actionGrammarMass"));
        actionGrammarMass->setCheckable(true);
        actionGrammarFacade = new QAction(MainWindowClass);
        actionGrammarFacade->setObjectName(QStringLiteral("actionGrammarFacade"));
        actionGrammarFacade->setCheckable(true);
        actionRenderingSSAO = new QAction(MainWindowClass);
        actionRenderingSSAO->setObjectName(QStringLiteral("actionRenderingSSAO"));
        actionRenderingSSAO->setCheckable(true);
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 800, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuTool = new QMenu(menuBar);
        menuTool->setObjectName(QStringLiteral("menuTool"));
        menuPen = new QMenu(menuTool);
        menuPen->setObjectName(QStringLiteral("menuPen"));
        menuRendering = new QMenu(menuTool);
        menuRendering->setObjectName(QStringLiteral("menuRendering"));
        menuGrammar = new QMenu(menuTool);
        menuGrammar->setObjectName(QStringLiteral("menuGrammar"));
        MainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindowClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindowClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuTool->menuAction());
        menuFile->addAction(actionClearBackground);
        menuFile->addAction(actionOpenImage);
        menuFile->addSeparator();
        menuFile->addAction(actionClearLines);
        menuFile->addAction(actionLoadLines);
        menuFile->addAction(actionSaveLines);
        menuFile->addSeparator();
        menuFile->addAction(actionClearSilhouette);
        menuFile->addAction(actionLoadSilhouette);
        menuFile->addAction(actionSaveSilhouette);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuEdit->addAction(actionUndo);
        menuTool->addAction(actionComputeVanishingPoint);
        menuTool->addAction(actionComputeCameraMatrix);
        menuTool->addSeparator();
        menuTool->addAction(menuRendering->menuAction());
        menuTool->addAction(actionTextureMapping);
        menuTool->addSeparator();
        menuTool->addAction(action3DReconstruction);
        menuTool->addAction(action3DReconstructionAll);
        menuTool->addSeparator();
        menuTool->addAction(menuPen->menuAction());
        menuTool->addAction(menuGrammar->menuAction());
        menuTool->addSeparator();
        menuTool->addAction(actionOption);
        menuPen->addAction(actionPenVanishingLine);
        menuPen->addAction(actionPenSilhouette);
        menuRendering->addAction(actionRenderingLine);
        menuRendering->addAction(actionRenderingSilhouette);
        menuRendering->addAction(actionRenderingBasic);
        menuRendering->addAction(actionRenderingSSAO);
        menuGrammar->addAction(actionGrammarMass);
        menuGrammar->addAction(actionGrammarFacade);

        retranslateUi(MainWindowClass);

        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "Vanishing Point", 0));
        actionExit->setText(QApplication::translate("MainWindowClass", "Exit", 0));
        actionOpenImage->setText(QApplication::translate("MainWindowClass", "Open Image", 0));
        actionOpenImage->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+O", 0));
        actionUndo->setText(QApplication::translate("MainWindowClass", "Undo", 0));
        actionUndo->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+Z", 0));
        actionSaveLines->setText(QApplication::translate("MainWindowClass", "Save Lines", 0));
        actionComputeVanishingPoint->setText(QApplication::translate("MainWindowClass", "Compute Vanishing Point", 0));
        actionLoadLines->setText(QApplication::translate("MainWindowClass", "Load Lines", 0));
        actionComputeCameraMatrix->setText(QApplication::translate("MainWindowClass", "Compute Camera Matrix", 0));
        actionClearLines->setText(QApplication::translate("MainWindowClass", "Clear Lines", 0));
        actionClearLines->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+N", 0));
        actionCentering->setText(QApplication::translate("MainWindowClass", "Centering", 0));
        actionOption->setText(QApplication::translate("MainWindowClass", "Option", 0));
        action3DReconstruction->setText(QApplication::translate("MainWindowClass", "3D Reconstruction", 0));
        actionLoadSilhouette->setText(QApplication::translate("MainWindowClass", "Load Silhouette", 0));
        actionSaveSilhouette->setText(QApplication::translate("MainWindowClass", "Save Silhouette", 0));
        actionPenVanishingLine->setText(QApplication::translate("MainWindowClass", "Vanishing Line", 0));
        actionPenSilhouette->setText(QApplication::translate("MainWindowClass", "Silhouette", 0));
        action3DReconstructionAll->setText(QApplication::translate("MainWindowClass", "3D Reconstruction for All", 0));
        actionClearSilhouette->setText(QApplication::translate("MainWindowClass", "Clear Silhouette", 0));
        actionClearBackground->setText(QApplication::translate("MainWindowClass", "Clear Background", 0));
        actionTextureMapping->setText(QApplication::translate("MainWindowClass", "Texture Mapping", 0));
        actionRenderingLine->setText(QApplication::translate("MainWindowClass", "Line", 0));
        actionRenderingSilhouette->setText(QApplication::translate("MainWindowClass", "Silhouette", 0));
        actionRenderingBasic->setText(QApplication::translate("MainWindowClass", "Basic", 0));
        actionGrammarMass->setText(QApplication::translate("MainWindowClass", "Building Mass", 0));
        actionGrammarFacade->setText(QApplication::translate("MainWindowClass", "Facade", 0));
        actionRenderingSSAO->setText(QApplication::translate("MainWindowClass", "SSAO", 0));
        menuFile->setTitle(QApplication::translate("MainWindowClass", "File", 0));
        menuEdit->setTitle(QApplication::translate("MainWindowClass", "Edit", 0));
        menuTool->setTitle(QApplication::translate("MainWindowClass", "Tool", 0));
        menuPen->setTitle(QApplication::translate("MainWindowClass", "Pen", 0));
        menuRendering->setTitle(QApplication::translate("MainWindowClass", "Rendering", 0));
        menuGrammar->setTitle(QApplication::translate("MainWindowClass", "Grammar", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
