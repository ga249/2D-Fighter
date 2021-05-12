#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gf2d_sprite.h"
#include "gf2d_draw.h"
#include "gfc_list.h"
#include "gf2d_shape.h"
#include "simple_json.h"


#define ENT_PLAYER      0
#define ENT_AI          1

#define ATK_LIGHT       1
#define ATK_HEAVY       2
#define DAMAGED         3
#define BLOCKING        4
#define IDLE            5
#define CHARGING        6
#define ENT_PROJ        7
#define HB_RECT         8
#define HB_CIRCLE       9
#define ENT_BLAST       10
#define BLASTING        11
#define AFTERIMAGE      12




typedef struct Entity_S
{
    Uint8       _inuse;         /**<check if this entity in memory is active or not>*/
    Sprite      *sprite;        /**<a pointer to the sprite that is used by this entity>*/
    float        frame;         /**<current frame for the sprite>*/
    SJson       *attackFrames;
    int         attackDist;
    Vector2D    *offset;        /**<offset of sprite draw>*/
    int         mainY;          /**<main y offset for characters that have altY's for when they're flipped>*/
    int         altY;           /**<alternate y offset for characters that need it>*/
    Vector2D    *scale;         /**<scaleOffset of sprite draw>*/
    Vector2D    *flip;          /**<(flip Horiz,flip Vert)>*/
    int         speed;          /**<Speed at which ent moves>*/
    float       ttd;            /**<time till death(killing) of ent*/

    int         hbType;         /**<defines whether to use rect or circle hb>*/
    SDL_Rect    hitBox;         /**<rect used for collisions>*/
    Circle      hitCircle;      /**<circle used for collisions>*/
    Circle      aiDistLarge;    
    Circle      aiDistMid;
    Circle      aiDistClose;
    float       hbWidth;
    int         tag;            /**<type of entity:player - 0>*/
    int         flag;           /**<flag used for if players are attacking to trigger damage>*/
    struct Entity_S      *parent;        /**<parent entity used for projectiles>*/
    struct Entity_S      *target;        /**<target entity used to identify target>*/

    int         health;
    int         ki;
    int         dmg;
    int         afterImgOn;

    Vector2D     position;      /**<where the entity is in 2D space>*/
    Vector3D     *rotation;     /**<rotation of entity (mainly for player)>*/
    Vector2D     *unitDirection;/**<2d vector that dictates direction*/  

    int         p;              /**<int to identify player 1 and 2>*/

    SDL_GameController *controller; /**<pointer to SDL_GameController (used for players)*/
    struct FrameMapping_S         *frameMapping;    /**<void pointer to a struct that has frame info for each character>*/
    float        superBlastSpeed;
    int          done;
    void (*think)(struct Entity_S *self);       /**<called when an entity draws>*/
}Entity;

typedef struct 
{
    Uint32  maxEnts;         /**<Maximum number of entities*/
    Entity  *entityList;     /**<List of entities*/
}EntityManager;

/**
 * @brief Returns a pointer to the entity_manager
 * */
EntityManager entity_manager_get_active();


/**
 * @brief get a pointer to a new entity
 * @return NULL on out of memory or error, a pointer to a blank entity otherwise
 */
Entity *entity_new();

/**
 * @brief initialize the entity resource manager
 * @param maxEnts upper bound of maximum concurrent entities to be supported
 * @note must be called before creating a new entity
 */
void entity_manager_init(Uint32 maxEnts);

/**
 * @brief free a previously allocated entity
 * @param self a pointer to the entity to free
 */
void entity_free(Entity *self);

/**
 * @brief free all ents exept player
 */
void free_all_ents();

/**
 * @brief update every active entity
 */
void entity_update_all();

/**
 * @brief draw every active entity
 */
void entity_draw_all();

/**
 * @brief draw every active entity's hitbox
 */
void entity_draw_all_hitboxes();

int proj_in_range(Entity *self);

/**
 * @brief Make 2 entities face each other
 * @param ent1 entity 1
 * @param ent2 entity 2
 */
void ent_face_eo(Entity *ent1, Entity *ent2);

void ent_move_toward_ent(Entity *e1, Entity *e2);

/**
 * @brief get a pointer player 1
 * @return pointer to player 1
 */
Entity *get_player_1();

/**
 * @brief get a pointer player 2
 * @return pointer to player 2
 */
Entity *get_player_2();

int is_in_array(SJson *array, int num);

//Entity *doneEnt();
//
//Entity *get_done_ent();

#endif