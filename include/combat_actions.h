#ifndef _COMBAT_ACTIONS_H_
#define _COMBAT_ACTIONS_H_

#include "entity.h"

#define SPEED_BASE      2
#define SPEED_DASH      4

void melee_attack(Entity *self);

void block(Entity *self);

void ki_blast(Entity *self);

void super_blast(Entity *self);

void charge(Entity *self);

void get_closer(Entity *self);

void retreat(Entity *self);

void move_up(Entity *self);

void move_down(Entity *self);

void move_left(Entity *self);

void move_right(Entity *self);

/**
 * @brief check distance from self's target
 * @param self the base entity
 * @return 0 = close, 1 = mid, 2 = large, 3 = far
 */
int  check_dist(Entity *self);

#endif