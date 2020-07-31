#pragma once

#include "../model/Config.h"

class IPresenter
{
public:
	virtual void toggle_tracking() = 0;
	virtual void save_prefs(const ConfigData& data) = 0;
	virtual void close_program() = 0;
};