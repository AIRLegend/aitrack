#include "viewmain.h"


ViewMain::ViewMain(QWidget* parent) 
{
	QUiLoader loader;
    QFile file("C:/Users/Alvaro/source/repos/Camera/Camera/src/view/MainWindow.ui");
    file.open(QFile::ReadOnly);
    QWidget *myWidget = loader.load(&file, this);
    file.close();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(myWidget);
    setLayout(layout);
}