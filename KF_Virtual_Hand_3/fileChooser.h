#pragma once

#include <Windows.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_File_Chooser.H>

class fileChooser
{
public:
	fileChooser(const char *startDir, const char* filter, const int type, const char * title);
	~fileChooser(void);

	LPCWSTR show();
	static const int MAX_WIDE_CHARS = 1024;

private:
	Fl_File_Chooser *chooser;
	wchar_t wideString[MAX_WIDE_CHARS];
};

