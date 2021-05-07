#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "gf2d_sprite.h"
#include "gf2d_draw.h"
#include "entity.h"
#include "projectiles.h"

#define SPEED_BASE      2
#define SPEED_DASH      4

typedef struct FrameMapping_S
{
    Uint8   _inuse;         /**<check if this entity in memory is active or not>*/    
    float     idle;
    float     right;
    float     left;
    float     down;      
    float     up;       
    float     charging;    
    float     endCharging;
    float     blocking;
    float     melee;  
    float     endMelee;    
    float     kiblast;   
    float     super_key;      
    float     end_Super;
    float     hit;         
    float     death;
    float     thrown;    
    float     superBlast; 
    float     endSuperBlast;
}FrameMapping;

typedef struct FMapManager_S
{
    Uint32  maxMaps;         /**<Maximum number of frame maps*/
    FrameMapping  *fmapList;     /**<List of frame maps*/
}FMapManager;

FrameMapping *frameMap_new();

/**
 * @brief initialize the frame map manager
 * @param maxMaps upper bound of maximum concurrent entities to be supported
 * @note must be called before creating a new entity
 */
void fmap_manager_init(Uint32 maxMaps);

void fmap_free(FrameMapping *self);

void playerThink(Entity *self);

//void player2Think(Entity *self);

void player_load(Entity *player,  const char *filename, char *character);

/**
 * @brief declare player ent
 * @param initPos initial spawn position
 * @param sprite pointer to sprite for player
 * @param isPlayer2 "bool" to differentiate from player 1 and 2, 0 = p1 1 = p2
 * @param target 
 * @return pointer to a player entity
 */
Entity *spawnPlayer(Vector2D initPos, int isPlayer2, char *charcater);


#endif