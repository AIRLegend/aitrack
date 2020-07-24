#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>


#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtUiTools/QUiLoader>
#include <QtCore/QFile>



#define Q_OBJECT


class ViewMain : public QWidget
	Q_OBJECT
{
public:
	explicit ViewMain(QWidget* parent = 0);

signals:

public slots:
};

