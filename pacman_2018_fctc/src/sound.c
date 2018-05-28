#include "sound.h"

#include <SDL/SDL_mixer.h>

//sound filenames
//static const char *EFFECT_FILES[] = {"wakawaka.wav", "bdc"};
//static const char MUSIC_FILE[] = "Music.wav";

static float volume;
static bool muted;

static Mix_Music *music;
//static Mix_Chunk *effects[NUM_EFFECTS];


static Mix_Chunk *levelStart;
static Mix_Chunk *smallEatSound;
static Mix_Chunk *largeEatSound;
static Mix_Chunk *sirenSound;
static Mix_Chunk *fruitEatSound;
static Mix_Chunk *munch_aSound;
static Mix_Chunk *munch_bSound;
static Mix_Chunk *boosterSound;
static Mix_Chunk *bonusSound;
static Mix_Chunk *dieSound;
static Mix_Chunk *deathSound;
static Mix_Chunk *chomp1Sound;
static Mix_Chunk *chomp2Sound;
static Mix_Chunk *cutsceneSound;
static Mix_Chunk *coinSound;
static Mix_Chunk *gameoverSound;

static int levelStartChanel;


void load_sounds(void)
{
        //22050 is sound frequency
        //MIX_DEFAULT_FORMAT is the sound format
        //2 is number of channels to use, we use 2 for stereo sound
        //4096 is the sample size
        if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
        {
                printf("Error creating sound effects.\n");
                printf("Aborting.\n");
                exit(1);
        }

        //now load the sounds and effects we're using
        //music = Mix_LoadMUS(MUSIC_FILE);

        //for (int i = 0; i < NUM_EFFECTS; i++)
        //{
                //effects[i] = Mix_LoadWAV(EFFECT_FILES[i]);
        //}

        levelStart = Mix_LoadWAV("sound/pacintro.wav");

        smallEatSound = Mix_LoadWAV("sound/ghost_eat.wav");
        largeEatSound = Mix_LoadWAV("sound/large_pellet.wav");
        sirenSound = Mix_LoadWAV("sound/siren.wav");
        fruitEatSound = Mix_LoadWAV("sound/fruit.wav");
        munch_aSound = Mix_LoadWAV("sound/munch_a.wav");
        munch_bSound = Mix_LoadWAV("sound/munch_b.wav");
        boosterSound = Mix_LoadWAV("sound/booster.wav");
        bonusSound = Mix_LoadWAV("sound/bonus.wav");
        dieSound = Mix_LoadWAV("sound/die.wav");
        deathSound = Mix_LoadWAV("sound/death.wav");	
        chomp1Sound = Mix_LoadWAV("sound/chomp1.wav");
        chomp2Sound = Mix_LoadWAV("sound/chomp2.wav");
        cutsceneSound = Mix_LoadWAV("sound/cutscene.wav");
        coinSound = Mix_LoadWAV("sound/coin.wav");
        gameoverSound = Mix_LoadWAV("sound/game_over.wav");
        set_sound_volume(0.5);
        set_sound_muted(false);
}

void dispose_sounds(void)
{
	//for (int i = 0; i < NUM_EFFECTS; i++)
	//{
		//Mix_FreeChunk(effects[i]);
	//}

	Mix_FreeChunk(levelStart);

	Mix_FreeMusic(music);
}

void set_sound_volume(float vol)
{
	Mix_VolumeChunk(levelStart, vol * MIX_MAX_VOLUME);
	volume = vol;
}

float get_sound_volume(void)
{
	return volume;
}

void set_sound_muted(bool isMuted)
{
	if (isMuted)
	{
		Mix_VolumeChunk(levelStart, 0);
	}
	else
	{
		set_sound_volume(volume);
	}

	muted = isMuted;
}

bool is_sound_muted(void)
{
	return muted;
}






void play_sound(SoundEffect effectName)
{
        Mix_Chunk *chunk;
        int *channel;

        switch (effectName)
        {
                case LevelStartSound:  chunk = levelStart; channel = &levelStartChanel; break;
                case SmallSound:    chunk = smallEatSound; channel = &levelStartChanel; break;
                case LargeSound:    chunk = largeEatSound; channel = &levelStartChanel; break;
                case SirenSound:       chunk = sirenSound; channel = &levelStartChanel; break;
                case FruitSound:    chunk = fruitEatSound; channel = &levelStartChanel; break;
                case Munch_Asound:  chunk = munch_aSound;  channel = &levelStartChanel; break;
                case Munch_Bsound: chunk = munch_bSound;   channel = &levelStartChanel; break;
                case BoosterSound:  chunk = boosterSound;  channel = &levelStartChanel; break;
                case BonusSound:  chunk = bonusSound;  channel = &levelStartChanel; break;
                case DieSound:  chunk = dieSound;  channel = &levelStartChanel; break;
                case Chomp1Sound:  chunk = chomp1Sound;  channel = &levelStartChanel; break;
                case Chomp2Sound:  chunk = chomp2Sound;  channel = &levelStartChanel; break;
                case CutsceneSound:  chunk = cutsceneSound;  channel = &levelStartChanel; break;
                case CoinSound:  chunk = coinSound;  channel = &levelStartChanel; break;
                case GameoverSound:  chunk = gameoverSound;  channel = &levelStartChanel; break;

                case PacmanDeathSound: chunk = deathSound; channel = &levelStartChanel; break;
        }

        *channel = Mix_PlayChannel(-1, chunk, 0);
}

void play_music(void)
{

}

void stop_sound(SoundEffect effectName)
{
        int channel;

        switch (effectName)
        {
                case LevelStartSound:  channel = levelStartChanel; break;
                case SmallSound:    channel = levelStartChanel; break;
                case LargeSound:        channel = levelStartChanel; break;
                case SirenSound:        channel = levelStartChanel; break;
                case FruitSound:       channel = &levelStartChanel; break;
                case Munch_Asound:    channel = &levelStartChanel; break;
                case Munch_Bsound:    channel = &levelStartChanel; break;
                case BoosterSound:    channel = &levelStartChanel; break;
                case BonusSound:    channel = &levelStartChanel; break;
                case DieSound:    channel = &levelStartChanel; break;
                case Chomp1Sound:    channel = &levelStartChanel; break;
                case Chomp2Sound:    channel = &levelStartChanel; break;
                case CutsceneSound:    channel = &levelStartChanel; break;
                case CoinSound:    channel = &levelStartChanel; break;
                case GameoverSound:    channel = &levelStartChanel; break;

                case PacmanDeathSound: channel = levelStartChanel; break;
                default: printf("badsound\naborting\n"); exit(1); //TODO: fix this up
        }
        Mix_HaltChannel(channel);
}


void stop_music(void)
{

}
