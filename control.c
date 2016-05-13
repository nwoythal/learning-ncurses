#include <ncurses.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

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

//Create trees randomly on terrain. Needs fixing.
void generate_terrain(int *list, struct winsize mods)
{
    for(int i=0; i<OBJ_COUNT; i+=2)
    {
        *(list+i)=rand()%mods.ws_col;
        *(list+i+1)=rand()%mods.ws_row;
    }
}

//Draw every tree
void render_terrain(int *list)
{
    for(int i=0; i<OBJ_COUNT; i+=2)
    {
        draw(*(list+i), *(list+i+1), SP_Tree);
    }
}

//Render player info
void render_stats(struct winsize size)
{
    for(int i=0; i<size.ws_row; i++)
    {
        mvaddch(i, size.ws_col+1, '|');
    }
}

int main(int argc, char* argv[])
{
    //variable declarations
    initscr();
    raw();
    curs_set(0);
    struct winsize size;
    ioctl(0, TIOCGWINSZ, &size);
    int tree_list[OBJ_COUNT]; //Tree linked list, even numbers are x-coord, odds are y-coord
    keypad(stdscr, TRUE);
    unsigned short input='\0'; //Store in int because KEY_values are large, apparently.
    char mode='\0'; //only supports 'e' for now.

    //Check if etch a sketch argument was passed.
    if(argc>1 && argv[1][1]=='e')
    {
        curs_set(2);
        printw("Starting in etch-a-sketch mode.");
        refresh();
        sleep(2);
        clear();
        mode='e';
    }
    else
    {
        generate_terrain(tree_list, size);
        size.ws_col=size.ws_col*2/3; //Reserve 1/3 of the window for other garbage
    }
    int x_loc=size.ws_col/2; //start x
    int y_loc=size.ws_row/2; //start y
    
    draw(x_loc, y_loc, SP_You);
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
            render_terrain(tree_list);
            render_stats(size);
        }
        else
        {
            refresh();
        }
        draw(x_loc, y_loc, SP_You);
        //draw(tree_list[0], tree_list[1], SP_Tree);
    }
    endwin();
    return 0;
}
