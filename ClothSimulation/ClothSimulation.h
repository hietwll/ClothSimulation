#ifndef CLOTHSIMULATION_H
#define CLOTHSIMULATION_H

#include <QtWidgets/QMainWindow>
#include "ui_ClothSimulation.h"

class ClothSimulation : public QMainWindow
{
	Q_OBJECT

public:
	ClothSimulation(QWidget *parent = Q_NULLPTR);

private:
	Ui::ClothSimulationClass ui;
};


#endif // !CLOTHSIMULATION_H