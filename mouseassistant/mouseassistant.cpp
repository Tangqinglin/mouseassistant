/*
                                ______________     ___________________
                               / ___________\ \   /__________________/
                               / /           \ \          |  |
                               / /            \ \         |  |
                               / /            | |         |  |
                               / /            | |         |  |
                               / /            / /         |  |
                               / /___________/ /          |  |
                               \____________/_/           |__|

*/

#include "mouseassistant.h"

#include <qmessagebox.h>

mouseassistant::mouseassistant(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.pushButton, SIGNAL(clicked(bool)), this, SLOT(ResetClicked(bool)));
	connect(ui.pushButton_3, SIGNAL(clicked(bool)), this, SLOT(SetDataClicked(bool)));

	m_blServer.Start();

	startTimer(1000);

}
mouseassistant::~mouseassistant()
{
	
}


void mouseassistant::ResetClicked(bool checked)
{
	
}


void mouseassistant::SetDataClicked(bool checked)
{
	

}

void mouseassistant::closeEvent(QCloseEvent *event)
{
	m_blServer.Stop();
}

void mouseassistant::timerEvent(QTimerEvent *event)
{
	QString sFlag = tr("Status: 0x");
	sFlag = sFlag + QString::number(m_blServer.m_logFlag, 16);
	ui.label_7->setText(sFlag);

	QString sErro = tr("Errno: 0x");
	sErro = sErro + QString::number(m_blServer.m_iOpenFileCode, 16);
	ui.label_8->setText(sErro);
}
