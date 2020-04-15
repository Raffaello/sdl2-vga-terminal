/********************************************
 * USING THE SO/DLL in a C program example  *
 * The DLL/SO is loaded at run-time.    *
 ********************************************/

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>

typedef void VGA_Terminal;

#ifdef WIN32
#include <Windows.h>
typedef void(__cdecl* PROCWRITEXY)(VGA_Terminal*, uint8_t, uint8_t, char* str, uint8_t, uint8_t);
typedef void(__cdecl* PROCDESTROY)(VGA_Terminal*);
typedef void(__cdecl* PROCRENDER)(VGA_Terminal*);
typedef VGA_Terminal*(__cdecl* PROCINIT)();
#endif


#ifdef _DEBUG
#define VGA_SO_NAME "vga-terminal-libd"
#else
#define VGA_SO_NAME "vga-terminal-lib"
#endif



#ifdef WIN32
void win32()
{
	const char* soname = VGA_SO_NAME ".dll";
	HINSTANCE hinstLib;
	PROCINIT ProcAddInit = NULL;
	PROCDESTROY ProcAddDestroy = NULL;
	PROCWRITEXY ProcAddWriteXY = NULL;
	PROCRENDER ProcAddRender = NULL;
	BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;
	printf("loading library: %s\n", soname);
	
	hinstLib = LoadLibrary(TEXT("vga-terminal-libd.dll"));
	if (hinstLib != NULL)
	{
		ProcAddInit = (PROCINIT)GetProcAddress(hinstLib, "VGA_TERMINAL_init");
		ProcAddDestroy = (PROCDESTROY)GetProcAddress(hinstLib, "VGA_TERMINAL_destroy");
		ProcAddWriteXY = (PROCWRITEXY)GetProcAddress(hinstLib, "VGA_TERMINAL_writeXY");
		ProcAddRender = (PROCRENDER)GetProcAddress(hinstLib, "VGA_TERMINAL_render");

		// If the function address is valid, call the function.

		if (NULL != ProcAddInit && NULL != ProcAddDestroy && NULL != ProcAddWriteXY && NULL != ProcAddRender)
		{
			fRunTimeLinkSuccess = TRUE;
			VGA_Terminal* term = ProcAddInit();
			ProcAddWriteXY(term, 0, 0, "test", 10, 0);
			ProcAddRender(term);
			SDL_Delay(1000);
			ProcAddDestroy(term);
		}
		// Free the DLL module.

		fFreeResult = FreeLibrary(hinstLib);
	}

	// If unable to call the DLL function, use an alternative.
	if (!fRunTimeLinkSuccess) {
		DWORD dw = GetLastError();
		
		fprintf(stderr, "failed to load the DLL. Error %d\n", dw);
	}

}
#endif

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	
#ifdef WIN32
	win32();
#endif

	SDL_Quit();
	return 0;
}
