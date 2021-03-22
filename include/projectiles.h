#ifndef __PROJECTILES_H__
#define __PROJECTILES_H__

#include "gf2d_draw.h"
#include "gf2d_sprite.h"
#include "entity.h"

void projectile_think(Entity *self);

void spawn_projectile(Entity *parent, char *type);


#endif
//typedef struct Projectile_S
//{
//    Uint8        _inuse;        /**<check if this proj in memory is active or not>*/
//    Sprite      *sprite;        /**<a pointer to the sprite that is used by this proj>*/
//    
//    Vector2D    *offset;        /**<offset of sprite draw>*/
//    Vector2D    *scale;         /**<scaleOffset of sprite draw>*/
//    Vector2D    *flip;          /**<(flip Horiz,flip Vert)>*/
//
//
//}Projectile;