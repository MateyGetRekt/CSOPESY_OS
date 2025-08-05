#include "screenManager.h"
#include <string>
#include <map>
#include "screen.h"

ScreenManager::ScreenManager() : current_screen("main"){
}

void ScreenManager::addScreen(Screen& screen) {
	screens_map.insert({ screen.getProcessName(),screen });
}

void ScreenManager::setCurrentScreen(std::string processName) {
	current_screen = processName;
}

std::map<std::string, Screen>& ScreenManager::getScreensMap() {
	return screens_map;
}

std::string ScreenManager::nameCurrentScreen() {
	return current_screen;
}

Screen& ScreenManager::getCurrentScreen() {
	return getScreen(current_screen);
}

Screen& ScreenManager::getScreen(std::string processName) {
	return screens_map[processName];
}

int ScreenManager::get_min_ins()
{
	return min_ins;
}

int ScreenManager::get_max_ins()
{
	return max_ins;
}

void ScreenManager::set_min_ins(int min_ins)
{
	this->min_ins = min_ins;
}

void ScreenManager::set_max_ins(int max_ins)
{
	this->max_ins = max_ins;
}

void ScreenManager::removeScreen(std::string process_name) {
    if (current_screen == process_name) {
        return;
    }
	screens_map.erase(process_name);
}
