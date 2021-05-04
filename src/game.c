#include <SDL.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "simple_logger.h"
#include "entity.h"
#include "player.h"
#include "level.h"
#include "damage.h"

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    float atkBuffer;
    float idleResetBuffer;
    const Uint8 * keys;
    Sprite *bg;
    Entity *player1;
    Entity *player2;
    Level *lvl;
    SDL_Rect p1Health;
    SDL_Rect p2Health;
    SDL_Rect p1Ki;
    SDL_Rect p2Ki;

    int mx,my;
    float mf = 0;
    Sprite *mouse;
    Vector4D mouseColor = {255,100,255,200};
    
    /*program initializtion*/
    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1200,
        720,
        1200,
        720,
        vector4d(0,0,0,255),
        0);
    gf2d_graphics_set_frame_delay(16);
    fmap_manager_init(1084);
    entity_manager_init(1084);
    gf2d_sprite_init(1024);
    SDL_ShowCursor(SDL_DISABLE);
    
    /*demo setup*/
    bg = gf2d_sprite_load_image("images/backgrounds/namek.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16);
    mouse->actionSpec = vector3d(0,0,16);
    player1 = spawnPlayer(vector2d(360,300), 0, "piccolo");
    player2 = spawnPlayer(vector2d(840,300), 1, "goku");
    lvl = level_new(bg, player1, player2);
    player1->target = player2;
    player2->target = player1;
    gfc_rect_set(p1Health, 10, 10, 500, 20);
    gfc_rect_set(p2Health, lvl->bounds.w - 510, 10, 500, 20);
    gfc_rect_set(p1Ki, 10, 35, 500, 20);
    gfc_rect_set(p2Ki, lvl->bounds.w - 510, 35, 500, 20);

    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;

        p1Health.w = player1->health;
        p2Health.w = player2->health;
        p2Health.x = lvl->bounds.w - p2Health.w - 10;
        p1Ki.w = player1->ki;
        if(player1->ki <= 0)p1Ki.w = 1;
        p2Ki.w = player2->ki;
        if(player2->ki <= 0)p2Ki.w = 1;
        p2Ki.x = lvl->bounds.w - p2Ki.w - 10;


        level_update(lvl);
        
        if (!lvl->paused)
        {
            entity_update_all();
        }
        
        //DAMANGE_COLLISION_CHECK--------------------------------------------------
        if (collide_ent(player1,player2))
        {
            if ((player1->flag == ATK_LIGHT) || (player1->flag == ATK_HEAVY))
            {
                if (SDL_GetTicks() - atkBuffer >= 200)
                {
                    atkBuffer = SDL_GetTicks();
                    damage_deal(player1,player2);
                }
            }else
            if ((player2->flag == ATK_LIGHT) || (player2->flag == ATK_HEAVY))
            {
                if (SDL_GetTicks() - atkBuffer >= 200)
                {
                    atkBuffer = SDL_GetTicks();
                    damage_deal(player2,player1);
                }
            }
        }
        if (SDL_GetTicks() - idleResetBuffer >= 300)
        {
            idleResetBuffer = SDL_GetTicks();
            if((player1->flag != ATK_LIGHT))player1->flag = IDLE;
            if((player2->flag != ATK_LIGHT))player2->flag = IDLE;
        }
        //---------------------------------------------------------------------------
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            level_draw(lvl);

            //drawing entitys
            if (!lvl->paused)
            {
                entity_draw_all();
            }else{
                //TODO: draw menus
            }
            
            entity_draw_all_hitboxes();
            
            //UI elements last
            gf2d_draw_rect(p1Health, v4d_red);
            gf2d_draw_rect(p2Health, v4d_red);
            gf2d_draw_rect(p1Ki, v4d_blue);
            gf2d_draw_rect(p2Ki, v4d_blue);

            gf2d_sprite_draw(
                mouse,
                vector2d(mx,my),
                NULL,
                NULL,
                NULL,
                NULL,
                &mouseColor,
                (int)mf);

        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame

        ent_face_eo(player1,player2);

        if((player1->health <= 0) || (player2->health <= 0))
        {
            done = 1;
        }
        
        //slog("rotation: %f", rot->z);
        if (keys[SDL_SCANCODE_ESCAPE] || SDL_GameControllerGetButton(player1->controller,SDL_CONTROLLER_BUTTON_START))done = 1; // exit condition
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    level_free(lvl);
    slog("level freed");
    return 0;
}
/*eol@eof*/
