#include <stdlib.h>
#include "simple_logger.h"
#include "menus.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include "gf2d_draw.h"
#include "collisions.h"
#include "level.h"
#include "simple_json.h"
#include "entity.h"
#include "ai.h"

#define B_CHALLENGE     0
#define B_REMOTE        1
#define B_QUIT          2
#define B_NEXTBAT       3
#define B_REMATCH       4
#define B_MAIN_MENU     5
#define T_P1            6
#define T_P2            7
#define T_P1_WINS       8
#define T_P2_WINS       9
#define T_U_LOSE        10
#define T_U_WIN         11

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
    
    gf2d_sprite_draw(
        self->sprite,
        vector2d(self->box.x,self->box.y),//vector2d(self->position.x + self->drawOffset.x,self->position.y + self->drawOffset.y),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        self->frame
    );
    //slog("drawing");
    
    //draw box collider
    gf2d_draw_rect(self->box, v4d_red);
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
    int         buttonFrame,
    Sprite      *sprite,
    void        (*think)(struct Menu_S *self)
){
    Menu *menu;
    menu = menu_new();
    menu->group = group;
    menu->box = box;
    menu->drawOffset = drawOffset;
    menu->sprite = sprite;
    menu->think = think;

    return menu;
}

void menu_genericV(
    int         group,
    SDL_Rect    box,
    Vector2D    drawOffset,
    int         buttonFrame,
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
        menu->sprite->actionSpec = vector3d(0,0,1);
        menu->frame = buttonFrame;
    }

}

void createMenus()
{
    //Menu *quit, *remoteVs, *challenge;
    SDL_Rect quitBox, remoteBox, challBox, rematchBox, mainMBox, p1winBox, p2winBox, uWLBox, nextBox;
    Level *lvl = level_get_active();
    Sprite  *menus;
    menus = gf2d_sprite_load_all("images/menus/menuButtons.png", 391, 56, 1);
    Vector2D blank = vector2d(0,0);

    //------------------------------------------MAIN_MENU--------------------------------
    gfc_rect_set(quitBox, lvl->bounds.w/2 - 80, lvl->bounds.h - 60, 140, 56);
    gfc_rect_set(remoteBox, 420, 407, 327, 56);
    gfc_rect_set(challBox, 420, 323, 325, 56);
    menu_genericV(MAIN_MENU,quitBox,blank,B_QUIT,menus,quitThink);
    menu_genericV(MAIN_MENU,remoteBox,blank,B_REMOTE,menus,remoteThink);
    menu_genericV(MAIN_MENU,challBox,blank,B_CHALLENGE,menus,challengeThink);
    //------------------------------------------PAUSED/LOCAL_LOSE------------------------
    gfc_rect_set(rematchBox, 450, 323, 275, 56);
    gfc_rect_set(mainMBox, 420, 407, 330, 56);
    menu_genericV(PAUSED,rematchBox,blank,B_REMATCH,menus,rematchThink);
    menu_genericV(PAUSED,mainMBox,blank,B_MAIN_MENU,menus,mainMThink);
    //------------------------------------------P1_WIN-----------------------------------
    gfc_rect_set(p1winBox, 475, 200, 0, 0);
    menu_genericV(P1_WIN,p1winBox,blank,T_P1_WINS,menus,NULL);
    //------------------------------------------P2_WIN-----------------------------------
    gfc_rect_set(p2winBox, 475, 200, 0, 0);
    menu_genericV(P2_WIN,p2winBox,blank,T_P2_WINS,menus,NULL);
    //------------------------------------------U_LOSE------------------------
    gfc_rect_set(uWLBox, 500, 200, 0, 0);
    menu_genericV(U_LOSE,uWLBox,blank,T_U_LOSE,menus,NULL);
    menu_genericV(U_LOSE,mainMBox,blank,B_MAIN_MENU,menus,mainMThink);
    //------------------------------------------U_WIN-------------------------
    gfc_rect_set(uWLBox, 500, 200, 0, 0);
    gfc_rect_set(nextBox, 400, 323, 390, 56);
    menu_genericV(U_WIN,uWLBox,blank,T_U_WIN,menus,NULL);
    menu_genericV(U_WIN,nextBox,blank,B_NEXTBAT,menus,nextThink);
    menu_genericV(U_WIN,mainMBox,blank,B_MAIN_MENU,menus,mainMThink);
}

void quitThink(Menu *self)
{
    //last_level_change = 0;
    int mx,my;
    SDL_GetMouseState(&mx,&my);
    if (collide_menu(self->box, vector2d(mx,my)))
    {
        if ((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && (SDL_GetTicks() - last_level_change >= 200))
        {
            last_level_change = SDL_GetTicks();
            level_get_active()->done = 1;
        }
    }
}

void nextThink(Menu *self)
{
    //if(level_get_active()->paused)
    //{
    //    last_level_change = SDL_GetTicks();
    //    level_get_active()->paused = 0;
    //}
    int mx,my;
    SDL_GetMouseState(&mx,&my);
    if (collide_menu(self->box, vector2d(mx,my)))
    {
        if ((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && (SDL_GetTicks() - last_level_change >= 200))
        {
            last_level_change = SDL_GetTicks();
            level_get_active()->winCount += 1;
            level_get_active()->screen = IN_GAME;
            level_get_active()->isLocalCoop = 0;
            level_get_active()->paused = 0;
            //level_get_active()->p2->think = ai_think;
            level_load_from(level_get_active(), "levels/level.json");
        }
    }
}

void challengeThink(Menu *self)
{
    if(level_get_active()->paused)
    {
        last_level_change = SDL_GetTicks();
        level_get_active()->paused = 0;
    }
    int mx,my;
    SDL_GetMouseState(&mx,&my);
    if (collide_menu(self->box, vector2d(mx,my)))
    {
        if ((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && (SDL_GetTicks() - last_level_change >= 200))
        {
            last_level_change = SDL_GetTicks();
            level_get_active()->winCount = 0;
            level_get_active()->screen = IN_GAME;
            level_get_active()->isLocalCoop = 0;
            level_get_active()->paused = 0;
            level_get_active()->p2->think = ai_think;
            level_load_from(level_get_active(), "levels/level.json");
        }
    }
}

void remoteThink(Menu *self)
{
    if(level_get_active()->paused)
    {
        last_level_change = SDL_GetTicks();
        level_get_active()->paused = 0;
    }
    int mx,my;
    SDL_GetMouseState(&mx,&my);
    if (collide_menu(self->box, vector2d(mx,my)))
    {
        if ((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && (SDL_GetTicks() - last_level_change >= 200))
        {
            last_level_change = SDL_GetTicks();
            level_get_active()->screen = IN_GAME;
            level_get_active()->isLocalCoop = 1;
            level_get_active()->paused = 0;
            level_get_active()->p2->think = playerThink;
            level_load_from(level_get_active(), "levels/level.json");
        }
    }
}

void rematchThink(Menu *self)
{
    //last_level_change = 0;
    int mx,my;
    SDL_GetMouseState(&mx,&my);
    if (collide_menu(self->box, vector2d(mx,my)))
    {
        if ((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && (SDL_GetTicks() - last_level_change >= 200))
        {
            last_level_change = SDL_GetTicks();
            level_get_active()->screen = IN_GAME;
            level_get_active()->isLocalCoop = 1;
            level_get_active()->paused = 0;
            level_load_from(level_get_active(), "levels/level.json");
        }
    }
}

void mainMThink(Menu *self)
{
    //last_level_change = 0;
    int mx,my;
    SDL_GetMouseState(&mx,&my);
    if (collide_menu(self->box, vector2d(mx,my)))
    {
        if ((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && (SDL_GetTicks() - last_level_change >= 200))
        {
            last_level_change = SDL_GetTicks();
            //level_get_active()->paused = 1;
            level_get_active()->screen = MAIN_MENU;
            level_load_from(level_get_active(), "levels/level.json");
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
