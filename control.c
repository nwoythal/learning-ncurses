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

void wait() //Do nothing, will be refined in the future.
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

void generate_terrain(int *list, struct winsize mods)
{
    for(int i=0; i<OBJ_COUNT; i+=2)
    {
        *(list+i)=rand()%mods.ws_col;
        *(list+i+1)=rand()%mods.ws_row;
    }
}

void render_terrain(int *list)
{
    for(int i=0; i<OBJ_COUNT; i+=2)
    {
        draw(*(list+i), *(list+i+1), SP_Tree);
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
    int x_loc=size.ws_col/2; //start x
    int y_loc=size.ws_row/2; //start y
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
    }
    
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
                    x_loc-=2;
                break;
            case KEY_RIGHT:
                if((x_loc+1)<size.ws_col)
                    x_loc+=2;
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
            clear();
        else
            refresh();
        draw(x_loc, y_loc, SP_You);
        render_terrain(tree_list);
        //draw(tree_list[0], tree_list[1], SP_Tree);
    }
    endwin();
    return 0;
}
