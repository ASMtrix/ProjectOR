// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include "resource.h"
#include <stdio.h>
#include <tlhelp32.h>
#include <Winternl.h>
#include <psapi.h>

class Checkmate
{
	public:
		HINSTANCE dialogINST;

		void Run();
		void Main();

		bool quit;


		Checkmate();
		~Checkmate();
		DWORD SpeedHack();
};

// TODO: reference additional headers your program requires here
