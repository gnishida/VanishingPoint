#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include "ui_OptionDialog.h"

class OptionDialog : public QDialog {
	Q_OBJECT

private:
	Ui::OptionDialog ui;
	QColor horizontalLeftLineColor;
	QColor horizontalRightLineColor;
	QColor verticalLineColor;
	QColor silhouetteColor;

public:
	OptionDialog(QWidget *parent = 0);
	~OptionDialog();

	void setVanishingLineWidth(int vanishingLineWidth);
	int getVanishingLineWidth();
	void setHorizontalLeftColor(const QColor& horizontalLeftLineColor);
	QColor getHorizontalLeftColor();
	void setHorizontalRightColor(const QColor& horizontalRightLineColor);
	QColor getHorizontalRightColor();
	void setVerticalColor(const QColor& verticalLineColor);
	QColor getVerticalColor();
	void setSilhouetteWidth(int silhouetteWidth);
	int getSilhouetteWidth();
	void setSilhouetteColor(const QColor& silhouetteColor);
	QColor getSilhouetteColor();
	void setMassGrammarId(int massGrammarId);
	int getMassGrammarId();
	void setFacadeGrammarId(int facadeGrammarId);
	int getFacadeGrammarId();
	void setWindowGrammarId(int windowGrammarId);
	int getWindowGrammarId();

public slots:
	void onHorizontalLeftLineColor();
	void onHorizontalRightLineColor();
	void onVerticalLineColor();
	void onSilhouetteColor();
	void onOK();
	void onCancel();
};

#endif // OPTIONDIALOG_H
