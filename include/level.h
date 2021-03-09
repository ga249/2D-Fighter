#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "entity.h"
#include "simple_json.h"
#include "simple_logger.h"
#include "player.h"
#include "collisions.h"


#define LEVEL_WIDTH 1200
#define LEVEL_HEIGHT 720

Vector4D v4d_red;
Vector4D v4d_green;
Vector4D v4d_blue;
Vector4D v4d_yellow;

Sprite *background;

typedef struct
{
    Sprite     *background;     /**<Background sprite>*/
    int         frame;          /**<Frame ++ on update>*/
    SDL_Rect    bounds;         /**<Level bounds to prevent leaving stage>*/
    int         done;           /**<Done condition>*/
    int         paused;
    SJson       *config;
    SJson       *save;
    int         winLose;
    int         isLocalCoop;    /**<bool value, 0 = vs computer, 1 = coop>*/
}Level;

/**
 * @brief Creates level with bg and bounds
 * @param backgroundFile Path to background file
 * @param bounds Level bounds
 * @return Object type of Level
 * */
Level *level_new(Sprite *backgroundFile);

/**
 * @brief Frees level and its sprite
 * */
void level_free(Level *level);

/**
 * @brief Draw level bg image and bounds
 * */
void level_draw(Level *level);

void level_spawn_ents(Level *level);

/**
 * @brief Get current level object
 * @return Static type Level
 * */
Level *level_get_active();

/**
 * @brief Update collisions for ents in level
 * */
void level_update_bounds_clip(Level *lvl);

/**
 * @brief Update things in level, pause etc.
 * */
void level_update(Level *lvl);

///**
// * @brief test if a rect is touching or exceeding the level bounds
// * @return 1 if anything exceeds level bounds
// */
//int is_rect_out_bounds();

#endif