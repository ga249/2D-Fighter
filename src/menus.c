#include <stdlib.h>
#include "simple_logger.h"
#include "menus.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include "gf2d_draw.h"
#include "collisions.h"
//#include "level.h"
#include "simple_json.h"
#include "entity.h"
#include "level.h"

typedef struct 
{
    Uint32      maxMenus;         /**<Maximum number of entities*/
    Menu        *menuList;     /**<List of entities*/
    TTF_Font    *text;           
}MenuManager;

static MenuManager menu_manager = {0};

Menu *menu_new(){
    int i;
    for (i=0; i < menu_manager.maxMenus; i++){
        if(menu_manager.menuList[i]._inuse)continue;
        menu_manager.menuList[i]._inuse = 1;
        // slog("Items in menu list %d", i);
        return &menu_manager.menuList[i];
    }
    slog("out of open menu slots in memory");
    return NULL;
}

void menu_manager_init(Uint32 maxMenus){
    if (menu_manager.menuList != NULL){
        //TODO: cleanup
    }
    if (!maxMenus){
        slog("cannot init 0 zise menu list");
        return;
    }
    menu_manager.menuList = (Menu * )malloc(sizeof(Menu) * maxMenus);
    if (menu_manager.menuList == NULL){
        slog("failed to allocate %i menus for the menu manager",maxMenus);
    }
    menu_manager.maxMenus = maxMenus;
    memset(menu_manager.menuList,0,sizeof(Menu)*maxMenus);
    slog("Menu manager initalized");
    atexit(menu_manager_close);
}

void menu_manager_close(){
    int i;
    for (i=0; i < menu_manager.maxMenus; i++){
        if(menu_manager.menuList[i]._inuse){
            menu_free(&menu_manager.menuList[i]);
        }
    }
    menu_manager.maxMenus = 0;
    free(menu_manager.menuList);
    menu_manager.menuList =NULL;
    slog("menu manager closed");
}

void menu_free(Menu *self){
    if (!self)return;
    if (!self->sprite)
    gf2d_sprite_free(self->sprite);
    memset(self,0,sizeof(Menu));
}

void menu_update(Menu *self){
    if (!self)return;
    if (self->think){
        self->think(self);
    }
}

void menu_update_group(int group){
    // level_get_active()->frame ++;
    int i;
    for (i = 0;i < menu_manager.maxMenus;i++)
    {
        if (!menu_manager.menuList[i]._inuse)continue;
        if (menu_manager.menuList[i].group == group)menu_update(&menu_manager.menuList[i]);
    }
}

void menu_draw(Menu *self){
    if (self == NULL){
        slog("cannot draw, null menu provided");
        return;
    }
    
    //gf2d_sprite_draw_image(self->sprite, 
    //                       vector2d(self->position.x + self->drawOffset.x,self->position.y + self->drawOffset.y));
    /*
    gf2d_sprite_draw(
        self->sprite,
        self->position,//vector2d(self->position.x + self->drawOffset.x,self->position.y + self->drawOffset.y),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        1
    );*/
    //slog("drawing");
    
    //draw box collider
    gf2d_draw_rect(self->box, vector4d(255,0,255,255));
    // slog("Pos %f.%f", self->position.x, self->position.y);

}

void menu_draw_group(int group)
{
    int i;
    for (i = 0;i < menu_manager.maxMenus;i++)
    {
        if (!menu_manager.menuList[i]._inuse)continue;
        if (menu_manager.menuList[i].group == group)menu_draw(&menu_manager.menuList[i]);
    }
}

Menu *menu_generic(
    int         group,
    SDL_Rect    box,
    Vector2D    drawOffset,
    Sprite      *sprite,
    void        (*think)(struct Menu_S *self)
){
    Menu *menu;
    menu = menu_new();
    menu->group = group;
    menu->box = box;
    //menu->drawOffset = drawOffset;
    //menu->sprite = sprite;
    menu->think = think;

    return menu;
}

void menu_genericV(
    int         group,
    SDL_Rect    box,
    Vector2D    drawOffset,
    Sprite      *sprite,
    void        (*think)(struct Menu_S *self)
){
    Menu *menu;
    menu = menu_new();
    menu->group = group;
    menu->box = box;
    menu->think = think;

    menu->drawOffset = drawOffset;
    if(sprite == NULL)
    {
        menu->sprite = NULL;
    }else{
        menu->sprite = sprite;
    }

}

void createMenus()
{
    //Menu *quit, *remoteVs, *challenge;
    SDL_Rect quitBox, remoteBox, challBox;
    Level *lvl = level_get_active();

    gfc_rect_set(quitBox, lvl->bounds.w/2 - 80, lvl->bounds.h - 60, 150, 60);
    gfc_rect_set(remoteBox, 420, 407, 325, 60);
    gfc_rect_set(challBox, 420, 323, 325, 60);

    menu_genericV(MAIN_MENU,quitBox,vector2d(0,0),NULL,quitThink);
    menu_genericV(MAIN_MENU,remoteBox,vector2d(0,0),NULL,remoteThink);
    menu_genericV(MAIN_MENU,challBox,vector2d(0,0),NULL,quitThink);
 
}

void quitThink(Menu *self)
{
    static int last_level_change = 0;
    int mx,my;
    SDL_GetMouseState(&mx,&my);
    if (collide_menu(self->box, vector2d(mx,my)))
    {
        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) && last_level_change + 750 < SDL_GetTicks())
        {
            last_level_change = SDL_GetTicks();
            level_get_active()->done = 1;
        }
    }
}

void remoteThink(Menu *self)
{
    static int last_level_change = 0;
    int mx,my;
    SDL_GetMouseState(&mx,&my);
    if (collide_menu(self->box, vector2d(mx,my)))
    {
        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) && last_level_change + 750 < SDL_GetTicks())
        {
            last_level_change = SDL_GetTicks();
            level_get_active()->screen = IN_GAME;
            level_get_active()->isLocalCoop = 1;
        }
    }
}

//void button_start_think(Menu *self)
//{
//    static int last_level_change = 0;
//    int mx,my;
//    SDL_GetMouseState(&mx,&my);
//    if (collide_menu(self->box, vector2d(mx,my)))
//    {
//        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) && last_level_change + 750 < SDL_GetTicks())
//        {
//            last_level_change = SDL_GetTicks();
//            free_all_ents();
//            level_spawn_ents(level_get_active());
//            entity_spawn_ambulance(vector2d(10.0,300.0));
//            level_get_active()->paused = 0;
//            level_get_active()->winLose = 0;
//            level_get_active()->timeLeft = 30;
//            get_player_ent()->position = vector2d(150, 300);
//        }
//    }
//}
