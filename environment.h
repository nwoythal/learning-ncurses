#include <ncurses.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define OBJ_COUNT 256

#ifdef _WIN32 //Portability
    #include <windows.h> 
    void sleep(long time)
    {
        Sleep(time*1000); //windows uses ms to sleep
    }
#elif __linux__
    #include <sys/ioctl.h>
#else
    #error "Unrecognized compiler"
#endif


/*****************************************************************
******************************************************************
***
***                 VARIABLES AND DATA
***
******************************************************************
*****************************************************************/


//List of objects
enum {SP_Tree, SP_You, SP_Cave, SP_Stalag, SP_Pit, SP_Mob};

//Object information, hp, mana, ATK/DEF, and location
struct character
{
    int hp_max;
    int hp_now;
    int mp_max;
    int mp_now;
    int damage;
    int armor;
    int x_loc;
    int y_loc;
}player={30, 30, 15, 15, 3, 0, 0, 0};

int level=1; //Global level var

int cave_x, cave_y;


/*****************************************************************
******************************************************************
***
***                   FUNCTION PROTOTYPES
***
******************************************************************
*****************************************************************/


int randi(int ceiling);
void render_stats(struct winsize size);
void draw(int x, int y, int type);
void generate_terrain(int *list, struct winsize mods);
void render_terrain(int *list, int pallet);
void wait();
void combat(struct character* attacker, struct character* defender);
void get_path();

/*****************************************************************
******************************************************************
***
***                      WINDOW RENDERING 
***
******************************************************************
*****************************************************************/


//Redefine random int function, because terrain generation was wonky 
int randi(int ceiling)
{
    int divisor=RAND_MAX/(ceiling+1);
    int rand_int;
    do
    {
        rand_int=rand()/divisor;
    }while(rand_int>ceiling);
    return rand_int;
}


/*****************************************************************
******************************************************************
***
***                      WINDOW RENDERING
***
******************************************************************
*****************************************************************/


//Render player info
void render_stats(struct winsize size)
{
    //divide window
    for(int i=0; i<size.ws_row; i++)
    {
        mvaddch(i, size.ws_col, '|');
    }
    mvprintw(1, size.ws_col+3, "Player");
    mvprintw(2, size.ws_col+3, "HP:  %d/%d", player.hp_now, player.hp_max);
    mvprintw(3, size.ws_col+3, "MP:  %d/%d", player.mp_now, player.mp_max);
    mvprintw(4, size.ws_col+3, "ATK: %d", player.damage);
    mvprintw(5, size.ws_col+3, "DEF: %d", player.armor);
    mvprintw(0, size.ws_col+11, "Floor %d", level);
}

//Put object to screen
void draw(int x, int y, int type)
{
    switch(type) 
    {
        case SP_You:
            mvaddch(y, x, 'i');
            break;
        case SP_Tree:
            mvaddch(y, x, 'P');
            break;
        case SP_Cave:
            mvaddch(y, x, '@');
            break;
        case SP_Stalag:
            mvaddch(y, x, '^');
            break;
        case SP_Pit:
            mvaddch(y, x, '#');
            break;
        case SP_Mob:
            mvaddch(y, x, '&');
            break;
        default:
            wait();
            break;
    }
}

//Create trees randomly on terrain.
//Will update in the future with more features
void generate_terrain(int *list, struct winsize mods)
{
    for(int i=0; i<OBJ_COUNT; i+=2)
    {
        *(list+i)=randi(mods.ws_col);
        *(list+i+1)=randi(mods.ws_row);
        if(*(list+i)==cave_x && *(list+i+1)==cave_y)
            *(list+i) = *(list+i+1) = 0;
    }
}

//Draw level obstacles, in color
void render_terrain(int *list, int pallet)
{
    //Pallet definitions
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    pallet = (pallet==3) ? 2 : pallet;
    pallet = (pallet==5) ? 4 : pallet;
    int sprite=SP_Tree;
    if(pallet > 1 && pallet < 4)
    {
        sprite=SP_Stalag;
    }
    else if(pallet > 3 && pallet < 6)
    {
        sprite=SP_Pit;
    } 
    attron(COLOR_PAIR(pallet));
    for(int i=0; i<OBJ_COUNT; i+=2)
    {
        draw(*(list+i), *(list+i+1), sprite);
    }
    attroff(COLOR_PAIR(pallet));
}


/*****************************************************************
******************************************************************
***
***                    SPRITE FUNCTIONS
***
******************************************************************
*****************************************************************/


void wait() //Do nothing. Needs fixing for combat.
{
}

//Subtract ATK from HP, needs refining with DEF in the future
void combat(struct character* attacker, struct character* defender)
{
    attacker->hp_now-=defender->damage;
    defender->hp_now-=attacker->damage;
    if(defender->hp_now<=0)
    {
        defender->x_loc=-1;
        defender->y_loc=-1;
    }
}

/*
What is the optimal pathfinding algorithm for a moving start & endpoint?
*/
void get_path()
{

}
