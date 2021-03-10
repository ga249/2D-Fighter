#include <stdlib.h>
#include "simple_logger.h"
#include "entity.h"
#include "player.h"
#include "gf2d_draw.h"

const Uint8 * keys;

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

    self->frame = 0;

    //Set dash speed if L2 is pressed
    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
    {
        speed = SPEED_DASH;
    }
    //---------------------------------
    //----MOVEMENT----
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
            self->frame = 1;
        }else{
            self->frame = 2;
        }
        
    }
    if (keys[SDL_SCANCODE_A] || lx < -600)
    {
        self->position.x -= speed;      //move left
        
        if (self->flip->y)
        {
            self->frame = 2;
        }else{
            self->frame = 1;
        }
    }
    //----------------------------------------------

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_A))
    {
        self->sprite->actionSpec = vector3d(5,(65 * 30 - 30),4);    //stick out hand
        self->frame = 3;
        self->health -= .5;
        //slog("c1 pressing A");
    }else
    {
        self->sprite->actionSpec = vector3d(5,(60 * 5 + 13),5);
        //slog("blah");
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

    self->frame = 0;

    //Set dash speed if L2 is pressed
    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
    {
        speed = SPEED_DASH;
    }
    //---------------------------------
    //----MOVEMENT----
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
    //--------------------------------------------

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_A)||keys[SDL_SCANCODE_J])
    {
        self->frame = 17;
        self->health -= .5;
        slog("c2 pressing A");
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
    //self->offset = vector2d_new();
    //self->offset->x = 20;
    //self->offset->y = 20;
    self->scale = vector2d_new();
    self->scale->x = 1.5;
    self->scale->y = 1.5;
    self->sprite->actionSpec = vector3d(0,0,6);
    SDL_Rect hb;            //hitbox
    hb.x = 100;
    hb.y = 100;
    hb.h = 65 * 1.5;
    hb.w = 48 * 1.5;
    self->hitBox = hb;
    if (isPlayer2)
    {
        self->think = player2Think;
        self->controller = SDL_GameControllerOpen(1);
    }else{
        self->think = player1Think;
        self->controller = SDL_GameControllerOpen(0);
    }
    return self;
}