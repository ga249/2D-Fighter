#include "level.h"
#include "menus.h"

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

Level *level_new(Sprite *backgroundFile, Entity *p1, Entity *p2)
{
    Level *level;
    if (!backgroundFile)
    {
        slog("No backgroundFile provided");
        return NULL;
    }
    if (!p1 || !p2)
    {
        slog("No players provided for level");
    }
    level = (Level*)gfc_allocate_array(sizeof(Level), 1); //allocate array
    if(!level)return NULL;
    level->background = backgroundFile;
    level->p1 = p1;
    level->p2 = p2;
    SDL_Rect bounds;
    gfc_rect_set(bounds,0,0, LEVEL_WIDTH, LEVEL_HEIGHT);
    level->bounds = bounds;
    current_level = level;
    return level;
}

void level_load_from(Level *level, const char *filename)
{
    SJson *json, *LevelJS;
    const char *string;

    json = sj_load(filename);

    LevelJS = sj_object_get_value(json,"level");

    string = sj_get_string_value(sj_object_get_value(LevelJS,"bgImage"));
    if (string)
    {
        level->background = gf2d_sprite_load_image((char *)string);
    }
    sj_get_float_value(sj_object_get_value(LevelJS, "p1posx"), &level->p1->position.x);
    sj_get_float_value(sj_object_get_value(LevelJS, "p1posy"), &level->p1->position.y);
    sj_get_float_value(sj_object_get_value(LevelJS, "p2posx"), &level->p2->position.x);
    sj_get_float_value(sj_object_get_value(LevelJS, "p2posy"), &level->p2->position.y);

    sj_get_integer_value(sj_object_get_value(LevelJS, "p1health"), &level->p1->health);
    sj_get_integer_value(sj_object_get_value(LevelJS, "p2health"), &level->p2->health);
    sj_get_integer_value(sj_object_get_value(LevelJS, "p1ki"), &level->p1->ki);
    sj_get_integer_value(sj_object_get_value(LevelJS, "p2ki"), &level->p2->ki);

    sj_free(LevelJS);
    sj_free(json);
}

void level_save(Level *level, const char *filename)
{
    SJson *json, *LevelJS;
    json = sj_object_new();
    LevelJS = sj_object_new();
    
    sj_object_insert(json, "level", LevelJS);

    sj_object_insert(LevelJS, "bgImage", sj_new_str("images/backgrounds/namek.png"));
    sj_object_insert(LevelJS, "p1posx", sj_new_float(level->p1->position.x));
    sj_object_insert(LevelJS, "p1posy", sj_new_float(level->p1->position.y));
    sj_object_insert(LevelJS, "p2posx", sj_new_float(level->p2->position.x));
    sj_object_insert(LevelJS, "p2posy", sj_new_float(level->p2->position.y));
    sj_object_insert(LevelJS, "p1health", sj_new_int(level->p1->health));
    sj_object_insert(LevelJS, "p2health", sj_new_int(level->p2->health));
    sj_object_insert(LevelJS, "p1ki", sj_new_int(level->p1->ki));
    sj_object_insert(LevelJS, "p2ki", sj_new_int(level->p2->ki));

    sj_save(json, (char *)filename);
    sj_free(LevelJS);
    sj_free(json);

    
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
                if (entity_manager.entityList[i].position.x - entity_manager.entityList[i].hitCircle.r < lvl->bounds.x)       //test left bounds
                {
                    entity_manager.entityList[i].position.x = lvl->bounds.x + entity_manager.entityList[i].hitCircle.r;
                }
                if (entity_manager.entityList[i].hitCircle.x + entity_manager.entityList[i].hitCircle.r > lvl->bounds.w)       //test right bounds
                {
                    entity_manager.entityList[i].position.x = lvl->bounds.w - entity_manager.entityList[i].hitCircle.r;
                }
                if (entity_manager.entityList[i].hitCircle.y - entity_manager.entityList[i].hitCircle.r < lvl->bounds.y)      //test upper bounds
                {
                    entity_manager.entityList[i].position.y = lvl->bounds.y + entity_manager.entityList[i].hitCircle.r;
                }
                if (entity_manager.entityList[i].hitCircle.y + entity_manager.entityList[i].hitCircle.r > lvl->bounds.h)      //test lower bounds
                {
                    entity_manager.entityList[i].position.y = lvl->bounds.h - entity_manager.entityList[i].hitCircle.r;
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

    //if (lvl->p1->health <= 0)
    //{
    //    lvl
    //}

    switch (lvl->screen)
    {
    case MAIN_MENU:
        lvl->background = gf2d_sprite_load_image("images/backgrounds/mainMenu.png");
        break;
    
    case IN_GAME:
        lvl->background = gf2d_sprite_load_image("images/backgrounds/namek.png");
        break;
    }

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

    //--------------------------------------------------------debugging-------------------------------------------

    //if (keys[SDL_SCANCODE_L])
    //{
    //    if (SDL_GetTicks() - pauseBuffer >= 200)
    //    {
    //        pauseBuffer = SDL_GetTicks();
//
    //        level_load_from(lvl, "levels/level.json");
    //    }
    //}
    //if (keys[SDL_SCANCODE_SPACE])
    //{
    //    if (SDL_GetTicks() - pauseBuffer >= 200)
    //    {
    //        pauseBuffer = SDL_GetTicks();
//
    //        level_save(lvl, "levels/save.json");
    //    }
    //}
    //if (keys[SDL_SCANCODE_C])
    //{
    //    if (SDL_GetTicks() - pauseBuffer >= 200)
    //    {
    //        pauseBuffer = SDL_GetTicks();
//
    //        level_load_from(lvl, "levels/save.json");
    //    }
    //}

    if (keys[SDL_SCANCODE_M])
    {
        if (SDL_GetTicks() - pauseBuffer >= 200)
        {
            pauseBuffer = SDL_GetTicks();

            if (lvl->screen == MAIN_MENU)
            {
                lvl->screen = IN_GAME;
            }else if (lvl->screen == IN_GAME)
            {
                lvl->screen = MAIN_MENU;
            }
        }
    }
    //--------------------------------------------------------------------------------------------------------------------------
}
