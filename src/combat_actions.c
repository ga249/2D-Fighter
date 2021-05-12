#include "combat_actions.h"
#include "simple_logger.h"
#include "entity.h"
#include "player.h"
#include "damage.h"
#include "gfc_types.h"
#include "gfc_vector.h"

float aiprojBuffer;
float aisuperBuffer;
float atkBuffer; 

void melee_attack(Entity *self)
{
    if (self->flag != DAMAGED)
    {
        self->flag = ATK_LIGHT;
        self->frame += .1;
        //slog("%f",self->frame);
        if ((self->frame > self->frameMapping->endMelee) || (self->frame < self->frameMapping->melee))
        {
            self->frame = self->frameMapping->melee;
        }
        if (is_in_array(self->attackFrames, (int)self->frame))
        {
            self->hitCircle.r = self->hbWidth + self->attackDist;
            if (collide_ent(self, self->target))
            {
                if (SDL_GetTicks() - atkBuffer >= 200)
                {
                    atkBuffer = SDL_GetTicks();
                    if(self->target->afterImgOn == 0)damage_deal(self, self->target);
                }
            }
        }else{
            self->hitCircle.r = self->hbWidth;
        }
    }
}

void block(Entity *self)
{
    if (self->flag != DAMAGED)
    {
        if (self->flag != DAMAGED)
        {
            self->flag = BLOCKING;
            self->frame = self->frameMapping->blocking;
        } 
    }
    
}

void ki_blast(Entity *self)
{
    if (self->flag != DAMAGED)
    {
        if (self->ki > 0)
        {
            self->frame = self->frameMapping->kiblast;
            if (SDL_GetTicks() - aiprojBuffer >= 300)
            {
                aiprojBuffer = SDL_GetTicks();
                spawn_projectile(self, "kiBlastSmall");
            }
        }
    }
}

void super_blast(Entity *self)
{
    if (self->flag != DAMAGED)
    {
        self->frame = self->frameMapping->end_Super;
        if (self->ki > 200)
        {
            if (SDL_GetTicks() - aisuperBuffer >= 1000)
            {
                aisuperBuffer = SDL_GetTicks();
                spawn_projectile(self, "superBlast");
            }
        }
    }
}

void charge(Entity *self)
{
    if (self->flag != DAMAGED)
    {
        self->flag = CHARGING;
        self->frame = self->frameMapping->endCharging;
        if (self->ki < 350)self->ki += 1;
    }
}

void get_closer(Entity *self)
{
    self->frame = self->frameMapping->right;
    vector2d_set_magnitude(self->unitDirection, SPEED_BASE);
    self->position.x -= self->unitDirection->x;
    self->position.y -= self->unitDirection->y;
}

void retreat(Entity *self)
{
    self->frame = self->frameMapping->left;
    vector2d_set_magnitude(self->unitDirection, SPEED_BASE);
    self->position.x += self->unitDirection->x;
    self->position.y += self->unitDirection->y;
}

void move_up(Entity *self)
{
    self->position.y -= self->speed;      //move up
    self->frame = self->frameMapping->up;
}

void move_down(Entity *self)
{
    self->position.y += self->speed;      //move down
    self->frame = self->frameMapping->down;
}

void move_left(Entity *self)
{
    self->position.x -= self->speed;      //move left

    if (self->flip->y)
    {
        self->frame = self->frameMapping->right;
    }else{
        self->frame = self->frameMapping->left;
    }
}

void move_right(Entity *self)
{
    self->position.x += self->speed;      //move right
    if (self->flip->y)
    {
        self->frame = self->frameMapping->left;
    }else{
        self->frame = self->frameMapping->right;
    }
}

int check_dist(Entity *self)
{
    if (collide_ent_circle(self->target,self->aiDistClose))
    {
        return 0;
    }else if(collide_ent_circle(self->target,self->aiDistMid))
    {
        return 1;
    }else if(collide_ent_circle(self->target,self->aiDistLarge))
    {
        return 2;
    }else
    {
        return 3;
    }

}

void afterImgAway(Entity *self)
{
    vector2d_set_magnitude(self->unitDirection, 200);
    if (self->p == 1)
    {
        self->position.x -= self->unitDirection->x;
        self->position.y -= self->unitDirection->y;
    }else if (self->p == 2)
    {
        self->position.x += self->unitDirection->x;
        self->position.y += self->unitDirection->y;
    }
    
}

void afterImgSide(Entity *self)
{
    int random = rand() % 2;
    Vector2D tempDir = vector2d(self->unitDirection->x,self->unitDirection->y);
    Vector2D sideDir;
    if (random == 0)
    {
        sideDir = vector2d_rotate(tempDir, GFC_HALF_PI);
        
    }else{
        sideDir = vector2d_rotate(tempDir, GFC_HALF_PI*GFC_PI);
    }

    vector2d_set_magnitude(&sideDir, 150);
    if (self->p == 1)
    {
        self->position.x -= sideDir.x;
        self->position.y -= sideDir.y;
    }else if (self->p == 2)
    {
        self->position.x += sideDir.x;
        self->position.y += sideDir.y;
    }
}
