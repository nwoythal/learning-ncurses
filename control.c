#include <ncurses.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

void wait()
{
}

int main(int argc, char* argv[])
{
    //variable declarations
    initscr();
    raw();
    struct winsize size;
    ioctl(0, TIOCGWINSZ, &size);
    int x_loc=size.ws_col/2; //start x
    int y_loc=size.ws_row/2; //start y
    char floor[size.ws_row][size.ws_col];
    keypad(stdscr, TRUE);
    char you='o';
    unsigned short input='\0'; //Store in int because KEY_values are large, apparently.
    char mode='\0';
    if(strcmp(argv[1], "-e"))
    {
        printw("Starting in etch-a-sketch mode.");
        sleep(50000);
        clear();
        mode='e';
    }

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
        mvaddch(y_loc,x_loc,you);
    }
    endwin();
    return 0;
}
