//------------------------------------------------------------------------------------------------------------------------------
#include "Game/Game.hpp"
//Engine Systems
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystems.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/ColorTargetView.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Shader.hpp"

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
	SetupMouseData();
	SetupCameras();

	GetandSetShaders();
	LoadGameTextures();

	g_devConsole->PrintString(Rgba::BLUE, "this is a test string");
	g_devConsole->PrintString(Rgba::RED, "this is also a test string");
	g_devConsole->PrintString(Rgba::GREEN, "damn this dev console lit!");
	g_devConsole->PrintString(Rgba::WHITE, "Last thing I printed");

	g_eventSystem->SubscribeEventCallBackFn("TestEvent", TestEvent);

	CreateInitialMeshes();
}

void Game::SetupMouseData()
{
	IntVec2 clientCenter = g_windowContext->GetClientCenter();
	g_windowContext->SetClientMousePosition(clientCenter);

	g_windowContext->SetMouseMode(MOUSE_MODE_RELATIVE);
	g_windowContext->HideMouse();
}

void Game::SetupCameras()
{
	//Create the Camera and setOrthoView
	m_mainCamera = new Camera();
	m_mainCamera->SetColorTarget(nullptr);

	//Create a devConsole Cam
	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetColorTarget(nullptr);

	//Set Projection Perspective for new Cam
	m_camPosition = Vec3(0.f, 0.f, -10.f);
	m_mainCamera->SetColorTarget(nullptr);
	m_mainCamera->SetPerspectiveProjection( m_camFOVDegrees, 0.1f, 100.0f, SCREEN_ASPECT);
	//m_mainCamera->SetOrthoView(Vec2(-10.f * SCREEN_ASPECT, -10.f), Vec2(10.f * SCREEN_ASPECT, 10.f));

	m_clearScreenColor = new Rgba(0.f, 0.f, 0.5f, 1.f);

}

void Game::SetStartupDebugRenderObjects()
{
	ColorTargetView* ctv = g_renderContext->GetFrameColorTarget();
	//Setup debug render client data
	g_debugRenderer->SetClientDimensions( ctv->m_height, ctv->m_width );

	//Setup Debug Options
	DebugRenderOptionsT options;
	options.mode = DEBUG_RENDER_ALWAYS;
	options.beginColor = Rgba::BLUE;
	options.endColor = Rgba::RED;

	//------------------------------------------------------------------------------------------------------------------------------
	// 2D Objects
	//------------------------------------------------------------------------------------------------------------------------------

	//Make 2D Point on screen
	g_debugRenderer->DebugRenderPoint2D(options, Vec2(10.f, 10.f), 5.0f);
	//Make 2D Point at screen center
	options.beginColor = Rgba::BLUE;
	options.endColor = Rgba::BLACK;
	g_debugRenderer->DebugRenderPoint2D(options, Vec2(0.f, 0.f), 10.f);

	options.beginColor = Rgba::YELLOW;
	options.endColor = Rgba::RED;
	//Draw a line in 2D screen space
	g_debugRenderer->DebugRenderLine2D(options, Vec2(ctv->m_width * -0.5f, ctv->m_height * -0.5f), Vec2(-150.f, -150.f), 20.f);

	//Draw a quad in 2D screen space
	options.beginColor = Rgba::GREEN;
	options.endColor = Rgba::RED;
	g_debugRenderer->DebugRenderQuad2D(options, AABB2(Vec2(-150.f, -150.f), Vec2(-100.f, -100.f)), 20.f);

	//Textured Quad
	options.beginColor = Rgba::WHITE;
	options.endColor = Rgba::RED;
	g_debugRenderer->DebugRenderQuad2D(options, AABB2(Vec2(-200.f, -200.f), Vec2(-150.f, -150.f)), 20.f, m_textureTest);

	//Disc2D
	options.beginColor = Rgba::DARK_GREY;
	options.endColor = Rgba::ORANGE;
	g_debugRenderer->DebugRenderDisc2D(options, Disc2D(Vec2(100.f, 100.f), 25.f), 10.f);

	//Ring2D
	options.beginColor = Rgba::ORANGE;
	options.endColor = Rgba::DARK_GREY;
	g_debugRenderer->DebugRenderRing2D(options, Disc2D(Vec2(100.f, 100.f), 25.f), 10.f, 10.f);


	//Wired Quad
	options.beginColor = Rgba::WHITE;
	options.endColor = Rgba::WHITE;
	g_debugRenderer->DebugRenderWireQuad2D(options, AABB2(Vec2(100.f, -100.f), Vec2(150.f, -50.f)), 20.f);


	//------------------------------------------------------------------------------------------------------------------------------
	// 3D Objects
	//------------------------------------------------------------------------------------------------------------------------------

	DebugRenderOptionsT options3D;
	options3D.space = DEBUG_RENDER_WORLD;
	options3D.beginColor = Rgba::GREEN;
	options3D.endColor = Rgba::RED;

	//make a 3D point
	g_debugRenderer->DebugRenderPoint(options3D, Vec3(10.0f, 0.0f, 0.0f), 10.0f, 0.2f);

	//Make a 3D textured point
	options3D.beginColor = Rgba::WHITE;
	options3D.endColor = Rgba::WHITE;
	g_debugRenderer->DebugRenderPoint(options3D, Vec3(-10.0f, 0.0f, 0.0f), 20.f, 1.f, m_textureTest);

	//Make a line in 3D
	options3D.beginColor = Rgba::WHITE;
	options3D.endColor = Rgba::BLACK;
	g_debugRenderer->DebugRenderLine(options3D, Vec3(0.f, 0.f, 0.f), Vec3(10.f, 0.f, 10.f), 2000.f);
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
		break;
		case A_KEY:
		{
			//Handle left movement
			Vec3 worldMovementDirection = m_mainCamera->m_cameraModel.GetIVector() * -1.f;
			float speed = 0.1f; 
			worldMovementDirection *= (speed);

			m_camPosition += worldMovementDirection; 
		}
		break;
		case W_KEY:
		{
			//Handle forward movement
			Vec3 worldMovementDirection = m_mainCamera->m_cameraModel.GetKVector();
			float speed = 0.1f; 
			worldMovementDirection *= (speed); 

			m_camPosition += worldMovementDirection; 
		}
		break;
		case S_KEY:
		{
			//Handle backward movement
			Vec3 worldMovementDirection = m_mainCamera->m_cameraModel.GetKVector() * -1.f;
			float speed = 0.1f; 
			worldMovementDirection *= (speed); 

			m_camPosition += worldMovementDirection; 
		}
		break;
		case D_KEY:
		{
			//Handle right movement
			Vec3 worldMovementDirection = m_mainCamera->m_cameraModel.GetIVector();
			float speed = 0.1f; 
			worldMovementDirection *= (speed); 

			m_camPosition += worldMovementDirection; 
		}
		break;
		case F4_KEY:
		{
			//Set volume back to 1
			//g_audio->SetSoundPlaybackVolume(m_testPlayback, 1.0f);

			//Unsubscribe test event
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
		}		case F7_KEY:
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

	delete m_devConsoleCamera;
	m_devConsoleCamera = nullptr;

	delete m_cube;
	m_cube = nullptr;

	delete m_sphere;
	m_sphere = nullptr;

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
	m_devConsoleCamera->SetColorTarget(colorTargetView);

	// Move the camera to where it is in the scene
	// (right now, no rotation (looking forward), set 10 back (so looking at 0,0,0)
	Matrix44 camTransform = Matrix44::MakeFromEuler( m_mainCamera->GetEuler(), m_rotationOrder ); 
	camTransform = Matrix44::SetTranslation3D(m_camPosition, camTransform);
	m_mainCamera->SetModelMatrix(camTransform);

	m_mainCamera->UpdateUniformBuffer(g_renderContext);

	g_renderContext->BeginCamera(*m_mainCamera); 
	g_renderContext->ClearColorTargets(Rgba::BLACK);

	//Bind the shader we are using (This case it's the default shader we made in Shaders folder)
	g_renderContext->BindShader( m_shader );
	//Bind the Texture to be used
	g_renderContext->BindTextureViewWithSampler( 0U, m_textureTest); 

	std::vector<Vertex_PCU> triangleVerts;
	
	triangleVerts.push_back(Vertex_PCU(Vec3(-1.f, -1.f, 0.f), Rgba::RED, Vec2::ZERO));
	triangleVerts.push_back(Vertex_PCU(Vec3(1.f, -1.f, 0.f), Rgba::GREEN, Vec2::ZERO));
	triangleVerts.push_back(Vertex_PCU(Vec3(-1.f, 1.f, 0.f), Rgba::BLUE, Vec2::ZERO));

	g_renderContext->SetModelMatrix(Matrix44::IDENTITY);
	g_renderContext->DrawVertexArray(triangleVerts);

	//Render the cube
	g_renderContext->BindTextureViewWithSampler(0U, m_boxTexturePath);  
	g_renderContext->SetModelMatrix(m_cubeTransform);
	g_renderContext->DrawMesh( m_cube ); 

	//Render the sphere
	g_renderContext->BindTextureViewWithSampler(0U, m_sphereTexturePath); 
	g_renderContext->SetModelMatrix( m_sphereTransform ); 
	g_renderContext->DrawMesh( m_sphere ); 

	g_renderContext->EndCamera();

	if(!m_consoleDebugOnce)
	{
		EventArgs* args = new EventArgs("TestString", "This is a test");
		g_devConsole->Command_Test(*args);
		g_devConsole->ExecuteCommandLine("Exec Health=25");
		g_devConsole->ExecuteCommandLine("Exec Health=85 Armor=100");
	}

	DebugRenderToCamera();
	
	if(g_devConsole->IsOpen())
	{	
		g_devConsole->Render(*g_renderContext, *m_devConsoleCamera, DEVCONSOLE_LINE_HEIGHT);
	}

	
}

void Game::DebugRenderToScreen() const
{
	/*
	Camera& debugCamera3D = *m_mainCamera;
	debugCamera3D.m_colorTargetView = g_renderContext->GetFrameColorTarget();
	g_renderContext->BeginCamera(debugCamera3D);

	//-----------------------------------------------------------------------------
	// 3D DEBUG RENDERS
	//-----------------------------------------------------------------------------
	//Setup Debug Options
	DebugRenderOptionsT options3D;
	options3D.space = DEBUG_RENDER_WORLD;
	options3D.beginColor = Rgba::GREEN;

	//Make a 3D point in the world
	g_debugRenderer->DebugRenderPoint(*m_mainCamera, options3D, Vec3(-10.0f, 0.0f, 0.0f), m_textureTest, 0.2f);

	g_renderContext->EndCamera();
	//-----------------------------------------------------------------------------

	Camera& debugCamera = g_debugRenderer->Get2DCamera();
	debugCamera.m_colorTargetView = g_renderContext->GetFrameColorTarget();
	g_renderContext->BeginCamera(debugCamera);

	//-----------------------------------------------------------------------------
	// 2D DEBUG RENDERS
	//-----------------------------------------------------------------------------

	//Setup Debug Options
	DebugRenderOptionsT options;
	options.mode = DEBUG_RENDER_ALWAYS;
	options.beginColor = Rgba::RED;

	//Make 2D Point on screen
	g_debugRenderer->DebugRenderPoint2D(options, Vec2(10.f, 10.f), 5.0f);
	//Make 2D Point at screen center
	options.beginColor = Rgba::WHITE;
	g_debugRenderer->DebugRenderPoint2D(options, Vec2(0.f, 0.f), 0.f);

	ColorTargetView* ctv = g_renderContext->GetFrameColorTarget();
	options.beginColor = Rgba::YELLOW;
	//Draw a line in 2D screen space
	g_debugRenderer->DebugRenderLine2D(options, Vec2(ctv->m_width * -0.5f, ctv->m_height * -0.5f), Vec2(-150.f, -150.f), 2.f);

	//Draw a quad in 2D screen space
	options.beginColor = Rgba::GREEN;
	g_debugRenderer->DebugRenderQuad2D(options, AABB2(Vec2(-150.f, -150.f), Vec2(-100.f, -100.f)));

	g_renderContext->EndCamera();
	*/

	
	Camera& debugCamera = g_debugRenderer->Get2DCamera();
	debugCamera.m_colorTargetView = g_renderContext->GetFrameColorTarget();
	g_renderContext->BeginCamera(debugCamera);
	
	g_debugRenderer->DebugRenderToScreen();

	g_renderContext->EndCamera();
	
}

void Game::DebugRenderToCamera() const
{
	Camera& debugCamera3D = *m_mainCamera;
	debugCamera3D.m_colorTargetView = g_renderContext->GetFrameColorTarget();

	g_renderContext->BeginCamera(debugCamera3D);
	
	g_debugRenderer->Setup3DCamera(&debugCamera3D);
	g_debugRenderer->DebugRenderToCamera();

	g_renderContext->EndCamera();
}

void Game::PostRender()
{
	//Debug bools
	m_consoleDebugOnce = true;

	if(!m_isDebugSetup)
	{
		SetStartupDebugRenderObjects();
		m_isDebugSetup = true;
	}

	//All screen Debug information
	DebugRenderToScreen();
}

void Game::Update( float deltaTime )
{
	UpdateMouseInputs(deltaTime);

	if(g_devConsole->GetFrameCount() > 1 && !m_devConsoleSetup)
	{
		m_devConsoleCamera->SetOrthoView(Vec2(-WORLD_WIDTH * 0.5f * SCREEN_ASPECT, -WORLD_HEIGHT * 0.5f), Vec2(WORLD_WIDTH * 0.5f * SCREEN_ASPECT, WORLD_HEIGHT * 0.5f));
		m_devConsoleSetup = true;
	}

	//UpdateCamera(deltaTime);
	g_renderContext->m_frameCount++;

	CheckXboxInputs();
	m_animTime += deltaTime;
	
	//Update the camera's transform
	Matrix44 camTransform = Matrix44::MakeFromEuler( m_mainCamera->GetEuler(), m_rotationOrder ); 
	camTransform = Matrix44::SetTranslation3D(m_camPosition, camTransform);
	m_mainCamera->SetModelMatrix(camTransform);
	
	float currentTime = static_cast<float>(GetCurrentTimeSeconds());

	// Set the cube to rotate around y (which is up currently),
	// and move the object to the left by 5 units (-x)
	m_cubeTransform = Matrix44::MakeFromEuler( Vec3(0.0f, 3.0f * currentTime, 0.0f), m_rotationOrder ); 
	m_cubeTransform = Matrix44::SetTranslation3D( Vec3(-5.0f, 0.0f, 0.0f), m_cubeTransform);

	m_sphereTransform = Matrix44::MakeFromEuler( Vec3(0.0f, -2.0f * currentTime, 0.0f) ); 
	m_sphereTransform = Matrix44::SetTranslation3D( Vec3(5.0f, 0.0f, 0.0f), m_sphereTransform);

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

void Game::CreateInitialMeshes()
{

	//Meshes for A4
	CPUMesh mesh;
	/*
	CPUMeshAddQuad(&mesh, AABB2(Vec2(-0.5f, -0.5f), Vec2(0.5f, 0.5f)));
	m_quad = new GPUMesh(g_renderContext);
	m_quad->CreateFromCPUMesh(&mesh, GPU_MEMORY_USAGE_STATIC);
	*/

	// create a cube (centered at zero, with sides 2 length)
	CPUMeshAddCube( &mesh, AABB3( Vec3(-0.5f, -0.5f, -0.5f), Vec3(0.5f, 0.5f, 0.5f)) ); 
	m_cube = new GPUMesh( g_renderContext ); 
	m_cube->CreateFromCPUMesh( &mesh, GPU_MEMORY_USAGE_STATIC ); // we won't be updated this; 


																 // create a sphere, cenetered at zero, with 
	mesh.Clear();
	CPUMeshAddUVSphere( &mesh, Vec3::ZERO, 1.0f );  
	m_sphere = new GPUMesh( g_renderContext ); 
	m_sphere->CreateFromCPUMesh( &mesh, GPU_MEMORY_USAGE_STATIC );
}

void Game::LoadGameTextures()
{
	//Get the test texture
	m_textureTest = g_renderContext->GetOrCreateTextureViewFromFile(m_testImagePath);
	m_boxTexture = g_renderContext->GetOrCreateTextureViewFromFile(m_boxTexturePath);
	m_sphereTexture = g_renderContext->GetOrCreateTextureViewFromFile(m_sphereTexturePath);
}

void Game::GetandSetShaders()
{
	//Get the Shader
	m_shader = g_renderContext->CreateOrGetShaderFromFile(m_xmlShaderPath);
	m_shader->SetDepth(eCompareOp::COMPARE_LEQUAL, true);
}

void Game::UpdateMouseInputs(float deltaTime)
{
	//Get pitch and yaw from mouse
	IntVec2 mouseRelativePos = g_windowContext->GetClientMouseRelativeMovement();
	Vec2 mouse = Vec2((float)mouseRelativePos.x, (float)mouseRelativePos.y);

	// we usually want to scale the pixels so we can think of it
	// as a rotational velocity;  Work with these numbers until 
	// it feels good to you; 
	Vec2 scalingFactor = Vec2( 10.f, 10.f ); 
	Vec2 turnSpeed = mouse * scalingFactor; 

	// y mouse movement would corresond to rotation around right (x for us)
	// and x mouse movement corresponds with movement around up (y for us)
	Vec3 camEuler = m_mainCamera->GetEuler();
	camEuler -= deltaTime * Vec3( turnSpeed.y, turnSpeed.x, 0.0f ); 
	m_mainCamera->SetEuler(camEuler);

	// Let's fix our "pitch", or rotation around right to be limited to -85 to 85 degrees (no going upside down)
	camEuler.x = Clamp( camEuler.x, -85.0f, 85.0f );

	// Next, let's keep the turning as between 0 and 360 (some people prefer -180.0f to 180.0f)
	// either way, we just want to keep it a single revolution
	// Note: modf does not correctly mod negative numbers (it'll ignore the sign and mod them as if 
	// they were positive), so I write a special mod function to take this into account;  
	//m_camEuler.y = Modf( m_camEuler.y, 360.0f ); 

	// Awesome, I now have my euler, let's construct a matrix for it;
	// this gives us our current camera's orientation.  we will 
	// use this to translate our local movement to a world movement 
	Matrix44 camMatrix = Matrix44::MakeFromEuler( camEuler ); 

	//Test implementation
	//m_camEuler.y -= static_cast<float>(mouseRelativePos.x);
	//m_camEuler.x -= static_cast<float>(mouseRelativePos.y);


}
