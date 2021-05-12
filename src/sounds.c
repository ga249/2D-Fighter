#include <SDL.h>

#include "simple_logger.h"
#include "sounds.h"
#include "gfc_audio.h"

static Sounds *sounds = {0};

void sounds_init()
{
    //gfc_audio_init(20,20,-1,30,1,1);

    sounds = (Sounds *)gfc_allocate_array(sizeof(Sounds), 1);

    //sounds->menuMusic = gfc_sound_load("sounds/chaLa.mp3", .1, 1);
    
}

void play_menu_music()
{
    //gfc_sound_play(sounds->menuMusic, 1, .1, 1, -1);
}