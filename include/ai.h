#ifndef _AI_H_
#define _AI_H_

#include "combat_actions.h"
#include "simple_logger.h"
#include "entity.h"
#include "player.h"
#include "damage.h"
#include "collisions.h"
#include "gfc_types.h"

void ai_think(Entity *self);

#endif