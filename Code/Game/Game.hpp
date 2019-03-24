//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine Systems
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/Matrix44.hpp"
//Game Systems
#include "Game/GameCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class Texture;
class BitmapFont;
class SpriteAnimDefenition;
class TextureView;
class Image;
class Shader;
class GPUMesh;

struct Camera;

//------------------------------------------------------------------------------------------------------------------------------
class Game
{
public:
	Game();
	~Game();
	
	static bool TestEvent(EventArgs& args);
	
	void								StartUp();
	
	void								SetupMouseData();
	void								SetupCameras();
	void								GetandSetShaders();
	void								LoadGameTextures();
	void								CreateInitialMeshes();
	void								SetStartupDebugRenderObjects();


	void								HandleKeyPressed( unsigned char keyCode );
	void								HandleKeyReleased( unsigned char keyCode );
	void								HandleCharacter( unsigned char charCode );

	void								DebugEnabled();
	void								Shutdown();

	void								Render() const;
	void								DebugRenderToScreen() const;
	void								DebugRenderToCamera() const;
	void								PostRender();
	void								Update( float deltaTime );
	void								UpdateMouseInputs(float deltaTime);
	void								UpdateCamera( float deltaTime );
	void								ClearGarbageEntities();
	void								CheckXboxInputs();
	void								CheckCollisions();

	bool								IsAlive();
private:
	bool								m_isGameAlive = false;
	bool								m_consoleDebugOnce = false;
	bool								m_devConsoleSetup = false;
	bool								m_isDebugSetup = false;

public:
	SoundID								m_testAudioID = NULL;
	
	TextureView*						m_textureTest = nullptr;
	TextureView*						m_boxTexture = nullptr;
	TextureView*						m_sphereTexture = nullptr;
	BitmapFont*							m_squirrelFont = nullptr;
	Image*								m_testImage = nullptr;
	float								m_animTime = 0.f;

	//D3D11 stuff
	Shader*								m_shader = nullptr;
	std::string							m_defaultShaderPath = "default_unlit.00.hlsl";
	std::string							m_testImagePath = "Test_StbiFlippedAndOpenGL.png";
	std::string							m_boxTexturePath = "woodcrate.jpg";
	std::string							m_sphereTexturePath = "2k_earth_daymap.jpg";
	std::string							m_xmlShaderPath = "default_unlit.xml";

	Camera*								m_mainCamera = nullptr;
	Camera*								m_devConsoleCamera = nullptr;
	Rgba*								m_clearScreenColor = nullptr;
	
	float								m_camFOVDegrees = 90.f; //Desired Camera Field of View
	eRotationOrder						m_rotationOrder = ROTATION_ORDER_DEFAULT;
	Vec3								m_camPosition = Vec3::ZERO;

	float								m_devConsoleScreenWidth = 0.f;
	float								m_devConsoleScreenHeight = 0.f;

	//FOR ASSIGNMENT 4:
	// Define the shapes, and how are they positionedin the world; 
	GPUMesh*							m_cube; 
	Matrix44							m_cubeTransform; // cube's model matrix

	GPUMesh*							m_sphere;
	Matrix44							m_sphereTransform;   // sphere's model matrix
};