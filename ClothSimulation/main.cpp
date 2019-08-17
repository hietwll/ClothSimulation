#include "ClothSimulation.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ClothSimulation w;
	w.show();
	return a.exec();
}
