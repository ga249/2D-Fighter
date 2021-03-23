#include "level.h"

Vector4D v4d_red   = {255, 100, 100, 255};
Vector4D v4d_green = {100, 255, 100, 255};
Vector4D v4d_blue  = {100, 100, 255, 255};

static Level *current_level = NULL;
const Uint8 *keys;
float pauseBuffer;

Level *level_get_active()
{
    return current_level;
}

Level *level_new(Sprite *backgroundFile)
{
    Level *level;
    if (!backgroundFile)
    {
        slog("No backgroundFile provided");
        return NULL;
    }
    level = (Level*)gfc_allocate_array(sizeof(Level), 1); //allocate array
    if(!level)return NULL;
    level->background = backgroundFile;
    SDL_Rect bounds;
    gfc_rect_set(bounds,0,0, LEVEL_WIDTH, LEVEL_HEIGHT);
    level->bounds = bounds;
    current_level = level;
    return level;
}

void level_free(Level *level)
{
    if (!level)return;
    gf2d_sprite_free(level->background);
    free(level);
}

void level_draw(Level *level){
    if (!level)return;
    gf2d_sprite_draw_image(level->background,vector2d(0,0));
}

void level_update_bounds_clip(Level *lvl)
{
    if(!lvl)return;
    EntityManager entity_manager = entity_manager_get_active();
    int i;
    for (i = 0; i < entity_manager.maxEnts; i++)
    {
        if (entity_manager.entityList[i]._inuse)
        {
            if (entity_manager.entityList[i].tag == 0)
            {
                if (entity_manager.entityList[i].hitBox.x < lvl->bounds.x)       //test left bounds
                {
                    entity_manager.entityList[i].position.x = lvl->bounds.x;
                }
                if (entity_manager.entityList[i].hitBox.x > lvl->bounds.w)       //test right bounds
                {
                    entity_manager.entityList[i].position.x = lvl->bounds.w - entity_manager.entityList[i].hitBox.w;
                }
                if (entity_manager.entityList[i].hitBox.y < lvl->bounds.y)      //test upper bounds
                {
                    entity_manager.entityList[i].position.y = lvl->bounds.y;
                }
                if (entity_manager.entityList[i].hitBox.y + entity_manager.entityList[i].hitBox.h > lvl->bounds.h)      //test lower bounds
                {
                    entity_manager.entityList[i].position.y = lvl->bounds.h - entity_manager.entityList[i].hitBox.h;
                }
            }
        }
    }

}

void level_update(Level *lvl)
{
    if(!lvl)return;
    keys = SDL_GetKeyboardState(NULL);
    level_update_bounds_clip(lvl);

    if (keys[SDL_SCANCODE_P])
    {
        if (SDL_GetTicks() - pauseBuffer >= 200)
        {
            pauseBuffer = SDL_GetTicks();

            if(lvl->paused)
            {
                lvl->paused = 0;
            }else{
                lvl->paused = 1;
            }
        }
    }
}

//void level_spawn_players(Level *level)
//{
//    if(!level)return;
//    spawnPlayer(vector2d(100,300),gf2d_sprite_load_all("images/gokuu.png",46,65,5), 0);
//    spawnPlayer(vector2d(400,500),gf2d_sprite_load_all("images/gokuu.png",46,65,5), 1);
//}