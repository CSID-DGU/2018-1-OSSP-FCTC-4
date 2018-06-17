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
static Mix_Chunk *introbgmSound;
static Mix_Chunk *bossSound;
static Mix_Chunk *endingbgmSound;
static Mix_Chunk *beepSound;


static Mix_Chunk *stage1Sound;
static Mix_Chunk *stage2Sound;
static Mix_Chunk *stage3Sound;
static Mix_Chunk *stage4Sound;

static int levelStartChanel;
static int smallStartChanel;
static int largeStartChanel;
static int sirenChanel;
static int fruitEatChanel;
static int munch_aChanel;
static int munch_bChanel;
static int bonusChanel;
static int dieChanel;
static int deathChanel;
static int chomp1Chanel;
static int chomp2Chanel;
static int cutsceneChanel;
static int coinChanel;
static int gameoverChanel;
static int introChanel;
static int endingChanel;
static int beepChanel;
static int bossStartChanel;
static int stage1StartChanel;
static int stage2StartChanel;
static int stage3StartChanel;
static int stage4StartChanel;
static int boosterChanel;



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
        introbgmSound = Mix_LoadWAV("sound/Requiem_In_Cello.wav");
        bossSound = Mix_LoadWAV("sound/Eyes_of_Glory.wav");
        endingbgmSound = Mix_LoadWAV("sound/Toe_Jam.wav");
        stage1Sound = Mix_LoadWAV("sound/Coupe.wav");
        stage2Sound = Mix_LoadWAV("sound/Operatic_3.wav");
        stage3Sound = Mix_LoadWAV("sound/Stuff.wav");
        stage4Sound = Mix_LoadWAV("sound/Apprehensive_at_Best.wav");
        beepSound = Mix_LoadWAV("sound/beep2.wav");
	
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
	switch(effectName)
		{
		case LevelStartSound:  chunk = levelStart; channel = &levelStartChanel; break;
                case SmallSound:    chunk = smallEatSound; channel = &smallStartChanel; break;
                case LargeSound:    chunk = largeEatSound; channel = &largeStartChanel; break;
                case SirenSound:       chunk = sirenSound; channel = &sirenChanel; break;
                case FruitSound:    chunk = fruitEatSound; channel = &fruitEatChanel; break;
                case Munch_Asound:  chunk = munch_aSound;  channel = &munch_aChanel; break;
                case Munch_Bsound: chunk = munch_bSound;   channel = &munch_bChanel; break;
                case BoosterSound:  chunk = boosterSound;  channel = &boosterChanel; break;
                case BonusSound:  chunk = bonusSound;  channel = &bonusChanel; break;
                case DieSound:  chunk = dieSound;  channel = &dieChanel; break;
                case Chomp1Sound:  chunk = chomp1Sound;  channel = &chomp1Chanel; break;
                case Chomp2Sound:  chunk = chomp2Sound;  channel = &chomp2Chanel; break;
                case CutsceneSound:  chunk = cutsceneSound;  channel = &cutsceneChanel; break;
                case CoinSound:  chunk = coinSound;  channel = &coinChanel; break;
                case GameoverSound:  chunk = gameoverSound;  channel = &gameoverChanel; break;
                case PacmanDeathSound: chunk = deathSound; channel = &deathChanel; break;
                case IntrobgmSound:  chunk = introbgmSound;  channel = &introChanel; break;
                case BossSound:  chunk = bossSound;  channel = &bossStartChanel; break;
                case EndingbgmSound:  chunk = endingbgmSound;  channel = &endingChanel; break;
                case Stage1Sound:  chunk = stage1Sound;  channel = &stage1StartChanel; break;
                case Stage2Sound:  chunk = stage2Sound;  channel = &stage2StartChanel; break;
                case Stage3Sound:  chunk = stage3Sound;  channel = &stage3StartChanel; break;
                case Stage4Sound:  chunk = stage4Sound;  channel = &stage4StartChanel; break;
                case BeepSound:  chunk = beepSound;  channel = &beepChanel; break;
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
                case SmallSound:    channel = smallStartChanel; break;
                case LargeSound:        channel = largeStartChanel; break;
                case SirenSound:        channel = sirenChanel; break;
               // case FruitSound:       channel = &fruitEatChanel; break;
               // case Munch_Asound:    channel = &munch_aChanel; break;
               // case Munch_Bsound:    channel = &munch_bChanel; break;
               // case BoosterSound:    channel = &boosterChanel; break;
               // case BonusSound:    channel = &bonusChanel; break;
               // case DieSound:    channel = &dieChanel; break;
               // case Chomp1Sound:    channel = &chomp1Chanel; break;
               // case Chomp2Sound:    channel = &chomp2Chanel; break;
               // case CutsceneSound:    channel = &cutsceneChanel; break;
               // case CoinSound:    channel = &coinChanel; break;
               // case GameoverSound:    channel = &gameoverChanel; break;
               // case PacmanDeathSound: channel = deathChanel; break;
                case IntrobgmSound: channel = introChanel; break;
                case BossSound: channel = bossStartChanel; break;
                case EndingbgmSound: channel = endingChanel; break;
                case Stage1Sound: channel = stage1StartChanel; break;
                case Stage2Sound: channel = stage2StartChanel; break;
                case Stage3Sound: channel = stage3StartChanel; break;
                case Stage4Sound: channel = stage4StartChanel; break;
                case BeepSound: channel = beepChanel; break;
		
                default: printf("badsound\naborting\n"); exit(1); //TODO: fix this up

                       }
        Mix_HaltChannel(channel);
}


void stop_music(void)
{

}
