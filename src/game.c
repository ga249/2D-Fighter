#include <SDL.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "simple_logger.h"
#include "entity.h"
#include "player.h"
#include "level.h"
#include "damage.h"
#include "menus.h"
#include "gfc_audio.h"
#include "sounds.h"

void draw_ui(SDL_Rect p1Health,SDL_Rect p2Health,SDL_Rect p1Ki,SDL_Rect p2Ki)
{
    gf2d_draw_rect(p1Health, v4d_red);
    gf2d_draw_rect(p2Health, v4d_red);
    gf2d_draw_rect(p1Ki, v4d_blue);
    gf2d_draw_rect(p2Ki, v4d_blue);
}

//void sound_set_backround(Sound *sound)
//{
//    //Mix_HaltChannel(1);
//    gfc_sound_play(sound, -1, .1, 1, -1);
//}

int main(int argc, char * argv[])
{
    /*variable declarations*/
    float atkBuffer;
    float idleResetBuffer;
    int toggleHb = 0;
    const Uint8 * keys;
    Sprite *bg;
    Entity *player1;
    Entity *player2;
    Level *lvl;
    SDL_Rect p1Health;
    SDL_Rect p2Health;
    SDL_Rect p1Ki;
    SDL_Rect p2Ki;
    //Sound *chaLa;  //--------------------------------------sounds

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
    fmap_manager_init(100);
    entity_manager_init(1084);
    gf2d_sprite_init(1024);
    menu_manager_init(1024);
    //sounds_init();    //--------------------------------------sounds
    SDL_ShowCursor(SDL_DISABLE);
    
    /*demo setup*/
    bg = gf2d_sprite_load_image("images/backgrounds/namek.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16);
    mouse->actionSpec = vector3d(0,0,16);
    player1 = spawnPlayer(vector2d(360,300), 0, "piccolo");
    player2 = spawnPlayer(vector2d(840,300), 1, "goku");
    lvl = level_new(bg, player1, player2);
    lvl->screen = MAIN_MENU;
    player1->target = player2;
    player2->target = player1;
    gfc_rect_set(p1Health, 10, 10, 500, 20);
    gfc_rect_set(p2Health, lvl->bounds.w - 510, 10, 500, 20);
    gfc_rect_set(p1Ki, 10, 35, 500, 20);
    gfc_rect_set(p2Ki, lvl->bounds.w - 510, 35, 500, 20);
    
    createMenus();
    //play_menu_music();   //--------------------------------------sounds

    /*main game loop*/
    while(!lvl->done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        //slog("%i, %i", mx, my);

        p1Health.w = player1->health;
        p2Health.w = player2->health;
        p2Health.x = lvl->bounds.w - p2Health.w - 10;
        p1Ki.w = player1->ki;
        if(player1->ki <= 0)p1Ki.w = 1;
        p2Ki.w = player2->ki;
        if(player2->ki <= 0)p2Ki.w = 1;
        p2Ki.x = lvl->bounds.w - p2Ki.w - 10;


        level_update(lvl);
        
        switch (lvl->screen)
        {
        case IN_GAME:
            if (!lvl->paused)
            {
                entity_update_all();
            }else{
                menu_update_group(PAUSED);
            }
            break;

        case MAIN_MENU:
            menu_update_group(MAIN_MENU);
            break;
        case P1_WIN:
            menu_update_group(PAUSED);
            break;
        case P2_WIN:
            menu_update_group(PAUSED);
            break;
        case U_LOSE:
            menu_update_group(U_LOSE);
            break;
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
            switch (lvl->screen)
            {
            case IN_GAME:
                if (!lvl->paused)
                {
                    entity_draw_all();
                    if(toggleHb)entity_draw_all_hitboxes();
                }else{
                    menu_draw_group(PAUSED);
                    gf2d_sprite_draw(mouse,vector2d(mx,my),NULL,NULL,NULL,NULL,&mouseColor,(int)mf);
                    }
                break;

            case MAIN_MENU:
                menu_draw_group(MAIN_MENU);
                gf2d_sprite_draw(mouse,vector2d(mx,my),NULL,NULL,NULL,NULL,&mouseColor,(int)mf);
                break;
            case P1_WIN:
                menu_draw_group(PAUSED);
                menu_draw_group(P1_WIN);
                gf2d_sprite_draw(mouse,vector2d(mx,my),NULL,NULL,NULL,NULL,&mouseColor,(int)mf);
                break;
            case P2_WIN:
                menu_draw_group(PAUSED);
                menu_draw_group(P2_WIN);
                gf2d_sprite_draw(mouse,vector2d(mx,my),NULL,NULL,NULL,NULL,&mouseColor,(int)mf);
                break;
            case U_LOSE:
                menu_draw_group(U_LOSE);
                gf2d_sprite_draw(mouse,vector2d(mx,my),NULL,NULL,NULL,NULL,&mouseColor,(int)mf);
                break;
            }
            
            //UI elements last
            if (!lvl->paused && lvl->screen == IN_GAME)
            {
                draw_ui(p1Health,p2Health,p1Ki,p2Ki);
            }

            

        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame

        ent_face_eo(player1,player2);

        if((player1->health <= 0))
        {
            if(!lvl->isLocalCoop)
            {
                lvl->screen = U_LOSE;
                lvl->paused = 1;
            }else if (lvl->isLocalCoop)
            {
                lvl->screen = P2_WIN;
                lvl->paused = 1;
            }
        }else if (player2->health <= 0)
        {
            if(!lvl->isLocalCoop)
            {
                lvl->screen = P1_WIN;
                lvl->paused = 1;
                //TODO: make it move on to next challenger
            }else if (lvl->isLocalCoop)
            {
                lvl->screen = P1_WIN;
                lvl->paused = 1;
            }
        }
        
        if (keys[SDL_SCANCODE_F4])
        {
            player_load(player1, "characters/characters.json", "goku");
        }else if (keys[SDL_SCANCODE_F5])
        {
            player_load(player1, "characters/characters.json", "piccolo");
        }

        if (keys[SDL_SCANCODE_B])
        {
            if (SDL_GetTicks() - atkBuffer >= 200)
            {
                atkBuffer = SDL_GetTicks();
                if (toggleHb)toggleHb = 0;else toggleHb = 1;
            }
            
        }

        if (keys[SDL_SCANCODE_V])
        {
            player1->health -= .5;
        }

        //slog("rotation: %f", rot->z);
        if (keys[SDL_SCANCODE_ESCAPE])lvl->done = 1; // exit condition
        if (SDL_GameControllerGetButton(player1->controller,SDL_CONTROLLER_BUTTON_START) || SDL_GameControllerGetButton(player2->controller,SDL_CONTROLLER_BUTTON_START))lvl->paused = 1;
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    level_free(lvl);
    slog("level freed");
    return 0;
}
/*eol@eof*/
