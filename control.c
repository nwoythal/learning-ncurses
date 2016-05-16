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

enum {SP_Tree, SP_You, SP_Cave};
struct character
{
    int hp_max;
    int hp_now;
    int mp_max;
    int mp_now;
    int damage;
    int armor;
}player={30, 30, 15, 15, 3, 0};

void wait() //Do nothing. Needs fixing.
{
}

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
        default:
            wait();
            break;
    }
}


//Redefine random int function, because terrain generation was finnicky
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

//Create trees randomly on terrain. Needs fixing.
void generate_terrain(int *list, struct winsize mods)
{
    for(int i=0; i<OBJ_COUNT; i+=2)
    {
        *(list+i)=randi(mods.ws_col);
        *(list+i+1)=randi(mods.ws_row);
    }
}

//Draw every tree
void render_terrain(int *list, int pallet)
{
    //Pallet definitions
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    pallet = (pallet==3) ? 2 : pallet;
    pallet = (pallet==5) ? 4 : pallet;

    attron(COLOR_PAIR(pallet));
    for(int i=0; i<OBJ_COUNT; i+=2)
    {
        draw(*(list+i), *(list+i+1), SP_Tree);
    }
    attroff(COLOR_PAIR(pallet));
}

//Render player info
void render_stats(struct winsize size)
{
    //divide window
    for(int i=0; i<size.ws_row; i++)
    {
        mvaddch(i, size.ws_col+1, '|');
    }
    mvprintw(1, size.ws_col+3, "Player");
    mvprintw(2, size.ws_col+3, "HP:  %d/%d", player.hp_now, player.hp_max);
    mvprintw(3, size.ws_col+3, "MP:  %d/%d", player.mp_now, player.mp_max);
    mvprintw(4, size.ws_col+3, "ATK: %d",player.damage);
    mvprintw(5, size.ws_col+3, "DEF: %d",player.armor);
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
    int tree_list[OBJ_COUNT]; //Tree linked list, even numbers are x-coord, odds are y-coord
    keypad(stdscr, TRUE);
    unsigned short input='\0'; //Store in int because KEY_values are large, apparently.
    char mode='\0'; //only supports 'e' for now.
    int cave_x;
    int cave_y;
    int level=1;

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
    int x_loc=size.ws_col/2; //start x
    int y_loc=size.ws_row/2; //start y
    
    draw(x_loc, y_loc, SP_You);
    render_stats(size);
    //Movement loop, exits on ESC press.
    while(input!=27)
    {
        //Movement
        input=getch();
        switch(input)
        {
            case KEY_UP:
                if((y_loc-1)>=0)
                    y_loc--;
                break;
            case KEY_DOWN:
                if((y_loc+1)<size.ws_row)
                    y_loc++;
                break;
            case KEY_LEFT:
                if((x_loc-1)>=0)
                    x_loc--;
                break;
            case KEY_RIGHT:
                if((x_loc+1)<size.ws_col)
                    x_loc++;
                break;
            case 'c':
                if(mode=='e')
                    clear();
                break;
            default:
                wait();
                break;
        }
        if(mode!='e')
        {
            clear();
            //Check for collisions
            if(y_loc==cave_y && x_loc==cave_x)
            {
                x_loc=size.ws_col/2; //reset x
                y_loc=size.ws_row/2; //reset y
                level++; //Increment level to change pallet
                cave_x=randi(size.ws_col);
                cave_y=randi(size.ws_row);
                generate_terrain(tree_list, size);
                //memset(tree_list, 0, OBJ_COUNT*sizeof(int)); //Remove terrain
            }
            else
            {
                render_terrain(tree_list, level);
                draw(cave_x, cave_y, SP_Cave);
            }
            render_stats(size);
        }
        else
        {
            refresh();
        }
        draw(x_loc, y_loc, SP_You);
    }
    endwin();
    return 0;
}
