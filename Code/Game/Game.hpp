#pragma once
#include "Engine/Math/Vertex_PCU.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"

class Texture;
class BitmapFont;
class SpriteAnimDefenition;
class TextureView;
class Image;
class Shader;

class Game
{
public:
	Game();
	~Game();
	
	static bool TestEvent(EventArgs& args);
	
	void								StartUp();
	
	void								HandleKeyPressed( unsigned char keyCode );
	void								HandleKeyReleased( unsigned char keyCode );
	void								HandleCharacter( unsigned char charCode );

	void								DebugEnabled();
	void								Shutdown();

	void								Render() const;
	void								PostRender();
	void								Update( float deltaTime );
	void								UpdateCamera( float deltaTime );
	void								ClearGarbageEntities();
	void								CheckXboxInputs();
	void								CheckCollisions();

	bool								IsAlive();
private:
	bool								m_isGameAlive = false;
	bool								m_consoleDebugOnce = false;

public:
	SoundID								m_testAudioID = NULL;
	
	TextureView*						m_textureTest = nullptr;
	BitmapFont*							m_squirrelFont = nullptr;
	Image*								m_testImage = nullptr;
	float								m_animTime = 0.f;

	//D3D11 stuff
	Shader*								m_shader = nullptr;
	std::string							m_defaultShaderPath = "default_unlit.hlsl";
	std::string							m_testImagePath = "Test_StbiFlippedAndOpenGL.png";
};