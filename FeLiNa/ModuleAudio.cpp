#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleGui.h"

#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

ModuleAudio::ModuleAudio(Application* app, bool start_enabled) : Module(app, start_enabled), music(NULL)
{
	name = "Audio";
}

// Destructor
ModuleAudio::~ModuleAudio()
{}


update_status ModuleAudio::PreUpdate(float dt) {
	update_status update_return = UPDATE_CONTINUE;
	module_timer.Start();
	return update_return;
}

update_status ModuleAudio::PostUpdate(float dt) {

	update_status update_return = UPDATE_CONTINUE;

	last_update_ms = module_timer.ReadMs();
	module_timer.Start();

	return update_return;

}

// Called before render is available
bool ModuleAudio::Init()
{
	LOG_GLOBAL ("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	int i = 0;
	//App->gui->Print_Log((char*)("SDL_INIT_AUDIO could not initialize! SDL_Error: %i\n", &i));
	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG_GLOBAL("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	// load support for the OGG format
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		//App->gui->Print_Log("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		ret = false;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		//App->gui->Print_Log("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		ret = false;
	}

	return true;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	LOG_GLOBAL("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music != NULL)
	{
		Mix_FreeMusic(music);
	}
	
	for (std::list<Mix_Chunk*>::iterator it = fx.begin(); it != fx.end(); ++it)
	{
		Mix_FreeChunk(*it);
	}

	fx.clear();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	return true;
}

// Play a music file
bool ModuleAudio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;
	
	if(music != NULL)
	{
		if(fade_time > 0.0f)
		{
			Mix_FadeOutMusic((int) (fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if(music == NULL)
	{
		//App->gui->Print_Log("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fade_time > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fade_time * 1000.0f)) < 0)
			{
				//App->gui->Print_Log("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music, -1) < 0)
			{
				//App->gui->Print_Log("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	//App->gui->Print_Log("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int ModuleAudio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if(chunk == NULL)
	{
		//App->gui->Print_Log("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
bool ModuleAudio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	std::list<Mix_Chunk*>::const_iterator chunk = fx.begin();
	std::advance(chunk, id - 1);

	if(chunk != fx.end())
	{
		Mix_PlayChannel(-1, *chunk, repeat);
		ret = true;
	}

	return ret;
}