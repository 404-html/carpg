#pragma once

//-----------------------------------------------------------------------------
#include "Base.h"
#include "Animesh.h"
#include "Timer.h"
#include "Resource.h"
#include "KeyStates.h"
#include "Physics.h"
#include "ResourceManager.h"

//-----------------------------------------------------------------------------
#define DISPLAY_FORMAT D3DFMT_X8R8G8B8
#define BACKBUFFER_FORMAT D3DFMT_A8R8G8B8
#define ZBUFFER_FORMAT D3DFMT_D24S8

//-----------------------------------------------------------------------------
typedef fastdelegate::FastDelegate1<int> KeyDownCallback;

//-----------------------------------------------------------------------------
struct CompileShaderParams
{
	cstring name;
	cstring cache_name;
	string* input;
	FILETIME file_time;
	D3DXMACRO* macros;
	ID3DXEffectPool* pool;
};

//-----------------------------------------------------------------------------
// Silnik
struct Engine
{
	Engine();

	void ChangeMode();
	bool ChangeMode(bool fullscreen);
	bool ChangeMode(int w, int h, bool fullscreen, int hz=0);
	int ChangeMultisampling(int type, int level);
	bool CheckDisplay(int w, int h, int& hz); // dla zera zwraca najlepszy hz
	void Cleanup();
	ID3DXEffect* CompileShader(cstring name);
	ID3DXEffect* CompileShader(CompileShaderParams& params);
	void DoPseudotick();
	void DoTick(bool update_game);
	void EngineShutdown();
	void FatalError(cstring err);
	void GatherParams(D3DPRESENT_PARAMETERS& d3dpp);
	inline void GetMultisampling(int& ms, int& msq) { ms = multisampling; msq = multisampling_quality; }
	LRESULT HandleEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void InitPhysics();
	void InitRender();
	void InitSound();
	void InitWindow(cstring title);
	inline bool IsEngineShutdown() const { return engine_shutdown; }
	inline bool IsLostDevice() const { return lost_device; }
	inline bool IsMultisamplingEnabled() const { return multisampling != 0; }
	Mesh* LoadMesh(cstring filename);
	VertexData* LoadMeshVertexData(cstring filename);
	FMOD::Sound* LoadMusic(cstring filename);
	FMOD::Sound* LoadSound(cstring filename);
	TEX LoadTex(cstring filename);
	TextureResource* LoadTexResource(cstring filename);
	void LogMultisampling();
	void PlaceCursor();
	void PlayMusic(FMOD::Sound* music);
	void PlaySound2d(FMOD::Sound* sound);
	// smax jest nieużywane
	void PlaySound3d(FMOD::Sound* sound, const VEC3& pos, float smin, float smax=0.f);
	void Render(bool dont_call_present=false);
	bool Reset(bool force);
	void SetStartingMultisampling(int multisampling, int multisampling_quality);
	void SetTitle(cstring title);
	void ShowCursor(bool show);
	void ShowError(cstring msg);
	bool Start(cstring title, bool fullscreen, int w, int h);
	void StopSounds();
	void UnlockCursor();
	void UpdateMusic(float dt);
	void WindowLoop();
	
	// funkcje implementowane przez Game
	virtual void InitGame() = 0;
	virtual void OnChar(char c) = 0;
	virtual void OnCleanup() = 0;
	virtual void OnDraw() = 0;
	virtual void OnReload() = 0;
	virtual void OnReset() = 0;
	virtual void OnResize() = 0;
	virtual void OnTick(float dt) = 0;
	virtual void OnFocus(bool focus) = 0;

	// ----- ZMIENNE -----
	static Engine* _engine;
	KeyDownCallback key_callback;
	
	// okno
	HWND hwnd;
	bool active, fullscreen, replace_cursor, locked_cursor;
	VEC2 cursor_pos;
	int mouse_wheel;
	uint frames;
	float frame_time, fps;
	Timer timer;
	INT2 wnd_size, real_size, mouse_dif, unlock_point, s_wnd_size, s_wnd_pos;

	// directx
	IDirect3D9* d3d;
	IDirect3DDevice9* device;
	ID3DXSprite* sprite;
	FONT font;
	DWORD clear_color;
	int wnd_hz, used_adapter, shader_version;
	bool vsync;

	// FMOD
	FMOD::System* fmod_system;
	FMOD::ChannelGroup* group_default, *group_music;
	FMOD::Channel* current_music;
	vector<FMOD::Channel*> playing_sounds;
	vector<FMOD::Channel*> fallbacks;
	bool music_ended, disabled_sound;

	// bullet physics
	btDefaultCollisionConfiguration* phy_config;
	btCollisionDispatcher* phy_dispatcher;
	btDbvtBroadphase* phy_broadphase;
	CustomCollisionWorld* phy_world;

	ResourceManager& resMgr;

private:
	void AdjustWindowSize();
	void SelectResolution();

	bool engine_shutdown;
	bool lost_device, res_freed;
	int multisampling, multisampling_quality;
};
