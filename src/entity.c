#include <stdlib.h>
#include "simple_logger.h"
#include "entity.h"
#include "gf2d_draw.h"
#include "damage.h"
#include "gfc_vector.h"
#include "player.h"

const Uint8 * keys;
static EntityManager entity_manager = {0};

EntityManager entity_manager_get_active()
{
    return entity_manager;
}

void entity_manager_close()
{
    int i;
    for (i = 0; i < entity_manager.maxEnts; i++)
    {
        if (entity_manager.entityList[i]._inuse)
        {
            entity_free(&entity_manager.entityList[i]);
        }
    }
    entity_manager.maxEnts = 0;
    free(entity_manager.entityList);
    entity_manager.entityList = NULL;
    slog("entity manager closed");
}

void entity_manager_init(Uint32 maxEnts)
{
    if (entity_manager.entityList != NULL)
    {
        //TODO: cleanup
    }
    if (!maxEnts)
    {
        slog("cannot initialize a zero size entity list!");
        return;
    }
    entity_manager.entityList = malloc(sizeof(Entity) * maxEnts);
    if (entity_manager.entityList == NULL)
    {
        slog("failed to allocate %i entities for the entity manager", maxEnts);
        return;
    }
    entity_manager.maxEnts = maxEnts;
    memset(entity_manager.entityList,0,sizeof(Entity)*maxEnts);
    slog("entity manager initialized");
    atexit(entity_manager_close);

}

Entity *entity_new()
{
    int i;
    if (entity_manager.entityList == NULL)
    {
        slog("entity system does not exist");
        return NULL;
    }
    for (i = 0; i < entity_manager.maxEnts; i++)
    {
        if (entity_manager.entityList[i]._inuse)continue; //entity in use
        memset(&entity_manager.entityList[i],0,sizeof(Entity));
        entity_manager.entityList[i]._inuse = 1;
        return &entity_manager.entityList[i];
    }
    slog("out of open entity slots in memory!");
    return NULL;
}

void free_all_ents()
{
    int i;
    for (i = 0; i < entity_manager.maxEnts; i++)
    {
        if (entity_manager.entityList[i]._inuse && entity_manager.entityList[i].tag != ENT_PLAYER)
        {
            entity_free(&entity_manager.entityList[i]);
        }
    }
}

void entity_free(Entity *self)
{
    if (!self)return;
    gf2d_sprite_free(self->sprite);
    self->sprite = NULL;
    self->hitBox.x = 0;
    self->hitBox.y = 0;
    self->hitBox.w = 0;
    self->hitBox.h = 0;
    self->hitCircle.r = 0;
    fmap_free(self->frameMapping);
    self->_inuse = 0;
}

void entity_update(Entity *self)
{
    if (!self)return;
    //self->frame = self->frame + 0.1;
    //
    //if (self->frame > 5)self->frame = 0;
    if (self->think)self->think(self);
    //damage_collision_check();
}

void entity_update_all()
{
    int i;
    for (i = 0; i < entity_manager.maxEnts; i++)
    {
        if (!entity_manager.entityList[i]._inuse)continue;
        entity_update(&entity_manager.entityList[i]);
    }
}

void entity_draw(Entity *self)
{
    if (self == NULL)
    {
        slog("cannot draw sprite, NULL entity provided!");
        return;
    }
    gf2d_sprite_draw(
    self->sprite,
    self->position,
    self->scale,
    self->offset,
    self->rotation,
    self->flip,
    NULL,
    (Uint32)self->frame);
}

void entity_draw_all()
{
    int i;
    for (i = 0; i < entity_manager.maxEnts; i++)
    {
        if (!entity_manager.entityList[i]._inuse)continue;
        entity_draw(&entity_manager.entityList[i]);
    }
}

void entity_draw_hitbox(Entity *self)
{
    switch(self->hbType)
    {
        case HB_CIRCLE:
            gf2d_draw_circle(vector2d(self->hitCircle.x,self->hitCircle.y),self->hitCircle.r, vector4d(255,100,255,200));
            if (self->tag == ENT_PLAYER)
            {
                gf2d_draw_circle(vector2d(self->aiDistLarge.x,self->aiDistLarge.y),self->aiDistLarge.r, vector4d(255,100,255,200));
                gf2d_draw_circle(vector2d(self->aiDistMid.x,self->aiDistMid.y),self->aiDistMid.r, vector4d(255,100,255,200));
                gf2d_draw_circle(vector2d(self->aiDistClose.x,self->aiDistClose.y),self->aiDistClose.r, vector4d(255,100,255,200));
            }
            break;
        case HB_RECT:
            gf2d_draw_rect(self->hitBox, vector4d(255,100,255,200));
            break;
    }
}

void entity_draw_all_hitboxes()
{
    int i;
    for (i = 0; i < entity_manager.maxEnts; i++)
    {
        if (!entity_manager.entityList[i]._inuse)continue;
        entity_draw_hitbox(&entity_manager.entityList[i]);
    }
}

Entity *menu_selector(Vector2D initPos)
{
    Entity *self;
    self = entity_new();
    if (!self)return NULL;
    

    return self;
}

int proj_in_range(Entity *self)
{
    int i;
    for (i = 0; i < entity_manager.maxEnts; i++)
    {
        if (!entity_manager.entityList[i]._inuse)continue;
        if (entity_manager.entityList[i].tag == ENT_PROJ && collide_ent_circle(&entity_manager.entityList[i],self->aiDistClose))
        {
            if(entity_manager.entityList[i].parent->p != self->p)return 1;
        }else if (entity_manager.entityList[i].tag == ENT_BLAST && collide_ent_circle(&entity_manager.entityList[i],self->aiDistClose))
        {
            if(entity_manager.entityList[i].parent->p != self->p)return 1;
        }
    }
    return 0;
}

void ent_face_eo(Entity *ent1, Entity *ent2)
{
    Vector3D *rot = vector3d_new();
    rot->x = 23;
    rot->y = 35;
    Vector2D *angle = vector2d_new();
    angle->x = ent2->position.x - ent1->position.x;
    angle->y = ent2->position.y - ent1->position.y;
    
    rot->z = vector_angle(angle->x,angle->y);
    
    
    ent1->rotation->z = rot->z;
    rot->z += 180;
    angle_clamp_degrees(&rot->z);

    ent2->rotation->z = rot->z;

    vector2d_normalize(angle);
    ent1->unitDirection = angle;
    //angle->x *= -1;
    //angle->y *= -1;
    ent2->unitDirection = angle;
    
    //slog("e1: %f | e2: %f", ent1->rotation->z, ent2->rotation->z);

    //ent1->rotation->x = 10;           //
    //ent1->rotation->y = 30;           //sets offset for ents rot
    //ent2->rotation->x = 23;           //
    //ent2->rotation->y = 50;           //

    if (ent1->position.x > ent2->position.x)
    {
        ent1->flip->y = 1; 
        if (ent1->altY)
        {
            ent1->offset->y = ent1->altY;
            ent1->rotation->y = ent1->altY;
        }
    }else{
        ent1->flip->y = 0; 
        ent1->offset->y = ent1->mainY;
        ent1->rotation->y = ent1->mainY;
    }

    if (ent2->position.x > ent1->position.x)
    {
        ent2->flip->y = 1;
        if (ent2->altY)
        {
            ent2->offset->y = ent2->altY;
            ent2->rotation->y = ent2->altY;
        }
    }else{
        ent2->flip->y = 0;
        ent2->offset->y = ent2->mainY;
        ent2->rotation->y = ent2->mainY;
    }
}

void ent_move_toward_ent(Entity *e1, Entity *e2)
{

}


Entity *get_player_1()
{
    Entity *p1;
    int i;
    for (i = 0; i < entity_manager.maxEnts; i++)
    {
        if ((entity_manager.entityList[i].p = 1) & (entity_manager.entityList[i]._inuse))
        {
            slog("p1 found");
            p1 = &entity_manager.entityList[i];
            return p1;
        }
    }
    slog("No player 1 found");
    return NULL;
}

Entity *get_player_2()
{
    Entity *p2;
    int i;
    for (i = 0; i < entity_manager.maxEnts; i++)
    {
        if ((entity_manager.entityList[i].p = 2) & (entity_manager.entityList[i]._inuse))
        {
            slog("p2 found");
            p2 = &entity_manager.entityList[i];
            return p2;
        }
    }
    slog("No player 2 found");
    return NULL;
}

int is_in_array(SJson *array, int num)
{
    int temp, i;
    for (i=0;i < sj_array_get_count(array); i++)
    {
        sj_get_integer_value(sj_array_get_nth(array, i), &temp);
        if (num == temp)
        {
            return 1;
        }
    }
    return 0;
}

//Entity *doneEnt()
//{
//    Entity *self;
//    self = entity_new();
//    if (!self)return NULL;
//    self->done = 0;
//    self->tag = ENT_DONE;
//    return self;
//}
//Entity *get_done_ent()
//{
//    for (int i = 0; i < entity_manager_get_active().maxEnts; i++)
//    {
//        if(entity_manager_get_active().entityList[i].tag == ENT_DONE)
//        {
//            return &entity_manager_get_active().entityList[i];
//        }
//    }
//    return NULL;
//}