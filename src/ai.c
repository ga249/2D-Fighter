#include "ai.h"

#define CLOSE       0
#define MID         1
#define LARGE       2
#define FAR         3

const Uint8 * keys;
float thinkBuffer;
float doBuffer;
float blockBuffer;
int   choice;
int   choiceDefence;
int   choiceProj;
int   choiceSuper;
float aiAfterImgTimer = 0;
float afterImgBuffer;

int retreatNCharge = 0;

void ai_think(Entity *self)
{
    keys = SDL_GetKeyboardState(NULL);
    self->speed = SPEED_BASE;
    
    //update hitboxes
    self->hitCircle.x = self->position.x;
    self->hitCircle.y = self->position.y;
    self->aiDistLarge.x = self->position.x;
    self->aiDistLarge.y = self->position.y;
    self->aiDistMid.x = self->position.x;
    self->aiDistMid.y = self->position.y;
    self->aiDistClose.x = self->position.x;
    self->aiDistClose.y = self->position.y;

    if (self->flag != IDLE)
    {
        if((self->flag != DAMAGED) & (self->flag != BLASTING) & (self->flag != ATK_LIGHT))
        {
            self->flag = IDLE;
        }
    }
    if (self->flag == BLASTING)
    {
        self->speed = 0;
    }
    if (self->flag == IDLE)
    {
        self->frame = self->frameMapping->idle;
        self->speed = 2;
    }
    if (self->flag == DAMAGED)
    {
        self->frame = self->frameMapping->hit;
    }

    if (proj_in_range(self))
    {
        if (SDL_GetTicks() - blockBuffer >= 500)
        {
            blockBuffer = SDL_GetTicks();
            
            if(self->ki > 50)
            {
                choiceDefence = rand() % 3;
            }else{
                choiceDefence = rand() % 2;
            }
        }
        if (choiceDefence == 1)block(self);
        if (choiceDefence == 2)self->afterImgOn = 1;;
    }

    if (self->afterImgOn)
    {
        aiAfterImgTimer += 1;
        self->frame = self->frameMapping->afterImage;
        if (proj_in_range(self))
        {
            afterImgSide(self);
            self->ki -= 50;
            self->afterImgOn = 0;
            afterImgBuffer = SDL_GetTicks();
        }else if (collide_ent(self, self->target) && self->target->flag == ATK_LIGHT)
        {
            afterImgAway(self);
            self->ki -= 50;
            self->afterImgOn = 0;
            afterImgBuffer = SDL_GetTicks();
        }else if (aiAfterImgTimer >= 100)
        {
            self->afterImgOn = 0;
            aiAfterImgTimer = 0;
            afterImgBuffer = SDL_GetTicks();
        }
    }

    if (self->ki <= 100)retreatNCharge = 1;

    if (retreatNCharge)
    {
        if (check_dist(self) != FAR)
        {
            if (self->ki > 2)
            {
                self->speed = SPEED_DASH;
                self->ki -= 2;
            }else{self->speed = SPEED_BASE;}
            retreat(self);
        }else{
            charge(self);
        }
        if(self->ki == 350)retreatNCharge = 0;
    }

//---------------------------------------------------------------------------Think pause and hold cycle----------------
    if (SDL_GetTicks() - thinkBuffer >= 400)
    {
        if (SDL_GetTicks() - doBuffer >= 500)
        {
            thinkBuffer = SDL_GetTicks();
            //slog("%i", choice);
        //--------------------------------------decision making logic-------------------------------
            switch (check_dist(self))
            {
            case CLOSE:
                if (self->target->flag == ATK_LIGHT)
                {
                    choice = rand() % 2;
                }else{
                    choice = 2;
                }
                break;

            case MID:
                choiceSuper = rand() % 4;
                choice = rand() % 4;
                break;

            case LARGE:
                choice = rand() % 3;
                choiceProj = rand() % 3;
                break;

            case FAR:
                if (self->ki <= 150)
                {
                    choice = 1;
                }else{
                    choice = 0;
                }
                break;
            }//---------------------------------decision making logic-------------------------------
        }
//---------------------------------------------------------------------------Think pause and hold cycle----------------
        
        switch (check_dist(self))
        {
        case CLOSE:
            switch (choice)
            {
            case 0:
                block(self);
                break;
            
            case 1:
                retreat(self);
                break;
            case 2:
                melee_attack(self);
                break;
            }
            break;
        
        case MID:
            if (choiceSuper == 0)super_blast(self);

            switch (choice)
            {
            case 0:
                get_closer(self);
                break;
            
            case 1:
                move_up(self);
                break;
            case 2:
                move_down(self);
                break;
            case 3:
                retreat(self);
                break;
            }

            break;

        case LARGE:
            switch (choice)
            {
            case 0:
                get_closer(self);
                break;
            
            case 1:
                move_up(self);
                break;
            case 2:
                move_down(self);
                break;
            }

            if (choiceProj == 0)ki_blast(self);

            break;

        case FAR:
            switch (choice)
            {
            case 0:
                get_closer(self);
                break;
            
            case 1:
                charge(self);
                break;
            }
            break;
        }

        //if (!collide_ent_circle(self->target,self->aiDistLarge))
        //{
        //    if (self->ki <= 50)
        //    {
        //        charge(self);
        //    }else{
        //        get_closer(self);
        //    }
        //}else{
        //    int dir = rand() % 4;
        //    slog("%i", dir);
        //}



//---------------------------------Think pause and hold cycle----------------
    }else{doBuffer = SDL_GetTicks();}
//---------------------------------Think pause and hold cycle----------------

//----------------------------------------DEBUGGING--------------------------
    if (keys[SDL_SCANCODE_SPACE])
    {
        self->health -= 3;
    }

}