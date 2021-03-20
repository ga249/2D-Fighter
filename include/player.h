#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "gf2d_sprite.h"
#include "gf2d_draw.h"
#include "entity.h"

#define SPEED_BASE      2
#define SPEED_DASH      4

void player1Think(Entity *self);

void player2Think(Entity *self);

/**
 * @brief declare player ent
 * @param initPos initial spawn position
 * @param sprite pointer to sprite for player
 * @param isPlayer2 "bool" to differentiate from player 1 and 2, 0 = p1 1 = p2
 * @return pointer to a player entity
 */
Entity *spawnPlayer(Vector2D initPos, Sprite *sprite, int isPlayer2);


#endif