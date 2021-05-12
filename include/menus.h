#ifndef _MENU_H_
#define _MENU_H_

#include <stdlib.h>
#include "gf2d_sprite.h"
#include "collisions.h"
#include "gf2d_graphics.h"
#include <SDL_ttf.h>

#define MAIN_MENU              0
#define CHARACTER_SELECT       1
#define IN_GAME                2
#define P1_WIN                 3
#define P2_WIN                 4
#define U_LOSE                 5
#define PAUSED                 6
#define U_WIN                  7

float last_level_change;

typedef struct Menu_S
{
    Uint8       _inuse;             /**<Check if entity in memory is active or not*/
    Sprite      *sprite;            /**<A pointer to the sprite*/
    //int         collider_shape;     /**<Indicates whether collider is cirlular or rectangular*/
    float       frame;              /**<Current frame of sripte*/
    // float       maxFrames;          /**<Maximum number of frames in a sprite*/
    Vector2D    position;           /**<2D position of entity*/
    Vector2D    drawOffset;         /**<Offset of collider*/
    SDL_Rect    box;                /**<Bounds of menu*/
    int         group;
    //SDL_Texture *Message;


    void        (*think)(struct Menu_S *self);

}Menu;

/**
 * @brief get a pointer to a new entity
 * @return NULL on out of memory or error, a pointer to a blank entity otherwise
 */
Menu *menu_new();

/**
 * @brief initialize entity resource manager
 * @param maxEnts maximum entities
 * */
void menu_manager_init(Uint32 maxEnts);

/**
 * @brief Close entity system
 */
void menu_manager_close();

/**
 * @brief free previously allocated entity
 * @param self a pointer to the entity to free
 */
void menu_free(Menu *menu);

/**
 * @brief Update frame info of entity
 * @param self a pointer to the entity to update
 */
void menu_update(Menu *self);

/**
 * @brief update every active entity
 */
void menu_update_group(int group);

/**
 * @brief draw every active entity
 */
void menu_draw_group(int group);

/**
 * @brief For each entity, check all other entities for collision
 * */
void menu_touch_check(Menu *menu);

Menu *menu_generic(
    int         group,
    SDL_Rect    box,
    Vector2D    drawOffset,
    int         buttonFrame,
    Sprite      *sprite,
    void        (*think)(struct Menu_S *self)
);

void menu_genericV(
    int         group,
    SDL_Rect    box,
    Vector2D    drawOffset,
    int         buttonFrame,
    Sprite      *sprite,
    void        (*think)(struct Menu_S *self)
);

void createMenus();

void quitThink(Menu *self);

void nextThink(Menu *self);

void challengeThink(Menu *self);

void remoteThink(Menu *self);

void rematchThink(Menu *self);

void mainMThink(Menu *self);



#endif