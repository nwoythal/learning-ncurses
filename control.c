#include <ncurses.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
    void sleep(long time)
    {
        Sleep(time*1000); //windows uses ms to sleep, and defines the function with 'S'
    }
#elif __linux__
    #include <sys/ioctl.h>
#else
    #error "Unrecognized compiler"
#endif

enum {SP_Tree, SP_You, SP_Cave};

void wait()
{
}

void draw(int x, int y, int type)
{
    switch(type) 
    {
        case SP_You:
            mvaddch(y, x, 'o');
            break;
        case SP_Tree:
            mvaddch(y, x, 'φ');
            break;
        case SP_Cave:
            mvaddch(y, x, 'Ħ');
            break;
        default:
            wait();
            break;
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
    char floor[size.ws_row][size.ws_col];
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
            clear();
        else
            refresh();
        draw(x_loc, y_loc, SP_You);
    }
    endwin();
    return 0;
}
