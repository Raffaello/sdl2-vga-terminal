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
typedef VGA_Terminal* (__cdecl* PROCINIT)();
#endif

#ifdef linux

#include <dlfcn.h>

#endif

#if defined(DEBUG) || !defined(NDBEBUG)

#define VGA_SO_NAME "vga-terminald"

#else

#define VGA_SO_NAME "vga-terminal"

#endif

#ifdef WIN32

void win32()
{
	const char* soname = TEXT(VGA_SO_NAME ".dll");
	HINSTANCE hinstLib;
	PROCINIT ProcAddInit = NULL;
	PROCDESTROY ProcAddDestroy = NULL;
	PROCWRITEXY ProcAddWriteXY = NULL;
	PROCRENDER ProcAddRender = NULL;
	BOOL fFreeResult;
	BOOL fRunTimeLinkSuccess = SDL_FALSE;
	
	printf("loading library: %s\n", soname);

	hinstLib = LoadLibrary(soname);
	if (hinstLib != NULL)
	{
		ProcAddInit = (PROCINIT)GetProcAddress(hinstLib, "VGA_TERMINAL_init");
		ProcAddDestroy = (PROCDESTROY)GetProcAddress(hinstLib, "VGA_TERMINAL_destroy");
		ProcAddWriteXY = (PROCWRITEXY)GetProcAddress(hinstLib, "VGA_TERMINAL_writeXY");
		ProcAddRender = (PROCRENDER)GetProcAddress(hinstLib, "VGA_TERMINAL_render");

		// If the function address is valid, call the function.

		if (NULL != ProcAddInit && NULL != ProcAddDestroy && NULL != ProcAddWriteXY && NULL != ProcAddRender)
		{
			fRunTimeLinkSuccess = SDL_TRUE;
			VGA_Terminal* term = ProcAddInit();
			ProcAddWriteXY(term, 0, 0, "GREETINGS PROFESSOR FALKENS.", 10, 0);
			ProcAddRender(term);
			SDL_Delay(1000);
			ProcAddDestroy(term);
		}
		// Free the DLL module.
		fFreeResult = FreeLibrary(hinstLib);
		if (!fFreeResult) {
			fprintf(stderr, "unable to unload library\n");
		}
		else {
			printf("library unloaded.\n");
		}
	} 

	// If unable to call the DLL function, use an alternative.
	if (!fRunTimeLinkSuccess) {
		DWORD dw = GetLastError();

		fprintf(stderr, "failed to load the DLL. Error %lu\n", dw);
	}
}
#endif

#ifdef linux
void lnx()
{
	const char* soname = "./lib" VGA_SO_NAME ".so";
	void* handle = dlopen(soname, RTLD_NOW);

	if (handle) {
		char* error;

		VGA_Terminal* (*init)(void);
		void (*destroy)(VGA_Terminal*);
		void (*writeXY)(VGA_Terminal*, uint8_t, uint8_t, char* str, uint8_t, uint8_t);

		init = dlsym(handle, "VGA_TERMINAL_init");
		if ((error = dlerror()) != NULL) {
			fputs(error, stderr);
			exit(1);
		}

		destroy = dlsym(handle, "VGA_TERMINAL_destroy");
		if ((error = dlerror()) != NULL) {
			fputs(error, stderr);
			exit(1);
		}

		writeXY = dlsym(handle, "VGA_TERMINAL_writeXY");
		if ((error = dlerror()) != NULL) {
			fputs(error, stderr);
			exit(1);
		}

		
		VGA_Terminal* term = (*init)();
		(*writeXY)(term, 0, 0, "test", 10, 0);
		SDL_Delay(1000);
		(*destroy)(term);


		dlclose(handle);
	}
	else {
		fprintf(stderr, "unable to load SO. Error %s\n", dlerror());
	}

}
#endif

int main(int, char**)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	
#ifdef WIN32
	win32();
#endif
#ifdef linux
	lnx();
#endif

	SDL_Quit();
	return 0;
}
