#pragma once


#include <QtWidgets/QMainWindow>
#include "ui_mouseassistant.h"

#include "dtblueserver.h"

class mouseassistant : public QMainWindow
{
	Q_OBJECT

public:
	mouseassistant(QWidget *parent = Q_NULLPTR);
	~mouseassistant();

protected:
	virtual void closeEvent(QCloseEvent *event);
	virtual void timerEvent(QTimerEvent *event);

private slots:
	void ResetClicked(bool checked);
	void SetDataClicked(bool checked);

private:
	Ui::mouseassistantClass ui;
	QString m_sInterfacePath;

	dtblueserver m_blServer;
};
