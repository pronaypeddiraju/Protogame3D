#pragma once
#include "Engine/Math/Vertex_PCU.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"

class Texture;
class BitmapFont;
class SpriteAnimDefenition;
class Image;

class Game
{
public:
	Game();
	~Game();
	
	static bool TestEvent(EventArgs& args);
	
	void StartUp();
	void HandleKeyPressed( unsigned char keyCode );
	void DebugEnabled();

	void HandleKeyReleased( unsigned char keyCode );
	void Render() const;
	void PostRender();
	void DebugRenderTextures() const;
	void DebugRenderSprites() const;
	void DebugRenderSpriteAnims() const;
	void DebugRenderTextAlignment() const;
	void DebugRender() const;
	void Update( float deltaTime );
	void UpdateCamera( float deltaTime );
	void ClearGarbageEntities();
	void CheckXboxInputs();
	void CheckCollisions();

	bool IsAlive();
private:
	bool m_isGameAlive = false;
	bool m_consoleDebugOnce = false;

public:
	SoundID m_testAudioID = NULL;
	SoundPlaybackID m_testPlayback  = NULL;
	Texture* m_textureTest = nullptr;
	Texture* m_spriteTest = nullptr;
	BitmapFont* m_squirrelFont = nullptr;
	SpriteAnimDefenition* m_explosionPingPong = nullptr;
	SpriteAnimDefenition* m_explosionOnce = nullptr;
	SpriteAnimDefenition* m_explosionLoop = nullptr;
	Texture* m_explosionTexture = nullptr;
	Image* m_testImage = nullptr;
	float m_animTime = 0.f;
};
