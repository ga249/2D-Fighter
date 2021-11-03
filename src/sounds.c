#include <SDL.h>

#include "simple_logger.h"
#include "sounds.h"
#include "gfc_audio.h"

static Sounds *sounds = {0};

void sounds_init()
{
    gfc_audio_init(20,20,-1,30,1,1);   //---------------------------sounds

    sounds = (Sounds *)gfc_allocate_array(sizeof(Sounds), 1);

    sounds->menuMusic = gfc_sound_load("sounds/chaLa.mp3", .1, 1); //---------------------------sounds
    sounds->teleport = gfc_sound_load("sounds/common/teleport.ogg", .1, 1);
    sounds->dash = gfc_sound_load("sounds/common/dash.ogg", .1, 1);
    sounds->charge = gfc_sound_load("sounds/charge2.wav", .1, 1);
}

void play_menu_music()
{
    gfc_sound_play(sounds->menuMusic, 1, .1, 1, -1);//---------------------------sounds
}

void play_teleport()
{
    gfc_sound_play(sounds->teleport, 0, .1, 2, -1);//---------------------------sounds
}

void play_dash()
{
    gfc_sound_play(sounds->dash, 0, .1, 3, -1);//---------------------------sounds
}

void play_charge()
{
    gfc_sound_play(sounds->charge, 0, .5, 4, -1);//---------------------------sounds
}