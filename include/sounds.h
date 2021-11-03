#ifndef __SOUNDS_H__
#define __SOUNDS_H__

#include "gfc_audio.h"

typedef struct Sounds_S
{
    Sound *menuMusic;
    Sound *fightMusic;
    Sound *melee;
    Sound *hit;
    Sound *kiblast;
    Sound *superBlast;
    Sound *teleport;
    Sound *dash;
    Sound *charge;
}Sounds;

void sounds_init();

void play_menu_music();

void play_teleport();

void play_dash();

void play_charge();

#endif
