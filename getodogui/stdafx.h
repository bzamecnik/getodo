// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef GETODOGUI_STDAFX_H__
#define GETODOGUI_STDAFX_H__

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>

// placing gtkmm in precompiled header could greatly improve
// compilation time
#include <gtkmm.h>

#endif // GETODOGUI_STDAFX_H__