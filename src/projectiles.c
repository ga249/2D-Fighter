#include <stdlib.h>
#include "simple_logger.h"
#include "entity.h"
#include "gf2d_draw.h"
#include "damage.h"
#include "gfc_vector.h"

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
        slog("%f,%f",self->unitDirection->x,self->unitDirection->y);
        vector2d_set_magnitude(self->unitDirection, 2.0);
        if (self->parent->p == 1)
        {
            self->position.x += self->unitDirection->x;
            self->position.y += self->unitDirection->y;
        }else if (self->parent->p == 2){
            self->position.x -= self->unitDirection->x;
            self->position.y -= self->unitDirection->y;
        }
        
        self->hitCircle.x = self->position.x;
        self->hitCircle.y = self->position.y;
        gf2d_draw_circle(vector2d(self->hitCircle.x,self->hitCircle.y),self->hitCircle.r, vector4d(255,100,255,200));

    }
}

    

void spawn_projectile(Entity *parent, char *type)
{
    
    Entity *self;
    float tempRot;
    Vector2D *tempDir;
    self = entity_new();
    self->parent = parent;
    self->target = parent->target;
    self->rotation = vector3d_new();
    self->rotation->x = -20;
    self->rotation->y = 50;
    if (strcmp(type, "kiBlastSmall") == 0)
    {
        self->position = parent->position;
        self->sprite = gf2d_sprite_load_all("images/kiBlast.png", 86, 81, 2);   
        self->think = projectile_think;
        
        tempRot = parent->rotation->z + 90;
        self->rotation->z = tempRot;
        //self->rotation->z = parent->rotation->z ;

        tempDir = parent->unitDirection;
        self->unitDirection = tempDir;
        vector2d_set_magnitude(self->unitDirection, 60);

        self->position.x += self->unitDirection->x;
        self->position.y -= self->unitDirection->y;

        self->ttd = 2;
        self->frame = 1;
        self->sprite->actionSpec = vector3d(0,0,2);
        self->tag = ENT_PROJ;
        self->hbType = HB_CIRCLE;
        Circle hc = gf2d_circle(parent->position.x,parent->position.y, 20);
        self->hitCircle = hc;
        self->offset = vector2d_new();
        if (parent->flip->y == 0)
        {
            self->offset->x = -20;
            self->offset->y = 50;
        }else{
            self->offset->x = 0;
            self->offset->y = -15;
        }
        
    }
}

