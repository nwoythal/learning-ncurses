#include <stdio.h>
#include <unistd.h>
#include "environment.h"

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
        cave_x=randi(size.ws_col);
        cave_y=randi(size.ws_row);
        generate_terrain(tree_list, size);
    }
    player.x_loc=size.ws_col/2; //start x
    player.y_loc=size.ws_row/2; //start y
    struct character monster={10, 10, 0, 0, 1, 0, randi(size.ws_col), randi(size.ws_row)};
    
    draw(player.x_loc, player.y_loc, SP_You);
    draw(monster.x_loc, monster.y_loc, SP_Mob);
    if(level<=5)
        draw(cave_x, cave_y, SP_Cave);
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
