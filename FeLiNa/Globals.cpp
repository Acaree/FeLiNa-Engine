#include "Globals.h"
#include "Application.h"

#ifndef GAME_MODE

void log_global(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);
	if (App) {
		sprintf_s(tmp_string2, 4096, "\n%s", tmp_string);
		App->Log_app(tmp_string2);
	}
}

#else
void log_global(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);
	/*if (App) {
		sprintf_s(tmp_string2, 4096, "\n%s", tmp_string);
		App->Log_app(tmp_string2);
	}*/
}

#endif