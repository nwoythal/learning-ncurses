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
int level=1;

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

//Create trees randomly on terrain.
//Will update in the future with more features
void generate_terrain(int *list, struct winsize mods)
{
    for(int i=0; i<OBJ_COUNT; i+=2)
    {
        *(list+i)=randi(mods.ws_col);
        *(list+i+1)=randi(mods.ws_row);
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


/*
What is the optimal pathfinding algorithm for a moving start & endpoint?
*/
void get_path()
{

}

int main(int argc, char* argv[])
{
    //variable declarations
    initscr();
    if(!has_colors())
    {
        printf("Your terminal does not support colors. Exiting...\n");
        endwin();
        exit(1);
    }
    start_color();
    raw();
    curs_set(0);
    struct winsize size;
    ioctl(0, TIOCGWINSZ, &size);
    int tree_list[OBJ_COUNT]; //Obstacle storage, even numbers are x-coord, odds are y-coord
    keypad(stdscr, TRUE);
    unsigned short input='\0'; //Store in short because KEY_values are large, apparently.
    char mode='\0'; //only supports 'e' for now.
    int cave_x;
    int cave_y;

    //Check if etch a sketch argument was passed.
    if(argc>1 && argv[1][1]=='e')
    {
        curs_set(2);
        printw("Starting in etch-a-sketch mode...");
        refresh();
        sleep(2);
        clear();
        mode='e';
    }
    else
    {
        size.ws_col=size.ws_col*2/3; //Reserve 1/3 of the window for stats 
        time_t t;
        srand((unsigned) time(&t)); //Set seed so terrain is less consistent
        generate_terrain(tree_list, size);
        cave_x=randi(size.ws_col);
        cave_y=randi(size.ws_row);
        
    }
    player.x_loc=size.ws_col/2; //start x
    player.y_loc=size.ws_row/2; //start y
    struct character monster={10, 10, 0, 0, 1, 0, randi(size.ws_col), randi(size.ws_row)};
    
    draw(player.x_loc, player.y_loc, SP_You);
    draw(monster.x_loc, monster.y_loc, SP_Mob);
    render_stats(size);
    //Movement loop, exits on ESC press.
    while(input!=27)
    {
        //Movement
        input=getch();
        switch(input)
        {
            case KEY_UP:
                if((player.y_loc-1)>=0)
                {
                    if(player.y_loc-1==monster.y_loc && player.x_loc==monster.x_loc)
                        combat(&player, &monster);
                    else
                        player.y_loc--;
                }
                break;
            case KEY_DOWN:
                if((player.y_loc+1)<size.ws_row)
                {
                    if(player.y_loc+1==monster.y_loc && player.x_loc==monster.x_loc)
                        combat(&player, &monster);
                    else
                        player.y_loc++;
                }
                break;
            case KEY_LEFT:
                if((player.x_loc-1)>=0)
                {
                    if(player.x_loc-1==monster.x_loc && player.y_loc==monster.y_loc)
                        combat(&player, &monster);
                    else
                        player.x_loc--;
                }
                break;
            case KEY_RIGHT:
                if((player.x_loc+1)<size.ws_col)
                {
                    if(player.x_loc+1==monster.x_loc && player.y_loc==monster.y_loc)
                        combat(&player, &monster);
                    else
                        player.x_loc++;
                }
                break;
            case 'c':
                if(mode=='e')
                    clear();
                break;
            default:
                wait();
                break;
        }
        //Check for PK, if you're dead, GAME OVER
        if(player.hp_now<=0)
        {
            clear();
            player.hp_max=0;
            player.hp_now=0;
            player.mp_max=0;
            player.mp_now=0;
            player.damage=0;
            player.armor =0;
            mvprintw(size.ws_row/2, (size.ws_col/2)-4, "GAME OVER.");
            render_stats(size);
            input=getch();
            endwin();
            exit(0);
        }
        if(mode!='e')
        {
            clear();
            //Is the player in the cave?
            if(player.y_loc==cave_y && player.x_loc==cave_x)
            {
                player.x_loc=size.ws_col/2; //reset x
                player.y_loc=size.ws_row/2; //reset y
                level++; //Increment level to change pallet
                cave_x=randi(size.ws_col);
                cave_y=randi(size.ws_row);
                generate_terrain(tree_list, size);
            }
            else
            {
                render_terrain(tree_list, level);
                if(level<=5)
                    draw(cave_x, cave_y, SP_Cave);
                if(monster.hp_now>0)
                    draw(monster.x_loc, monster.y_loc, SP_Mob);
            }
            render_stats(size);
        }
        else
        {
            refresh();
        }
        draw(player.x_loc, player.y_loc, SP_You); //Put character to screen
    }
    endwin();
    return 0;
}
