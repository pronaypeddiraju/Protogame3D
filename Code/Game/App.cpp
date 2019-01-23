#include "Game/App.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/Game.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/XMLUtils/XMLUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystems.hpp"

App* g_theApp = nullptr;
//extern InputSystem* g_inputSystem;

//------------------------------------------------------------------------------------------------------------------------------
App::App()
{
	//Constructor method
	
}

App::~App()
{
	//De constructor method
}

STATIC bool App::Command_Quit(EventArgs& args)
{
	UNUSED(args);
	g_theApp->HandleQuitRequested();
	return true;
}

void App::LoadGameBlackBoard()
{
	const char* xmlDocPath = "Data/Gameplay/GameConfig.xml";
	tinyxml2::XMLDocument gameconfig;
	gameconfig.LoadFile(xmlDocPath);
	
	if(gameconfig.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		printf("\n >> Error loading XML file from %s ", xmlDocPath);
		printf("\n >> Error ID : %i ", gameconfig.ErrorID());
		printf("\n >> Error line number is : %i", gameconfig.ErrorLineNum());
		printf("\n >> Error name : %s", gameconfig.ErrorName());
		ERROR_AND_DIE(">> Error loading GameConfig XML file ")
		return;
	}
	else
	{
		//We read everything fine. Now just shove all that data into the black board
		XMLElement* rootElement = gameconfig.RootElement();
		g_gameConfigBlackboard.PopulateFromXmlElementAttributes(*rootElement);

	}

}

void App::StartUp()
{
	//Load initial black board here
	LoadGameBlackBoard();

	//Create event system
	g_eventSystem = new EventSystems();

	//Create the Render Context
	g_renderContext = new RenderContext();
	g_renderContext->Startup();
	
	//Create the Input System
	g_inputSystem = new InputSystem();

	//Create the Audio System
	g_audio = new AudioSystem();

	//Create dev console
	g_devConsole = new DevConsole();
	g_devConsole->Startup();

	//create the networking system
	//g_networkSystem = new NetworkSystem();

	//Create the game here
	m_game = new Game();
	m_game->StartUp();
	
	g_eventSystem->SubscribeEventCallBackFn("Quit", Command_Quit);
}

void App::ShutDown()
{
	delete g_theApp;			
	g_theApp = nullptr;

	delete g_renderContext;
	g_renderContext = nullptr;

	delete g_inputSystem;
	g_inputSystem = nullptr;

	delete g_audio;
	g_audio = nullptr;

	delete g_devConsole;
	g_devConsole = nullptr;

	delete g_eventSystem;
	g_eventSystem = nullptr;
}

void App::RunFrame()
{
	
	BeginFrame();	
	
	Update();
	Render();	

	PostRender();

	EndFrame();
}


void App::EndFrame()
{
	g_renderContext->EndFrame();
	g_inputSystem->EndFrame();
	g_audio->EndFrame();
	g_devConsole->EndFrame();
	g_eventSystem->EndFrame();
}

void App::BeginFrame()
{
	g_renderContext->BeginFrame();
	g_inputSystem->BeginFrame();
	g_audio->BeginFrame();
	g_eventSystem->BeginFrame();
	g_devConsole->BeginFrame();
}

void App::Update()
{	
	m_timeAtLastFrameBegin = m_timeAtThisFrameBegin;
	m_timeAtThisFrameBegin = GetCurrentTimeSeconds();
	float deltaTime = static_cast<float>(m_timeAtThisFrameBegin - m_timeAtLastFrameBegin);

	deltaTime = Clamp(deltaTime, 0.0f, 0.1f);

	m_game->Update(deltaTime);
}

void App::Render() const
{
	m_game->Render();
}

void App::PostRender()
{
	m_game->PostRender();
}

bool App::HandleKeyPressed(unsigned char keyCode)
{
	switch(keyCode)
	{
	case T_KEY:
		//Implement code to slow down the ship (deltaTime /= 10)
		m_isSlowMo = true;
		return true;
	break;
	case  P_KEY:
		//Implement code to pause game (deltaTime = 0)
		m_isPaused = !m_isPaused;
		return true;
	break;
	case UP_ARROW:
	case RIGHT_ARROW:
	case LEFT_ARROW:	
	case A_KEY:
	case N_KEY:
	case F1_KEY:
	case F2_KEY:
	case F3_KEY:
	case F4_KEY:
	case F5_KEY:
	case F6_KEY:
	case F7_KEY:
	case SPACE_KEY:
		m_game->HandleKeyPressed(keyCode);
		return true;
	break;
	case F8_KEY:
	//Kill and restart the app
	delete m_game;
	m_game = nullptr;
	m_game = new Game();
	m_game->StartUp();
	return true;
	break;
	default:
		//Nothing to worry about
		return false;
	break;
	}
}

bool App::HandleKeyReleased(unsigned char keyCode)
{
	switch(keyCode)
	{
	case 'T':
		//Implement code to return deltaTime to original value
		m_isSlowMo = false;
		return true;
	break;
	case  'P':
		//Implement code to un-pause game
		m_isPaused = false;
		return true;
	break;
	case 38:
	case 32:
	case 39:
	case 37:
		m_game->HandleKeyReleased(keyCode);
		return true;
	break;

	default:
	//Nothing to worry about
	return false;
	break;
	}


}

bool App::HandleQuitRequested()
{
	m_isQuitting = true;
	return m_isQuitting;
}