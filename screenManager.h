#pragma once

#include <string>
#include <map>
#include "screen.h"

class ScreenManager {
public:
	ScreenManager();
	void addScreen(Screen& screen);
	void setCurrentScreen(std::string processName);
	void removeScreen(std::string processName);
	std::string nameCurrentScreen();

	std::map<std::string, Screen>& getScreensMap();
	Screen& getCurrentScreen();
	Screen& getScreen(std::string processName);

	int get_min_ins();
	int get_max_ins();

	void set_min_ins(int min_ins);
	void set_max_ins(int max_ins);

private:
    std::map<std::string, Screen> screens_map;
    std::string current_screen;
	int min_ins;
	int max_ins;
};

extern ScreenManager screenManager;
