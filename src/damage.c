#include "damage.h"
#include "simple_logger.h"
#include "entity.h"
#include "player.h"
#include "collisions.h"


void damage_deal(Entity *parent, Entity *victim)
{
    if ((parent->flag ==  ATK_HEAVY) & (victim->flag != BLOCKING))
    {
        victim->health -= 20;
    }
    if ((parent->flag ==  ATK_LIGHT) & (victim->flag != BLOCKING))
    {
        victim->health -= 10;
    }
}

void damage_collision_check()           //broken
{
    Entity *p1 = get_player_1();
    Entity *p2 = get_player_2();

    if (collide_ent(p1,p2))
    {
        if ((p1->flag == ATK_LIGHT) || (p1->flag == ATK_HEAVY))
        {
            damage_deal(p1,p2);
        }
        if ((p2->flag == ATK_LIGHT) || (p2->flag == ATK_HEAVY))
        {
            damage_deal(p2,p1);
        }
    }
}

//EntityManager entity_manager;
    //entity_manager = entity_manager_get_active();
    //int i;
    //for (i = 0; i < entity_manager.maxEnts; i++)
    //{
    //    if (!entity_manager.entityList[i]._inuse)continue;
//
    //    if (entity_manager.entityList[i]._inuse)
    //    {
    //        if (entity_manager.entityList[i].tag == ENT_PLAYER)
    //        {
    //            if (entity_manager.entityList[i].flag == ATK_LIGHT || entity_manager.entityList[i].flag == ATK_HEAVY)
    //            {
    //                slog("detecting flag");
    //                Entity temp = entity_manager.entityList[i];
    //                int e;
    //                for (e = 0; e < entity_manager.maxEnts; e++)
    //                {
    //                    if ((entity_manager.entityList[e].tag == ENT_PLAYER) & (entity_manager.entityList[e] != entity_manager.entityList[i]))
//                    {
    //                        if (collide_ent(entity_manager.entityList[e],temp))
    //                        {
    //                            slog("detecting contact");
    //                            damage_deal(temp, entity_manager.entityList[e]);
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }
    //}
//


