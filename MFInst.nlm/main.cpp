//
//
// + MailFilter Installation Agent
//
//   Copyright 2004 Christian Hofstaedtler
//
//

//#define _MAILFILTER_MAIN_
#define _MFD_MODULE				"main.cpp"

#include "MailFilter.h"

void MFD_Out_func(int attr, const char* format, ...)
{
#pragma unused(attr,format)
}


int main( int argc, char *argv[ ])
{
#pragma unused(argc,argv)
	// TODO

	MailFilter_Configuration::Configuration config;
	config.setDefaults("sys:\\etc\\mailflt");
	
	config.WriteToFile("");


}