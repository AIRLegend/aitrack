#pragma once

#include <QWidget>
#include "ui_ConfigWindow.h"

#include "i_view.h"

class ConfigWindow : public QWidget
{
	Q_OBJECT

public:
	ConfigWindow(QWidget *parent = Q_NULLPTR);
	~ConfigWindow();
	void subscribe(IObserver observer);

private:
	Ui::ConfigWindow ui;
};
