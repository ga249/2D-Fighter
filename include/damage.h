#ifndef __DAMAGE_H__
#define __DAMAGE_H__

#include "entity.h"
#include "player.h"
#include "collisions.h"

/**
 * @brief Deal damage to victim from child
 * @param parent Entity dealing damage
 * @param victim Entity recieving damage
 */
void damage_deal(Entity *parent, Entity *victim);

/**
 * @brief run through list of entities and check if any players are colliding & have atking flags
 */
void damage_collision_check();

#endif