/*
 * This file is part of the Advance project.
 *
 * Copyright (C) 2004 Andrea Mazzoleni
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details. 
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "portable.h"

#include "error.h"

static char error_text[512];
static int error_message_interactive;

void error_interactive(int interactive)
{
	error_message_interactive = interactive;
}

#ifdef __WIN32__

#include <windows.h>

void error_set(const char* text, ...)
{
	va_list arg;

	if (!error_text[0]) {
		va_start(arg, text);
		vsprintf(error_text, text, arg);
		if (error_message_interactive)
			MessageBox(NULL, error_text, "makebootfat", MB_ICONERROR);
		va_end(arg);
	}
}

#else

void error_set(const char* text, ...)
{
	va_list arg;

	if (!error_text[0]) {
		va_start(arg, text);
		vsnprintf(error_text, sizeof(error_text), text, arg);
		va_end(arg);
	}
}

#endif

const char* error_get(void)
{
	return error_text;
}

