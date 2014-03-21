#include "StdAfx.h"
#include <cstdlib>
#include "fileChooser.h"


fileChooser::fileChooser(const char *startDir, const char* filter, const int type, const char * title)
{
		chooser = new Fl_File_Chooser(startDir, filter,	type, title);
		chooser->preview(0);
}


fileChooser::~fileChooser(void)
{
}

LPCWSTR fileChooser::show(){
	chooser->show();

	// Block until user picks something.
	//     (The other way to do this is to use a callback())
	//
	while(chooser->shown())
		{ Fl::wait(); }

	const char *val = chooser->value();
	// User hit cancel?
	if ( val == NULL ){
		fprintf(stderr, "(User hit 'Cancel')\n");
		return NULL;
	}

	// Print what the user picked
	fprintf(stderr, "--------------------\n");
	fprintf(stderr, "DIRECTORY: '%s'\n", chooser->directory());
	fprintf(stderr, "    VALUE: '%s'\n", val);
	fprintf(stderr, "    COUNT: %d files selected\n", chooser->count());

	mbstowcs(wideString, val, MAX_WIDE_CHARS);

	return wideString;
}
