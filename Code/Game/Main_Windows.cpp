 #define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "Engine/Renderer/RenderContext.hpp"
#include "App.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Game/GameCommon.hpp"

//Purely for debugging
#include <stdio.h>

extern App* g_theApp;

HWND g_hWnd = nullptr;							// ...becomes WindowContext::m_windowHandle
HDC g_displayDeviceContext = nullptr;			// ...becomes WindowContext::m_displayContext
HGLRC g_openGLRenderingContext = nullptr;		// ...becomes RenderContext::m_apiRenderingContext
const char* APP_NAME = "Protogame 3D";	// ...becomes ???

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called by Windows whenever we ask it for notifications
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	switch( wmMessageCode )
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
		case WM_CLOSE:		
		{
			g_theApp->HandleQuitRequested();
			return 0; // "Consumes" this message (tells Windows "okay, we handled it")
		}		

		// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
		case WM_KEYDOWN:
		{
			unsigned char asKey = (unsigned char) wParam;

			switch( asKey )
			{
			case KEY_ESC:
			case UP_ARROW:
			case DOWN_ARROW:
			case SPACE_KEY:				
			case LEFT_ARROW:
			case RIGHT_ARROW:
			case TILDY_KEY:
			case DEL_KEY:
			case BACK_SPACE:
			case ENTER_KEY:
			case F1_KEY:
			case F2_KEY:
			case F3_KEY:
			case F4_KEY:
			case F5_KEY:
			case F6_KEY:
			case F7_KEY:
			case F8_KEY:
			case A_KEY:
			case W_KEY:
			case S_KEY:
			case D_KEY:
				g_theApp->HandleKeyPressed(asKey);
				return 0;
			break;
			}
			break;
		}		

		// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
		case WM_KEYUP:
		{
			unsigned char asKey = (unsigned char) wParam;
			switch( asKey )
			{
			case UP_ARROW:
			case SPACE_KEY:				
			case LEFT_ARROW:
			case RIGHT_ARROW:
			case F1_KEY:
			case F2_KEY:
			case F3_KEY:
			case F4_KEY:
			case F5_KEY:
			case F6_KEY:
			case F7_KEY:
			case F8_KEY:
				g_theApp->HandleKeyReleased(asKey);
				return 0;
			break;
			}
			break;
		}

		case WM_CHAR:
		{
			unsigned char asKey = (unsigned char) wParam;
			
			bool is_released = ((lParam & (1U << 31)) != 0);
			if((asKey >= 20 && asKey <= 126 && !is_released))
			{
				//Ignore escape character and tilde
				if(asKey != 96 && asKey != 27)
				{
					g_theApp->HandleCharacter(asKey);
					return 0;
				}
			}
		}
		break;
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}

void CreateD3D11Window(HINSTANCE applicationInstanceHandle, float clientAspect)
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( WindowsMessageHandlingProcedure ); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
	RegisterClassEx( &windowClassDescription );

	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth = (float)( desktopRect.right - desktopRect.left );
	float desktopHeight = (float)( desktopRect.bottom - desktopRect.top );
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if( clientAspect > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int) clientMarginX;
	clientRect.right = clientRect.left + (int) clientWidth;
	clientRect.top = (int) clientMarginY;
	clientRect.bottom = clientRect.top + (int) clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP(), 0, APP_NAME, -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );
	g_hWnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		applicationInstanceHandle,
		NULL );

	ShowWindow( g_hWnd, SW_SHOW );
	SetForegroundWindow( g_hWnd );
	SetFocus( g_hWnd );

	g_displayDeviceContext = GetDC( g_hWnd );

	HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
	SetCursor( cursor );
}

//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
//
// #SD1ToDo: We will move this function to a more appropriate place later on...
//
void RunMessagePump()
{
	MSG queuedMessage;
	for( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}

//-----------------------------------------------------------------------------------------------
void Startup( HINSTANCE applicationInstanceHandle )
{
	//We create app first and read black board. Then we create window and we get the window data based on black board info to create either full screen/ windowed screen
	//CreateOpenGLWindow( applicationInstanceHandle, CLIENT_ASPECT );

	//Here call a CreateWindow 
	CreateD3D11Window(applicationInstanceHandle, CLIENT_ASPECT);

	//Create the app and pass it the window handle
	g_theApp = new App(g_hWnd);	
	g_theApp->StartUp();
}


//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	// Destroy the global App instance
	delete g_theApp;
	g_theApp = nullptr;
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	UNUSED( commandLineString );
	Startup( applicationInstanceHandle );

	// Program main loop; keep running frames until it's time to quit
	while( !g_theApp->IsQuitting() ) // #SD1ToDo: ...becomes:  !g_theApp->IsQuitting()
	{
		RunMessagePump();
		g_theApp->RunFrame();
		//SwapBuffers(g_displayDeviceContext);
		Sleep(0);
	}

	Shutdown();
	return 0;
}


