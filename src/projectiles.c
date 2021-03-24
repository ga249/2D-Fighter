#include <stdlib.h>
#include "simple_logger.h"
#include "entity.h"
#include "gf2d_draw.h"
#include "damage.h"
#include "gfc_vector.h"

void projectile_think(Entity *self)
{
    switch (self->tag)
    {
        case ENT_PROJ:
            if(self->ttd == 0.0)
            {
                entity_free(self);
            }else
            {        
                if(self->ttd > 0)
                {
                    self->ttd -= .01;
                    slog("%f", self->ttd);
                }else{
                    self->ttd = 0;
                }
            //------------------------------------
                //slog("%f,%f",self->unitDirection->x,self->unitDirection->y);
                vector2d_set_magnitude(self->unitDirection, 3.0);
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

                if (collide_ent(self,self->target))
                {
                    damage_deal(self,self->target);
                    entity_free(self);
                }

            }
        case ENT_BLAST:
            if (self->parent->p == 1)
            {
                vector2d_set_magnitude(self->unitDirection, 2.25);
                self->hitCircle.x += self->unitDirection->x;
                self->hitCircle.y += self->unitDirection->y;
                self->frame += .05;
                if (self->frame >= 5)entity_free(self);
            }else if (self->parent->p == 2)
            {
                vector2d_set_magnitude(self->unitDirection, 1.5);
                self->hitCircle.x -= self->unitDirection->x;
                self->hitCircle.y -= self->unitDirection->y;
                self->frame += .05;
                if (self->frame >= 14)entity_free(self);
            }
            //self->parent->frame = 18;
            self->parent->flag = BLASTING;

            if (collide_ent(self,self->target))
            {
                //slog("blast collide");
                damage_deal(self,self->target);
                entity_free(self);
            }
            
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
    self->think = projectile_think;
    
    if (strcmp(type, "kiBlastSmall") == 0)
    {
        self->position = parent->position;
        self->sprite = gf2d_sprite_load_all("images/kiBlast.png", 86, 81, 2);   
        parent->ki -= 15;
        self->dmg = 10;

        self->rotation->x = 43;//-20;
        self->rotation->y = 40.5;//-25;

        tempRot = parent->rotation->z + 90;
        self->rotation->z = tempRot;
        //self->rotation->z = parent->rotation->z ;

        tempDir = parent->unitDirection;
        self->unitDirection = tempDir;
        //slog("x: %f | y: %f", tempDir->x, tempDir->y);
        vector2d_set_magnitude(self->unitDirection, 60);

        if (parent->p == 1)
        {
            self->position.x += self->unitDirection->x;
            self->position.y += self->unitDirection->y;
        }else if (parent->p == 2)
        {
            self->position.x -= self->unitDirection->x;
            self->position.y -= self->unitDirection->y;
        }
        

        self->ttd = 2;
        self->frame = 1;
        self->sprite->actionSpec = vector3d(0,0,2);
        self->tag = ENT_PROJ;
        self->hbType = HB_CIRCLE;
        Circle hc = gf2d_circle(parent->position.x,parent->position.y, 20);
        self->hitCircle = hc;
        self->offset = vector2d_new();
        self->offset->x = 43;
        self->offset->y = 40.5;
        
    }else if(strcmp(type, "superBlast") == 0)
    {
        self->position = parent->position;
        self->sprite = gf2d_sprite_load_all("images/superBlasts.png", 233, 63, 2);   
        self->sprite->actionSpec = vector3d(0,0,2);
        parent->ki -= 200;
        self->dmg = 150;

        self->rotation->x = 0;
        self->rotation->y = 30;

        tempRot = parent->rotation->z;
        self->rotation->z = tempRot;

        tempDir = parent->unitDirection;
        self->unitDirection = tempDir;
        
        vector2d_set_magnitude(self->unitDirection, 40);
        if (parent->p == 1)
        {
            self->position.x += self->unitDirection->x;
            self->position.y += self->unitDirection->y;
            self->frame = 0;
        }else if (parent->p == 2)
        {
            self->position.x -= self->unitDirection->x;
            self->position.y -= self->unitDirection->y;
            self->frame = 7;
        }

        self->hbType = HB_CIRCLE;
        Circle hc = gf2d_circle(self->position.x,self->position.y, 25);
        self->hitCircle = hc;
        self->tag = ENT_BLAST;

        self->offset = vector2d_new();
        self->offset->x = 0;
        self->offset->y = 30;

        
        
        
    }
}

