#include <stdlib.h>
#include "simple_logger.h"
#include "entity.h"
#include "player.h"
#include "gf2d_draw.h"
#include "level.h"

const Uint8 * keys;
float p1projBuffer;
float p2projBuffer;
float p1superBuffer;
float p2superBuffer;


void player1Think(Entity *self)
{
    keys = SDL_GetKeyboardState(NULL);
    SDL_GameController *c = self->controller;
    int speed = self->speed;
    speed = SPEED_BASE;

    //get controller leftstick input
    float lx = SDL_GameControllerGetAxis(c, SDL_CONTROLLER_AXIS_LEFTX);
    float ly = SDL_GameControllerGetAxis(c, SDL_CONTROLLER_AXIS_LEFTY);
    
    //update hitboxes
    self->hitBox.x = self->position.x;
    self->hitBox.y = self->position.y;

    self->hitCircle.x = self->position.x;// + 10;
    self->hitCircle.y = self->position.y;// + 35;

    if (self->flag != IDLE)
    {
        if((self->flag != DAMAGED) & (self->flag != BLASTING) & !(SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_Y)) &!(SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_X)) & (!SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)))
        {
            self->flag = IDLE;
        }
    }
    if (self->flag == BLASTING)
    {
        self->speed = 0;
    }
    
    if (self->flag == IDLE)
    {
        self->frame = 0;
        self->speed = 2;
    }

    //Set dash speed if X is pressed
    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_A))
    {
        if (self->ki > 2)
        {
            speed = SPEED_DASH;
            self->ki -= 2;
        }
    }
    //---------------------------------
    //----MOVEMENT----
    if (self->flag == DAMAGED)
    {
        self->frame = 20;
    }else
    {
        if (keys[SDL_SCANCODE_W] || ly < -600)
        {
            self->position.y -= speed;      //move up
            self->frame = 4;
        }
        if (keys[SDL_SCANCODE_S] || ly > 600)
        {
            self->position.y += speed;      //move down
            self->frame = 3;
        }
        if (keys[SDL_SCANCODE_D] || lx > 600)
        {
            self->position.x += speed;      //move right

            if (self->flip->y)
            {
                self->frame = 2;
            }else{
                self->frame = 1;
            }

        }
        if (keys[SDL_SCANCODE_A] || lx < -600)
        {
            self->position.x -= speed;      //move left

            if (self->flip->y)
            {
                self->frame = 1;
            }else{
                self->frame = 2;
            }
        }
    }
    
    //----------------------------------------------

    

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_X) & (self->flag != DAMAGED))
    {
        self->flag = ATK_LIGHT;
        self->frame += .075;
        //slog("%f",self->frame);
        if ((self->frame > 15) || (self->frame < 11))
        {
            self->frame = 11;
        }
        //self->frame += .1;
    }

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)||keys[SDL_SCANCODE_J])
    {
        if (self->flag != DAMAGED)
        {
            self->flag = BLOCKING;
            self->frame = 19;
        }
    }

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_DPAD_DOWN) & (self->flag != DAMAGED))
    {
        self->flag = CHARGING;
        self->frame = 7;
        if (self->ki < 350)self->ki += 1;
    }

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_B))
    {
        if (self->ki > 0)
        {
            self->frame = 15;
            if (SDL_GetTicks() - p1projBuffer >= 300)
            {
                p1projBuffer = SDL_GetTicks();
                spawn_projectile(self, "kiBlastSmall");
            }
        }
    }

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_Y) & (self->flag != CHARGING))
    {
        self->frame = 18;
        if (self->ki > 200)
        {
            if (SDL_GetTicks() - p1superBuffer >= 1000)
            {
                p1superBuffer = SDL_GetTicks();
                spawn_projectile(self, "superBlast");
            }
            
            
        }
    }

}

void player2Think(Entity *self)
{
    keys = SDL_GetKeyboardState(NULL);
    SDL_GameController *c = self->controller;
    int speed = self->speed;
    speed = SPEED_BASE;

    //get controller leftstick input
    float lx = SDL_GameControllerGetAxis(c, SDL_CONTROLLER_AXIS_LEFTX);
    float ly = SDL_GameControllerGetAxis(c, SDL_CONTROLLER_AXIS_LEFTY);
    
    //update hitboxes
    self->hitBox.x = self->position.x;
    self->hitBox.y = self->position.y;

    self->hitCircle.x = self->position.x;// + 10;
    self->hitCircle.y = self->position.y;// + 35;

    if (self->flag != IDLE)
    {
        if((self->flag != DAMAGED) & !(SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_X)) & (!SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)) & !keys[SDL_SCANCODE_L])
        {
            self->flag = IDLE;
        }
    }
    if (self->flag == IDLE)self->frame = 0;
    
    //Set dash speed if X is pressed
    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_A))
    {
        if (self->ki > 2)
        {
            speed = SPEED_DASH;
            self->ki -= 2;
        }
    }
    //---------------------------------
    //----MOVEMENT----
    if (self->flag == DAMAGED)
    {
        self->frame = 23;
    }else
    {
        if (keys[SDL_SCANCODE_UP] || ly < -600)
        {
            self->position.y -= speed;      //move up
            self->frame = 4;
        }
        if (keys[SDL_SCANCODE_DOWN] || ly > 600)
        {
            self->position.y += speed;      //move down
            self->frame = 3;
        }
        if (keys[SDL_SCANCODE_RIGHT] || lx > 600)
        {
            self->position.x += speed;      //move right

            if (self->flip->y)
            {
                self->frame = 2;
            }else{
                self->frame = 1;
            }

        }
        if (keys[SDL_SCANCODE_LEFT] || lx < -600)
        {
            self->position.x -= speed;      //move left

            if (self->flip->y)
            {
                self->frame = 1;
            }else{
                self->frame = 2;
            }
        }
    }
    //--------------------------------------------

    if ((SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_X)||keys[SDL_SCANCODE_L]) & (self->flag != DAMAGED))
    {
        self->flag = ATK_LIGHT;
        self->frame += .075;
        slog("%f",self->frame);
        if ((self->frame > 16) || (self->frame < 11))
        {
            self->frame = 11;
        }
        //self->frame += .1;
    }

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)||keys[SDL_SCANCODE_J])
    {
        if (self->flag != DAMAGED)
        {
            self->flag = BLOCKING;
            self->frame = 22;
        }
    }

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_DPAD_DOWN) & (self->flag != DAMAGED))
    {
        self->flag = CHARGING;
        self->frame = 7;
        if (self->ki < 350)self->ki += 1;
    }

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_B) || keys[SDL_SCANCODE_U])
    {
        if (self->ki > 0)
        {
            self->frame = 17;
            if (SDL_GetTicks() - p2projBuffer >= 300)
            {
                p2projBuffer = SDL_GetTicks();
                spawn_projectile(self, "kiBlastSmall");
            }
        }
    }

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_Y) & (self->flag != CHARGING))
    {
        self->frame = 21;
        if (self->ki > 200)
        {
            if (SDL_GetTicks() - p2superBuffer >= 1000)
            {
                p2superBuffer = SDL_GetTicks();
                spawn_projectile(self, "superBlast");
            }
            
            
        }
    }

}

Entity *spawnPlayer(Vector2D initPos, Sprite *sprite, int isPlayer2)
{
    Entity *self;
    self = entity_new();
    if (!self)return NULL;
    self->sprite = sprite;
    self->position = initPos;
    self->rotation = vector3d_new();
    self->health = 500;
    self->ki = 350;
    self->tag = ENT_PLAYER;
    self->frame = 0;
    self->flip = vector2d_new();
    self->flip->x = 0;
    self->flip->y = 0;
    self->speed = 2;
    self->scale = vector2d_new();
    self->scale->x = 1.5;
    self->scale->y = 1.5;

    self->hbType = HB_CIRCLE;
    SDL_Rect hb;            //hitbox
    hb.x = 100;
    hb.y = 100;
    hb.h = 65 * 1.5;
    hb.w = 48 * 1.5;
    self->hitBox = hb;
    Circle hc = gf2d_circle(self->position.x ,self->position.y , 30);
    self->hitCircle = hc;

    if (isPlayer2)
    {
        self->think = player2Think;
        self->p = 2;
        self->offset = vector2d_new();
        self->offset->x = 23;
        self->offset->y = 60;

        self->rotation->x = 23;           //sets offset for ents rot
        self->rotation->y = 60;           //

        self->sprite->actionSpec = vector3d(0,0,6);
        self->controller = SDL_GameControllerOpen(1);
    }else{
        self->think = player1Think;
        self->p = 1;
        self->offset = vector2d_new();
        self->offset->x = 30;
        self->offset->y = 40;

        self->rotation->x = 30;           //sets offset for ents rot
        self->rotation->y = 40;           //

        self->sprite->actionSpec = vector3d(0,0,5);
        self->controller = SDL_GameControllerOpen(0);
    }
    return self;
}