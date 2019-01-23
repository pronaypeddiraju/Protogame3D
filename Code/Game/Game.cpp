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

//Create Camera and set to null 
Camera *g_mainCamera = nullptr; 
float g_shakeAmount = 0.0f;
//ScreenColor
Rgba* g_clearScreenColor = nullptr;

RandomNumberGenerator* g_randomNumGen;
extern RenderContext* g_renderContext;
extern AudioSystem* g_audio;
bool g_debugMode = false;

Game::Game()
{
	m_isGameAlive = true;
	m_testAudioID = g_audio->CreateOrGetSound("Data/Audio/UproarLilWayne.mp3");

	m_textureTest = g_renderContext->CreateOrGetTextureFromFile("Data/Images/Test_StbiFlippedAndOpenGL.png");
	m_testImage = new Image("Data/Images/Test_StbiFlippedAndOpenGL.png");
	m_spriteTest = g_renderContext->CreateOrGetTextureFromFile("Data/Images/Test_SpriteSheet8x2.png");
	m_squirrelFont = g_renderContext->CreateOrGetBitmapFontFromFile("SquirrelFixedFont");

	m_explosionTexture = g_renderContext->CreateOrGetTextureFromFile("Data/Images/Explosion_5x5.png");
	SpriteSheet* explosionSheet = new SpriteSheet(m_explosionTexture, IntVec2(5,5));
	m_explosionPingPong = new SpriteAnimDefenition(*explosionSheet, 0, 24, 1.f, SPRITE_ANIM_PLAYBACK_PINGPONG);
	m_explosionOnce = new SpriteAnimDefenition(*explosionSheet, 0, 24, 2.0f, SPRITE_ANIM_PLAYBACK_ONCE);
	m_explosionLoop = new SpriteAnimDefenition(*explosionSheet, 0, 24, 0.5f, SPRITE_ANIM_PLAYBACK_LOOP);

	g_devConsole->SetBitmapFont(*m_squirrelFont);
}

Game::~Game()
{
	m_isGameAlive = false;
}

void Game::StartUp()
{
	//Create the Camera and setOrthoView
	g_clearScreenColor = new Rgba(0.f, 0.f, 0.5f, 1.f);
	
	g_devConsole->PrintString(Rgba::BLUE, "this is a test string");
	g_devConsole->PrintString(Rgba::RED, "this is also a test string");
	g_devConsole->PrintString(Rgba::GREEN, "damn this dev console lit!");
	g_devConsole->PrintString(Rgba::WHITE, "Last thing I printed");

	g_eventSystem->SubscribeEventCallBackFn("TestEvent", TestEvent);
}

STATIC bool Game::TestEvent(EventArgs& args)
{
	UNUSED(args);
	g_devConsole->PrintString(Rgba::YELLOW, "This a test event called from Game.cpp");
	return true;
}

void Game::HandleKeyPressed(unsigned char keyCode)
{
	switch( keyCode )
	{
	case UP_ARROW:
	case RIGHT_ARROW:
	case LEFT_ARROW:
	case SPACE_KEY:
	case A_KEY:
	case N_KEY:
	break;
	case F1_KEY:
	//m_testPlayback = g_audio->PlaySound(m_testAudioID);
	break;
	case F2_KEY:
	//Set playback speed to 0
	//g_audio->SetSoundPlaybackSpeed(m_testPlayback, 0.0f);
	break;
	case F3_KEY:
	//Set playback speed back to 1
	//g_audio->SetSoundPlaybackSpeed(m_testPlayback, 1.0f);
	break;
	case F4_KEY:
	//Set volume back to 1
	//g_audio->SetSoundPlaybackVolume(m_testPlayback, 1.0f);
	//Unsub test
	g_eventSystem->UnsubscribeEventCallBackFn("TestEvent", TestEvent);
	break;
	case F5_KEY:
	//Set volume to 0
	//g_audio->SetSoundPlaybackVolume(m_testPlayback, 0.0f);
	//Help Debug
	g_eventSystem->FireEvent("Help");
	break;
	case F6_KEY:
	//Fire event
	g_eventSystem->FireEvent("TestEvent");
	break;
	case F7_KEY:
	//Quit Debug
	g_eventSystem->FireEvent("Quit");
	break;
	default:
	break;
	}
}

//Function that handles debug mode enabled
void Game::DebugEnabled()
{
	g_debugMode = !g_debugMode;
}


void Game::HandleKeyReleased(unsigned char keyCode)
{
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

void Game::Render() const
{
	g_renderContext->ClearScreen(*g_clearScreenColor);

	g_renderContext->BeginCamera(*g_mainCamera);

	g_renderContext->BindTexture(nullptr);
	DebugRender();

	if(!m_consoleDebugOnce)
	{
		EventArgs* args = new EventArgs("TestString", "This is a test");
		g_devConsole->Command_Test(*args);
		g_devConsole->ExecuteCommandLine("Exec Health=25");
		g_devConsole->ExecuteCommandLine("Exec Health=85 Armor=100");
	}

	g_devConsole->Render(*g_renderContext, *g_mainCamera, DEVCONSOLE_LINE_HEIGHT);
	g_renderContext->BindTexture(nullptr);

	g_renderContext->EndCamera(*g_mainCamera);

}

void Game::PostRender()
{
	//Debug bools
	m_consoleDebugOnce = true;
}

void Game::DebugRenderTextures() const
{
	//Test to see if textures render
	g_renderContext->BindTexture(m_textureTest);
	g_renderContext->SetBlendMode(BLEND_MODE_ALPHA);
	std::vector<Vertex_PCU> boxVerts;
	boxVerts.push_back(Vertex_PCU(Vec3(0.f,0.f,0.f), Rgba(1.f,1.f,1.f,1.f), Vec2(0.f,0.f)));
	boxVerts.push_back(Vertex_PCU(Vec3(100.f,0.f,0.f), Rgba(1.f,1.f,1.f,1.f), Vec2(1.f,0.f)));
	boxVerts.push_back(Vertex_PCU(Vec3(100.f,100.f,0.f), Rgba(1.f,1.f,1.f,1.f), Vec2(1.f,1.f)));

	boxVerts.push_back(Vertex_PCU(Vec3(0.f,0.f,0.f), Rgba(1.f,1.f,1.f,1.f), Vec2(0.f,0.f)));
	boxVerts.push_back(Vertex_PCU(Vec3(0.f,100.f,0.f), Rgba(1.f,1.f,1.f,1.f), Vec2(0.f,1.f)));
	boxVerts.push_back(Vertex_PCU(Vec3(100.f,100.f,0.f), Rgba(1.f,1.f,1.f,1.f), Vec2(1.f,1.f)));
	//addVertsForAABB2(boxVerts, box1Bounds, Rgba(1.f,1.f,0.f));
	g_renderContext->DrawVertexArray(6, &boxVerts[0]);	//boxVerts.size()
}

void Game::DebugRenderSprites() const
{
	g_renderContext->BindTexture(m_spriteTest);
	//Test to see if sprites render
	SpriteSheet testSheet = SpriteSheet(m_spriteTest, IntVec2(8,2));
	Vec2 uvAtBottomLeft = Vec2::ZERO;
	Vec2 uvAtTopRight = Vec2::ONE;
	//Get the sprite defenition here
	SpriteDefenition sd = testSheet.GetSpriteDef(0);
	sd.GetUVs(uvAtBottomLeft, uvAtTopRight);
	AABB2 box = AABB2(Vec2(10.f,10.f), Vec2(50.f,50.f));
	std::vector<Vertex_PCU> spriteVerts;
	AddVertsForAABB2D(spriteVerts, box, Rgba(1.f, 1.f, 1.f, 1.f), uvAtBottomLeft, uvAtTopRight);
	g_renderContext->DrawVertexArray(spriteVerts);
}

void Game::DebugRenderSpriteAnims() const
{
	//Sprite animation test
	
	Vec2 uvAtBottomLeft = Vec2::ZERO;
	Vec2 uvAtTopRight = Vec2::ONE;

	//Ping Pong Test
	AABB2 animBoxPingPong = AABB2(Vec2(55.0f,5.0f), Vec2(75.f, 25.f));
	std::vector<Vertex_PCU> animVertsPingPong;
	SpriteDefenition sdAnimPingPong = m_explosionPingPong->GetSpriteDefAtTime(m_animTime);
	sdAnimPingPong.GetUVs(uvAtBottomLeft, uvAtTopRight);
	AddVertsForAABB2D(animVertsPingPong, animBoxPingPong, Rgba::WHITE, uvAtBottomLeft, uvAtTopRight);

	//Once Test
	AABB2 animBoxOnce = AABB2(Vec2(75.0f,5.0f), Vec2(95.f, 25.f));
	std::vector<Vertex_PCU> animVertsOnce;
	SpriteDefenition sdAnimOnce = m_explosionOnce->GetSpriteDefAtTime(m_animTime);
	sdAnimOnce.GetUVs(uvAtBottomLeft, uvAtTopRight);
	AddVertsForAABB2D(animVertsOnce, animBoxOnce, Rgba::GREEN, uvAtBottomLeft, uvAtTopRight);

	//Loop test
	AABB2 animBoxLoop = AABB2(Vec2(95.0f,5.0f), Vec2(115.f, 25.f));
	std::vector<Vertex_PCU> animVertsLoop;
	SpriteDefenition sdAnimLoop = m_explosionLoop->GetSpriteDefAtTime(m_animTime);
	sdAnimLoop.GetUVs(uvAtBottomLeft, uvAtTopRight);
	AddVertsForAABB2D(animVertsLoop, animBoxLoop, Rgba::RED, uvAtBottomLeft, uvAtTopRight);

	g_renderContext->BindTexture(m_explosionTexture);
	g_renderContext->SetBlendMode(BLEND_MODE_ADDITIVE);
	g_renderContext->DrawVertexArray(animVertsPingPong);
	g_renderContext->DrawVertexArray(animVertsOnce);
	g_renderContext->DrawVertexArray(animVertsLoop);

}

void Game::DebugRenderTextAlignment() const
{
	g_renderContext->BindTexture(nullptr);
	g_renderContext->SetBlendMode(BLEND_MODE_ALPHA);

	//text Alignment Tests!
	AABB2 textAlignBoundaryLeft = AABB2(Vec2(118.f, 8.f), Vec2(120.f, 42.f));
	std::vector<Vertex_PCU> TextBoundaryLeftVerts;
	AddVertsForAABB2D(TextBoundaryLeftVerts, textAlignBoundaryLeft, Rgba::WHITE);
	AABB2 textAlignBoundaryRight = AABB2(Vec2(180.f, 8.f), Vec2(182.f, 42.f));
	std::vector<Vertex_PCU> TextBoundaryRightVerts;
	AddVertsForAABB2D(TextBoundaryRightVerts, textAlignBoundaryRight, Rgba::WHITE);
	AABB2 textAlignBoundaryTop = AABB2(Vec2(118.f, 40.f), Vec2(182.f, 42.f));
	std::vector<Vertex_PCU> TextBoundaryTopVerts;
	AddVertsForAABB2D(TextBoundaryTopVerts, textAlignBoundaryTop, Rgba::WHITE);
	AABB2 textAlignBoundaryBottom = AABB2(Vec2(118.f, 8.f), Vec2(182.f, 10.f));
	std::vector<Vertex_PCU> TextBoundaryBottomVerts;
	AddVertsForAABB2D(TextBoundaryBottomVerts, textAlignBoundaryBottom, Rgba::WHITE);

	//Box to align
	AABB2 boxToAlign = AABB2(Vec2(0.f,0.f), Vec2(4.f, 4.f));
	std::vector<Vertex_PCU> boxToAlignVerts;
	boxToAlign.AlignWithinBox(AABB2(Vec2(120.f,10.f), Vec2(180.f, 40.f)), Vec2::ALIGN_TOP_CENTERED);
	AddVertsForAABB2D(boxToAlignVerts, boxToAlign, Rgba::BLUE);

	g_renderContext->DrawVertexArray(TextBoundaryLeftVerts);
	g_renderContext->DrawVertexArray(TextBoundaryRightVerts);
	g_renderContext->DrawVertexArray(TextBoundaryTopVerts);
	g_renderContext->DrawVertexArray(TextBoundaryBottomVerts);
	g_renderContext->DrawVertexArray(boxToAlignVerts);

	//Text alignment test
	g_renderContext->BindTexture(m_squirrelFont->GetTexture());
	std::vector<Vertex_PCU> fontVerts;

	g_renderContext->BindTexture(nullptr);
	boxToAlign = AABB2(Vec2(0.f,0.f), Vec2(4.f, 4.f));
	boxToAlignVerts.clear();
	boxToAlign.AlignWithinBox(AABB2(Vec2(120.f,10.f), Vec2(180.f, 40.f)), Vec2::ALIGN_RIGHT_TOP);
	AddVertsForAABB2D(boxToAlignVerts, boxToAlign, Rgba::BLUE);
	g_renderContext->DrawVertexArray(boxToAlignVerts);

	g_renderContext->BindTexture(m_squirrelFont->GetTexture());
	m_squirrelFont->AddVertsForTextInBox2D(fontVerts, AABB2(Vec2(150.f, 38.f), Vec2(180.f, 40.f)), 5.f, "Align Text", Rgba::RED, 1.f, Vec2::ALIGN_RIGHT_TOP, TEXT_BOX_MODE_SHRINK);
	g_renderContext->DrawVertexArray(fontVerts);
	fontVerts.clear();

	m_squirrelFont->AddVertsForText2D(fontVerts, Vec2(120.f,20.f), 5.f, "Hello World", Rgba::BLACK);
	g_renderContext->DrawVertexArray(fontVerts);
	fontVerts.clear();

	g_renderContext->BindTexture(nullptr);
	boxToAlign = AABB2(Vec2(0.f,0.f), Vec2(4.f, 4.f));
	boxToAlignVerts.clear();
	boxToAlign.AlignWithinBox(AABB2(Vec2(120.f,10.f), Vec2(180.f, 40.f)), Vec2::ALIGN_LEFT_TOP);
	AddVertsForAABB2D(boxToAlignVerts, boxToAlign, Rgba::BLUE);
	g_renderContext->DrawVertexArray(boxToAlignVerts);

	g_renderContext->BindTexture(m_squirrelFont->GetTexture());
	//Get this checked out
	m_squirrelFont->AddVertsForTextInBox2D(fontVerts, AABB2(Vec2(120.f, 10.f), Vec2(150.f, 40.f)), 5.f, "Align Text", Rgba::RED, 1.f, Vec2::ALIGN_LEFT_TOP, TEXT_BOX_MODE_SHRINK);
	g_renderContext->DrawVertexArray(fontVerts);
	fontVerts.clear();

	g_renderContext->BindTexture(nullptr);
	boxToAlign = AABB2(Vec2(0.f,0.f), Vec2(4.f, 4.f));
	boxToAlignVerts.clear();
	boxToAlign.AlignWithinBox(AABB2(Vec2(120.f,10.f), Vec2(180.f, 40.f)), Vec2::ALIGN_LEFT_BOTTOM);
	AddVertsForAABB2D(boxToAlignVerts, boxToAlign, Rgba::BLUE);
	g_renderContext->DrawVertexArray(boxToAlignVerts);

	g_renderContext->BindTexture(m_squirrelFont->GetTexture());
	m_squirrelFont->AddVertsForTextInBox2D(fontVerts, AABB2(Vec2(120.f, 10.f), Vec2(150.f, 40.f)), 5.f, "Align Text", Rgba::RED, 1.f, Vec2::ALIGN_LEFT_BOTTOM, TEXT_BOX_MODE_SHRINK);
	g_renderContext->DrawVertexArray(fontVerts);
	fontVerts.clear();

	g_renderContext->BindTexture(nullptr);
	boxToAlign = AABB2(Vec2(0.f,0.f), Vec2(4.f, 4.f));
	boxToAlignVerts.clear();
	boxToAlign.AlignWithinBox(AABB2(Vec2(120.f,10.f), Vec2(180.f, 40.f)), Vec2::ALIGN_RIGHT_BOTTOM);
	AddVertsForAABB2D(boxToAlignVerts, boxToAlign, Rgba::BLUE);
	g_renderContext->DrawVertexArray(boxToAlignVerts);

	g_renderContext->BindTexture(m_squirrelFont->GetTexture());
	m_squirrelFont->AddVertsForTextInBox2D(fontVerts, AABB2(Vec2(150.f, 10.f), Vec2(180.f, 40.f)), 5.f, "Align Text", Rgba::RED, 1.f, Vec2::ALIGN_RIGHT_BOTTOM, TEXT_BOX_MODE_SHRINK);
	g_renderContext->DrawVertexArray(fontVerts);
	fontVerts.clear();
}

void Game::DebugRender() const
{
 	DebugRenderTextures();
	g_renderContext->BindTexture(nullptr);
 	DebugRenderSprites();
	g_renderContext->BindTexture(nullptr);
 	DebugRenderSpriteAnims();
	g_renderContext->BindTexture(nullptr);
 	DebugRenderTextAlignment();
	g_renderContext->BindTexture(nullptr);
}

//Calls the UpdateShip function in playerShip
void Game::Update( float deltaTime )
{
	UpdateCamera(deltaTime);

	CheckXboxInputs();
	m_animTime += deltaTime;
	CheckCollisions();

	ClearGarbageEntities();	
}

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



void Game::ClearGarbageEntities()
{

	
}

void Game::CheckXboxInputs()
{
	XboxController playerController = g_inputSystem->GetXboxController(0);
}

void Game::CheckCollisions()
{	
	
}

bool Game::IsAlive()
{
	//Check if alive
	return m_isGameAlive;
}
