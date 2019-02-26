//------------------------------------------------------------------------------------------------------------------------------
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystems.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Renderer/ColorTargetView.hpp"
#include "Engine/Renderer/CPUMesh.hpp"

//------------------------------------------------------------------------------------------------------------------------------
//Create Camera and set to null 
//Camera *g_mainCamera = nullptr; // Define these next, and group by data type - primitives first, structs next, classes next; spaces only necessary if there are clear categories

float g_shakeAmount = 0.0f;

RandomNumberGenerator* g_randomNumGen;
extern RenderContext* g_renderContext;	// Declare these first
extern AudioSystem* g_audio;
bool g_debugMode = false;

//------------------------------------------------------------------------------------------------------------------------------
Game::Game()
{
	m_isGameAlive = true;
	m_testAudioID = g_audio->CreateOrGetSound("Data/Audio/UproarLilWayne.mp3");

	m_squirrelFont = g_renderContext->CreateOrGetBitmapFontFromFile("SquirrelFixedFont");
	g_devConsole->SetBitmapFont(*m_squirrelFont);

}

Game::~Game()
{
	m_isGameAlive = false;
}

void Game::StartUp()
{
	//Create the Camera and setOrthoView
	m_mainCamera = new Camera();
	m_mainCamera->SetColorTarget(nullptr);

	//Create a devConsole Cam
	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetColorTarget(nullptr);

	m_devConsoleCamera->SetOrthoView(Vec2(-10.f, -10.f), Vec2(10.f, 10.f));

	//Set Projection Perspective for new Cam
	m_camPosition = Vec3(0.f, 0.f, -10.f);
	m_mainCamera->SetColorTarget(nullptr);
	m_mainCamera->SetPerspectiveProjection( m_camFOVDegrees, 0.1f, 100.0f, SCREEN_ASPECT);
	
	//m_mainCamera->SetOrthoView(Vec2(-10.f * SCREEN_ASPECT, -10.f), Vec2(10.f * SCREEN_ASPECT, 10.f));

	m_clearScreenColor = new Rgba(0.f, 0.f, 0.5f, 1.f);

	g_devConsole->PrintString(Rgba::BLUE, "this is a test string");
	g_devConsole->PrintString(Rgba::RED, "this is also a test string");
	g_devConsole->PrintString(Rgba::GREEN, "damn this dev console lit!");
	g_devConsole->PrintString(Rgba::WHITE, "Last thing I printed");

	g_eventSystem->SubscribeEventCallBackFn("TestEvent", TestEvent);

	//Get the Shader
	m_shader = g_renderContext->CreateOrGetShaderFromFile(m_defaultShaderPath);

	//Get the test texture
	m_textureTest = g_renderContext->GetOrCreateTextureViewFromFile(m_testImagePath);

	//Meshes for A4
	CPUMesh mesh;

	// create a cube (centered at zero, with sides 2 length)
	/*
	CPUMeshAddCube( &mesh, AABB3::ThatContains( Vec3(-1.0f), Vec3( 1.0f ) ) ); 
	m_cube = new GPUMesh( ctx ); 
	m_cube->CreateFromCPUMesh( mesh, GPU_MEMORY_USAGE_STATIC ); // we won't be updated this; 

																// create a sphere, cenetered at zero, with 
	mesh.Clear();
	CPUMeshAddUVSphere( &mesh, vec3::ZERO, 1.0f );  
	m_sphere = new GPUMesh( ctx ); 
	m_sphere->CreateFromCPUMesh( mesh, GPU_MEMORY_USAGE_STATIC );
	*/
}

STATIC bool Game::TestEvent(EventArgs& args)
{
	UNUSED(args);
	g_devConsole->PrintString(Rgba::YELLOW, "This a test event called from Game.cpp");
	return true;
}

void Game::HandleKeyPressed(unsigned char keyCode)
{
	if(g_devConsole->IsOpen())
	{
		g_devConsole->HandleKeyDown(keyCode);
	}

	switch( keyCode )
	{
		case UP_ARROW:
		case RIGHT_ARROW:
		case LEFT_ARROW:
		case DOWN_ARROW:
		case SPACE_KEY:
		case N_KEY:
		case F1_KEY:
		case F2_KEY:
		case F3_KEY:
		case A_KEY:
		{
			//Handle left movement
			m_camPosition.x -= 0.1f;
		}
		break;
		case W_KEY:
		{
			//Handle forward movement
			m_camPosition.z += 0.1f;
		}
		break;
		case S_KEY:
		{
			//Handle backward movement
			m_camPosition.z -= 0.1f;
		}
		break;
		case D_KEY:
		{
			//Handle right movement
			m_camPosition.x += 0.1f;
		}
		break;
		case F4_KEY:
		{
			//Set volume back to 1
			//g_audio->SetSoundPlaybackVolume(m_testPlayback, 1.0f);
			//Unsub test
			g_eventSystem->UnsubscribeEventCallBackFn("TestEvent", TestEvent);
			break;
		}
		case F5_KEY:
		{
			//Set volume to 0
			//g_audio->SetSoundPlaybackVolume(m_testPlayback, 0.0f);
			//Help Debug
			g_eventSystem->FireEvent("Help");
			break;
		}
		case F6_KEY:
		{
			//Fire event
			g_eventSystem->FireEvent("TestEvent");
			break;
		}
		case F7_KEY:
		{
			//Quit Debug
			g_eventSystem->FireEvent("Quit");
			break;
		}
		default:
		break;
	}
}

//Function that handles debug mode enabled
void Game::DebugEnabled()
{
	g_debugMode = !g_debugMode;
}


void Game::Shutdown()
{
	delete m_mainCamera;
	m_mainCamera = nullptr;

	//FreeResources();
}

void Game::HandleKeyReleased(unsigned char keyCode)
{
	if(g_devConsole->IsOpen())
	{
		g_devConsole->HandleKeyUp(keyCode);
		return;
	}

	//SoundID testSound = g_audio->CreateOrGetSound( "Data/Audio/TestSound.mp3" );
	switch( keyCode )
	{
		case UP_ARROW:
		case RIGHT_ARROW:
		case LEFT_ARROW:
		//g_audio->PlaySound( m_testAudioID );
		break;
		default:
		break;
	}
}

void Game::HandleCharacter( unsigned char charCode )
{
	if(g_devConsole->IsOpen())
	{
		g_devConsole->HandleCharacter(charCode);
		return;
	}
}

void Game::Render() const
{
	//Get the ColorTargetView from rendercontext
	ColorTargetView *colorTargetView = g_renderContext->GetFrameColorTarget();

	//Setup what we are rendering to
	m_mainCamera->SetColorTarget(colorTargetView);

	// Move the camera to where it is in the scene
	// (right now, no rotation (looking forward), set 10 back (so looking at 0,0,0)
	Matrix44 camTransform = Matrix44::MakeFromEuler( m_camEuler, m_camPosition, m_rotationOrder ); 
	m_mainCamera->SetModelMatrix(camTransform);

	m_mainCamera->UpdateUniformBuffer(g_renderContext);

	g_renderContext->BeginCamera(*m_mainCamera); 
	g_renderContext->ClearColorTargets(Rgba::BLACK);

	//Bind the shader we are using (This case it's the default shader we made in Shaders folder)
	g_renderContext->BindShader( m_shader );
	//Bind the Texture to be used
	g_renderContext->BindTextureViewWithSampler( 0U, m_textureTest); 

	std::vector<Vertex_PCU> triangleVerts;
	
	triangleVerts.push_back(Vertex_PCU(Vec3(-1.f, -1.f, 0.f), Rgba::WHITE, Vec2::ZERO));
	triangleVerts.push_back(Vertex_PCU(Vec3(1.f, -1.f, 0.f), Rgba::WHITE, Vec2::ZERO));
	triangleVerts.push_back(Vertex_PCU(Vec3(-1.f, 1.f, 0.f), Rgba::WHITE, Vec2::ZERO));

	g_renderContext->DrawVertexArray(triangleVerts);

	
	if(!m_consoleDebugOnce)
	{
		EventArgs* args = new EventArgs("TestString", "This is a test");
		g_devConsole->Command_Test(*args);
		g_devConsole->ExecuteCommandLine("Exec Health=25");
		g_devConsole->ExecuteCommandLine("Exec Health=85 Armor=100");
	}

	if(g_devConsole->IsOpen())
	{
		g_renderContext->BindTextureViewWithSampler( 0U, m_squirrelFont->GetTexture()); 
		g_devConsole->Render(*g_renderContext, *m_devConsoleCamera, DEVCONSOLE_LINE_HEIGHT);
	}

	g_renderContext->EndCamera();
}

void Game::PostRender()
{
	//Debug bools
	m_consoleDebugOnce = true;
}

void Game::Update( float deltaTime )
{

	if(g_devConsole->GetFrameCount() > 1)
	{
		ColorTargetView *colorTargetView = g_renderContext->GetFrameColorTarget();
		float height = colorTargetView->m_height;
		float width = colorTargetView->m_width;
		float aspect = width / height; 

		float desiredHeight = WORLD_HEIGHT; 
		float desiredWidth = desiredHeight * aspect; 

		m_devConsoleCamera->SetOrthoView(Vec2(-desiredWidth * 0.5f, -desiredHeight * 0.5f), Vec2(desiredWidth * 0.5f, desiredHeight * 0.5f));
		//m_devConsoleCamera->SetOrthoView(Vec2(-10.f, -10.f), Vec2(10.f, 10.f));
	}

	//UpdateCamera(deltaTime);
	g_renderContext->m_frameCount++;

	CheckXboxInputs();
	m_animTime += deltaTime;
	
	//Update the camera's transform
	Matrix44 camTransform = Matrix44::MakeFromEuler( m_camEuler, m_camPosition, m_rotationOrder ); 
	m_mainCamera->SetModelMatrix(camTransform);
	
	CheckCollisions();

	ClearGarbageEntities();	
}

//Use this chunk of code only for screen shake!
/*
void Game::UpdateCamera(float deltaTime)
{
	g_mainCamera = new Camera();
	Vec2 orthoBottomLeft = Vec2(0.f,0.f);
	Vec2 orthoTopRight = Vec2(WORLD_WIDTH, WORLD_HEIGHT);
	g_mainCamera->SetOrthoView(orthoBottomLeft, orthoTopRight);

	float shakeX = 0.f;
	float shakeY = 0.f;

	if(g_shakeAmount > 0)
	{
		shakeX = g_randomNumGen->GetRandomFloatInRange(-g_shakeAmount, g_shakeAmount);
		shakeY = g_randomNumGen->GetRandomFloatInRange(-g_shakeAmount, g_shakeAmount);

		g_shakeAmount -= deltaTime * CAMERA_SHAKE_REDUCTION_PER_SECOND;
	}
	else
	{
		g_shakeAmount = 0;
	}

	Vec2 translate2D = Vec2(shakeX, shakeY);
	translate2D.ClampLength(MAX_SHAKE);
	g_mainCamera->Translate2D(translate2D);
}
*/

void Game::ClearGarbageEntities()
{
}

void Game::CheckXboxInputs()
{
	//XboxController playerController = g_inputSystem->GetXboxController(0);
}

void Game::CheckCollisions()
{		
}

bool Game::IsAlive()
{
	//Check if alive
	return m_isGameAlive;
}
