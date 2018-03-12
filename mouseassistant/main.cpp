#include "mouseassistant.h"
#include <QtWidgets/QApplication>
//#include <QTextCodec>

int main(int argc, char *argv[])
{
	//QTextCodec *dfCode= QTextCodec::codecForName("gb2312");
	//QTextCodec::setCodecForLocale(dfCode);
	//QTextCodec::setCodecForTr(dfCode);
	//QTextCodec::setCodecForCStrings(dfCode);

	QApplication a(argc, argv);
	mouseassistant w;
	w.show();
	return a.exec();
}
