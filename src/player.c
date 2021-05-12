#include <stdlib.h>
#include "simple_logger.h"
#include "entity.h"
#include "player.h"
#include "gf2d_draw.h"
#include "level.h"
#include "damage.h"
#include "combat_actions.h"

const Uint8 * keys;
float p1projBuffer;
float p1superBuffer;
float atkBuffer; 

static FMapManager fmap_manager = {0};

FrameMapping *frameMap_new()
{
    int i;
    if (fmap_manager.fmapList == NULL)
    {
        slog("fmap system does not exist");
        return NULL;
    }
    for (i = 0; i < fmap_manager.maxMaps; i++)
    {
        if (fmap_manager.fmapList[i]._inuse)continue; //fmap in use
        memset(&fmap_manager.fmapList[i],0,sizeof(FrameMapping));
        fmap_manager.fmapList[i]._inuse = 1;
        return &fmap_manager.fmapList[i];
    }
    slog("out of open fmap slots in memory!");
    return NULL;
}

void fmap_manager_close()
{
    int i;
    for (i = 0; i < fmap_manager.maxMaps; i++)
    {
        if (fmap_manager.fmapList[i]._inuse)
        {
            fmap_free(&fmap_manager.fmapList[i]);
        }
    }
    fmap_manager.maxMaps = 0;
    free(fmap_manager.fmapList);
    fmap_manager.fmapList = NULL;
    slog("fmap manager closed");
}

void fmap_manager_init(Uint32 maxMaps)
{
    if (fmap_manager.fmapList != NULL)
    {
        //TODO: cleanup
    }
    if (!maxMaps)
    {
        slog("cannot initialize a zero size fmap list!");
        return;
    }
    fmap_manager.fmapList = malloc(sizeof(FrameMapping) * maxMaps);
    if (fmap_manager.fmapList == NULL)
    {
        slog("failed to allocate %i fmaps for the fmap manager", (int)maxMaps);
        return;
    }
    fmap_manager.maxMaps = maxMaps;
    memset(fmap_manager.fmapList,0,sizeof(FrameMapping)*maxMaps);
    slog("fmap manager initialized");
    atexit(fmap_manager_close);
}

void fmap_free(FrameMapping *self)
{
    if(!self)return;
    if(self->_inuse)self->_inuse = 0;
}

void playerThink(Entity *self)
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
    self->aiDistLarge.x = self->position.x;
    self->aiDistLarge.y = self->position.y;
    self->aiDistMid.x = self->position.x;
    self->aiDistMid.y = self->position.y;
    self->aiDistClose.x = self->position.x;
    self->aiDistClose.y = self->position.y;

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
        self->frame = self->frameMapping->idle;
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
        self->frame = self->frameMapping->hit;
    }else if (self->flag != CHARGING)
    {
        if (keys[SDL_SCANCODE_W] || ly < -600)
        {
            self->position.y -= speed;      //move up
            self->frame = self->frameMapping->up;
        }
        if (keys[SDL_SCANCODE_S] || ly > 600)
        {
            self->position.y += speed;      //move down
            self->frame = self->frameMapping->down;
        }
        if (keys[SDL_SCANCODE_D] || lx > 600)
        {
            self->position.x += speed;      //move right

            if (self->flip->y)
            {
                self->frame = self->frameMapping->left;
            }else{
                self->frame = self->frameMapping->right;
            }

        }
        if (keys[SDL_SCANCODE_A] || lx < -600)
        {
            self->position.x -= speed;      //move left

            if (self->flip->y)
            {
                self->frame = self->frameMapping->right;
            }else{
                self->frame = self->frameMapping->left;
            }
        }
    }
    
    //----------------------------------------------

    

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_X) & (self->flag != DAMAGED))
    {
        self->flag = ATK_LIGHT;
        self->frame += .075;
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
                    damage_deal(self, self->target);
                }
            }
        }else{
            self->hitCircle.r = self->hbWidth;
        }

    }

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) & (self->flag != DAMAGED))
    {
        if (self->flag != DAMAGED)
        {
            self->flag = BLOCKING;
            self->frame = self->frameMapping->blocking;
        }
    }

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_DPAD_DOWN) & (self->flag != DAMAGED))
    {
        self->flag = CHARGING;
        self->frame = self->frameMapping->endCharging;
        if (self->ki < 350)self->ki += 1;
    }

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_B) & (self->flag != CHARGING))
    {
        if (self->ki > 0)
        {
            self->frame = self->frameMapping->kiblast;
            if (SDL_GetTicks() - p1projBuffer >= 300)
            {
                p1projBuffer = SDL_GetTicks();
                spawn_projectile(self, "kiBlastSmall");
            }
        }
    }

    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_Y) & (self->flag != CHARGING))
    {
        self->frame = self->frameMapping->end_Super;
        if (self->ki > 200)
        {
            if (SDL_GetTicks() - p1superBuffer >= 1000)
            {
                p1superBuffer = SDL_GetTicks();
                spawn_projectile(self, "superBlast");
            } 
        }
    }

    //if (keys[SDL_SCANCODE_V])
    //{
        //get_closer(self);
    //}
//
    //if (keys[SDL_SCANCODE_COMMA])
    //{
    //    retreat(self);
    //}

}

void player_load(Entity *player,  const char *filename, char *character)
{
    SJson *json, *characterJson, *fmapJson, *atkFramesJson;
    const char *string;

    json = sj_load(filename);

    characterJson = sj_object_get_value(json,character);

    int sWidth,sHeight,fpl;
    string = sj_get_string_value(sj_object_get_value(characterJson,"sprite"));
    sj_get_integer_value(sj_object_get_value(characterJson, "FPL"), &fpl);
    sj_get_integer_value(sj_object_get_value(characterJson, "sWidth"), &sWidth);
    sj_get_integer_value(sj_object_get_value(characterJson, "sHeight"), &sHeight);
    sj_get_integer_value(sj_object_get_value(characterJson, "attackDist"), &player->attackDist);
    if (string)
    {
        player->sprite = gf2d_sprite_load_all((char *)string, sWidth, sHeight, fpl);
    }
    player->sprite->actionSpec = vector3d(0,0,fpl);
    player->offset = vector2d_new();
    sj_get_float_value(sj_object_get_value(characterJson, "offsetX"), &player->offset->x);
    sj_get_float_value(sj_object_get_value(characterJson, "offsetY"), &player->offset->y);
    player->rotation = vector3d_new();
    sj_get_float_value(sj_object_get_value(characterJson, "offsetX"), &player->rotation->x);
    sj_get_float_value(sj_object_get_value(characterJson, "offsetY"), &player->rotation->y);
    sj_get_integer_value(sj_object_get_value(characterJson, "offsetY"), &player->mainY);
    sj_get_integer_value(sj_object_get_value(characterJson, "altY"), &player->altY);
    sj_get_float_value(sj_object_get_value(characterJson, "superBlastSpeed"), &player->superBlastSpeed);
    sj_get_float_value(sj_object_get_value(characterJson, "hbWidth"), &player->hbWidth);
    player->hbType = HB_CIRCLE;
    Circle hc = gf2d_circle(player->position.x ,player->position.y , player->hbWidth);
    player->hitCircle = hc;

    atkFramesJson = sj_object_get_value(characterJson,"attackFrames");

    player->attackFrames = sj_copy(atkFramesJson);
    if (sj_array_check(player->attackFrames))slog("yes");
    if (is_in_array(player->attackFrames, 12))slog("haha");
    //int num;
    //sj_get_integer_value(sj_array_get_nth(atkFramesJson, 0), &num);
    //slog("%i", num); //reeeeeeeeeeeeeeeeee
    
    FrameMapping *fmap = frameMap_new();

    fmapJson = sj_object_get_value(characterJson,"frameMappings");
    sj_get_float_value(sj_object_get_value(fmapJson, "idle"), &fmap->idle);
    sj_get_float_value(sj_object_get_value(fmapJson, "right"), &fmap->right);
    sj_get_float_value(sj_object_get_value(fmapJson, "left"), &fmap->left);
    sj_get_float_value(sj_object_get_value(fmapJson, "down"), &fmap->down);
    sj_get_float_value(sj_object_get_value(fmapJson, "up"), &fmap->up);
    sj_get_float_value(sj_object_get_value(fmapJson, "charging"), &fmap->charging);
    sj_get_float_value(sj_object_get_value(fmapJson, "endCharging"), &fmap->endCharging);
    sj_get_float_value(sj_object_get_value(fmapJson, "blocking"), &fmap->blocking);
    sj_get_float_value(sj_object_get_value(fmapJson, "endMelee"), &fmap->endMelee);
    sj_get_float_value(sj_object_get_value(fmapJson, "melee"), &fmap->melee);
    sj_get_float_value(sj_object_get_value(fmapJson, "kiblast"), &fmap->kiblast);
    sj_get_float_value(sj_object_get_value(fmapJson, "super_key"), &fmap->super_key);
    sj_get_float_value(sj_object_get_value(fmapJson, "end_Super"), &fmap->end_Super);
    sj_get_float_value(sj_object_get_value(fmapJson, "hit"), &fmap->hit);
    sj_get_float_value(sj_object_get_value(fmapJson, "death"), &fmap->death);
    sj_get_float_value(sj_object_get_value(fmapJson, "thrown"), &fmap->thrown);
    sj_get_float_value(sj_object_get_value(fmapJson, "superBlast"), &fmap->superBlast);
    sj_get_float_value(sj_object_get_value(fmapJson, "endSuperBlast"), &fmap->endSuperBlast);

    player->frameMapping = fmap;

    slog("Character loaded");

    sj_free(atkFramesJson);
    sj_free(fmapJson);
    sj_free(characterJson);
    sj_free(json);
}

Entity *spawnPlayer(Vector2D initPos, int isPlayer2, char *character)
{
    Entity *self;
    self = entity_new();
    if (!self)return NULL;
    player_load(self, "characters/characters.json", character);
    self->position = initPos;
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
    Circle hc = gf2d_circle(self->position.x ,self->position.y , 30);
    Circle aiDistLarge = gf2d_circle(self->position.x ,self->position.y , 400);
    Circle aiDistMid = gf2d_circle(self->position.x ,self->position.y , 200);
    Circle aiDistClose = gf2d_circle(self->position.x ,self->position.y , 75);
    self->hitCircle = hc;
    self->aiDistLarge = aiDistLarge;
    self->aiDistMid = aiDistMid;
    self->aiDistClose = aiDistClose;


    self->think = playerThink;

    if (isPlayer2)
    {
        self->p = 2;
        self->controller = SDL_GameControllerOpen(1);
    }else{
        self->p = 1;
        self->controller = SDL_GameControllerOpen(0);
    }
    return self;
}



/*
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
    //SDL_Rect hb;            //hitbox
    //hb.x = 100;
    //hb.y = 100;
    //hb.h = 65 * 1.5;
    //hb.w = 48 * 1.5;
    //self->hitBox = hb;
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
*/

/*
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

    if ((SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_X) & (self->flag != DAMAGED)))
    {
        self->flag = ATK_LIGHT;
        self->frame += .075;
        slog("%f",self->frame);
        if ((self->frame > self->frameMapping->endMelee) || (self->frame < self->frameMapping->melee))
        {
            self->frame = self->frameMapping->melee;
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
*/