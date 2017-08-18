#include "OptionDialog.h"
#include <QColorDialog>

OptionDialog::OptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.spinBoxVanishingLineWidth->setRange(1, 10);
	ui.spinBoxSilhouetteLineWidth->setRange(1, 10);
	ui.spinBoxMassGrammarId->setRange(1, 10);
	ui.spinBoxFacadeGrammarId->setRange(1, 10);
	ui.spinBoxWindowGrammarId->setRange(1, 10);

	connect(ui.pushButtonHorizontalLeftLineColor, SIGNAL(clicked()), this, SLOT(onHorizontalLeftLineColor()));
	connect(ui.pushButtonHorizontalRightLineColor, SIGNAL(clicked()), this, SLOT(onHorizontalRightLineColor()));
	connect(ui.pushButtonVerticalLineColor, SIGNAL(clicked()), this, SLOT(onVerticalLineColor()));
	connect(ui.pushButtonSilhouetteColor, SIGNAL(clicked()), this, SLOT(onSilhouetteColor()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

OptionDialog::~OptionDialog() {
}

void OptionDialog::setVanishingLineWidth(int vanishingLineWidth) {
	ui.spinBoxVanishingLineWidth->setValue(vanishingLineWidth);
}
int OptionDialog::getVanishingLineWidth() {
	return ui.spinBoxVanishingLineWidth->value();
}

void OptionDialog::setHorizontalLeftColor(const QColor& horizontalLeftLineColor) {
	this->horizontalLeftLineColor = horizontalLeftLineColor;
	ui.pushButtonHorizontalLeftLineColor->setStyleSheet("border: 2px solid black; background-color: " + horizontalLeftLineColor.name() + ";");
}

QColor OptionDialog::getHorizontalLeftColor() {
	return horizontalLeftLineColor;
}

void OptionDialog::setHorizontalRightColor(const QColor& horizontalRightLineColor) {
	this->horizontalRightLineColor = horizontalRightLineColor;
	ui.pushButtonHorizontalRightLineColor->setStyleSheet("border: 2px solid black; background-color: " + horizontalRightLineColor.name() + ";");
}

QColor OptionDialog::getHorizontalRightColor() {
	return horizontalRightLineColor;
}

void OptionDialog::setVerticalColor(const QColor& verticalLineColor) {
	this->verticalLineColor = verticalLineColor;
	ui.pushButtonVerticalLineColor->setStyleSheet("border: 2px solid black; background-color: " + verticalLineColor.name() + ";");
}

QColor OptionDialog::getVerticalColor() {
	return verticalLineColor;
}

void OptionDialog::setSilhouetteWidth(int silhouetteWidth) {
	ui.spinBoxSilhouetteLineWidth->setValue(silhouetteWidth);
}

int OptionDialog::getSilhouetteWidth() {
	return ui.spinBoxSilhouetteLineWidth->value();
}

void OptionDialog::setSilhouetteColor(const QColor& silhouetteColor) {
	this->silhouetteColor = silhouetteColor;
	ui.pushButtonSilhouetteColor->setStyleSheet("border: 2px solid black; background-color: " + silhouetteColor.name() + ";");
}

QColor OptionDialog::getSilhouetteColor() {
	return silhouetteColor;
}

void OptionDialog::setMassGrammarId(int massGrammarId) {
	ui.spinBoxMassGrammarId->setValue(massGrammarId + 1);
}

int OptionDialog::getMassGrammarId() {
	return ui.spinBoxMassGrammarId->value() - 1;
}

void OptionDialog::setFacadeGrammarId(int facadeGrammarId) {
	ui.spinBoxFacadeGrammarId->setValue(facadeGrammarId + 1);
}

int OptionDialog::getFacadeGrammarId() {
	return ui.spinBoxFacadeGrammarId->value() - 1;
}

void OptionDialog::setWindowGrammarId(int windowGrammarId) {
	ui.spinBoxWindowGrammarId->setValue(windowGrammarId + 1);
}

int OptionDialog::getWindowGrammarId() {
	return ui.spinBoxWindowGrammarId->value() - 1;
}

void OptionDialog::onHorizontalLeftLineColor() {
	QColor tmp = QColorDialog::getColor(horizontalLeftLineColor);
	if (tmp.isValid()) {
		setHorizontalLeftColor(tmp);
	}
}

void OptionDialog::onHorizontalRightLineColor() {
	QColor tmp = QColorDialog::getColor(horizontalRightLineColor);
	if (tmp.isValid()) {
		setHorizontalRightColor(tmp);
	}
}

void OptionDialog::onVerticalLineColor() {
	QColor tmp = QColorDialog::getColor(verticalLineColor);
	if (tmp.isValid()) {
		setVerticalColor(tmp);
	}
}

void OptionDialog::onSilhouetteColor() {
	QColor tmp = QColorDialog::getColor(silhouetteColor);
	if (tmp.isValid()) {
		setSilhouetteColor(tmp);
	}
}

void OptionDialog::onOK() {
	accept();
}

void OptionDialog::onCancel() {
	reject();
}
