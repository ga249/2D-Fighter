#include <stdlib.h>
#include "simple_logger.h"
#include "entity.h"
#include "gf2d_draw.h"
#include "damage.h"

void projectile_think(Entity *self)
{
    if(self->ttd == 0.0)
    {
        entity_free(self);
    }else
    {        
        if(self->ttd > 0)
        {
            self->ttd -= .01;
        }else{
            self->ttd = 0;
        }
    //------------------------------------
        vector2d_set_magnitude(self->unitDirection, 2.0);
        self->position.x += self->unitDirection->x;
        self->position.y += self->unitDirection->y;


    }
}

    

void spawn_projectile(Entity *parent, char *type)
{
    
    Entity *self;
    float tempRot;
    Vector2D *tempDir;
    self = entity_new();
    self->rotation = vector3d_new();
    self->rotation->x = 43;
    self->rotation->y = 40;
    if (strcmp(type, "kiBlastSmall") == 0)
    {
        self->position = parent->position;
        self->sprite = gf2d_sprite_load_all("images/kiBlast.png", 86, 81, 2);   
        self->think = projectile_think;
        
        tempRot = parent->rotation->z - 90;
        self->rotation->z = tempRot;
        //self->rotation->z = parent->rotation->z ;

        tempDir = parent->unitDirection;
        self->unitDirection = tempDir;

        self->ttd = 2;
        self->frame = 0;
        self->sprite->actionSpec = vector3d(0,0,2);
        self->tag = ENT_PROJ;
    }
}

