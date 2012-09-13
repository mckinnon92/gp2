//Are we on the windows platform?
#ifdef WIN32
//if so include the header for windows applications
#include <Windows.h>
#endif

#include "Win32Window.h"
#include "GameApplication.h"


//Check to see if we are on windows
#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, 
					HINSTANCE hPrevInstance, 
					LPSTR lpCmdLine,
					int nCmdShow)

#else
//we are on some other platform
int main(int argc, char **argv)
#endif
{
	//Creates the Game Application in the main window
	CGameApplication *pApp=new CGameApplication();

	//Checks to see if it can be initialized 
	if (!pApp->init())
	{
		if (pApp)
		{
			delete pApp;
			pApp=NULL;
			return 1;
		}
	}

	//Runs the app
	pApp->run();

	//If it cannot run, deletes the pointer
	if (pApp)
	{
		delete pApp;
		pApp=NULL;
	}

	return 0;
}

