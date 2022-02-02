#define _POSIX_C_SOURCE 200201L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>

void draw_menu_box();
void draw_game_box(int color, int how_to_draw);
void draw_start_game();
void draw_level_1();
void draw_level_2();
void draw_restart_game();
void draw_go_to_menu();
void draw_exit();
int lost_menu();
void draw_score(unsigned int score, unsigned int max_score_1, unsigned int max_score_2, int lvl);
int menu();
int levels();
void put_spaces_in_blocks(char blocks[2][3][66]);
int game_level_1(char blocks[2][3][66], unsigned int score, unsigned int max_score_1, unsigned int max_score_2);
int game_level_2(char blocks[2][3][66], unsigned int score, unsigned int max_score_1, unsigned int max_score_2);
int update_blockage(int block_y, int where_am_i, int y_player_position, int y_2, int lvl, int logical_update, char blocks[2][3][66], unsigned int score, unsigned int max_score_1, unsigned int max_score_2);
void write_max_score_to_file(unsigned int max_score_1, unsigned int max_score_2);
int lost();

int main(){
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
    srand(time(NULL)); 
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);

    if (LINES >= 33 && COLS >= 76){
        unsigned int score = 0;
        unsigned int max_score_1 = 0;
        unsigned int max_score_2 = 0;
        bool file_exists = 0;
        
        char blocks[2][3][66];
        put_spaces_in_blocks(blocks);
        
        FILE *fp;
        if ((fp = fopen("max_score.txt", "r"))){
            fclose(fp);
            file_exists = true;
        }
        else{
            file_exists = false;
        }
        //read max_score and put it in int max_score
        if (file_exists == true){
            FILE *fp = fopen("max_score.txt", "r");
            fscanf(fp, "%u\n%u", &max_score_1, &max_score_2);
            fclose(fp);
        }
        else{
            FILE *fp = fopen("max_score.txt", "w");
            putc('0', fp);
            putc('\n', fp);
            putc('0', fp);
            fclose(fp);
        }
        int game_state = 0;
        while (true){
            switch (game_state){
                case 0 : { //menu
                    clear();
                    int choice = menu();
                    if (choice == 1){
                        game_state = 1;
                    }
                    else if (choice == 2){
                        game_state = 2;
                    }
                    break;
                }
                case 1 : { // play game
                    clear();
                    int lvl = levels();
                    if (lvl == 1){
                        clear();
                        sleep(0.1);
                        int lost = game_level_1(blocks, score, max_score_1, max_score_2);
                        if (lost == 1){
                            clear();
                            FILE *fp2 = fopen("max_score.txt", "r");
                            fscanf(fp2, "%u\n%u", &max_score_1, &max_score_2);
                            fclose(fp2);
                            put_spaces_in_blocks(blocks); //do new game
                            game_state = 1; 
                            //open and read again. If player restarting - see new result
                        }
                        else if (lost == 2){
                            clear();
                            FILE *fp2 = fopen("max_score.txt", "r");
                            fscanf(fp2, "%u\n%u", &max_score_1, &max_score_2);
                            fclose(fp2);
                            put_spaces_in_blocks(blocks); //do new game
                            game_state = 0;
                        }
                    }
                    else if (lvl == 2){
                        clear();
                        sleep(0.1);
                        int lost = game_level_2(blocks, score, max_score_1, max_score_2);
                        if (lost == 1){
                            clear();
                            //open and read again. If player restarting - see new result
                            FILE *fp2 = fopen("max_score.txt", "r");
                            fscanf(fp2, "%u\n%u", &max_score_1, &max_score_2);
                            fclose(fp2);
                            put_spaces_in_blocks(blocks); //do new game
                            game_state = 1; 
                        }
                        else if (lost == 2){
                            clear();
                            FILE *fp2 = fopen("max_score.txt", "r");
                            fscanf(fp2, "%u\n%u", &max_score_1, &max_score_2);
                            fclose(fp2);
                            put_spaces_in_blocks(blocks); //do new game
                            game_state = 0;
                        }
                    }
                    break;
                }
                case 2 : {//exit
                    clear();
                    endwin();
                    return EXIT_SUCCESS;
                    break;
                }
            }
        }
    }
    else{
        char error[] = "To run the game successfully make the console bigger and restart the game";
        char exit[] = "Press Enter to exit";
        int len_error = strlen(error);
        int len_exit = strlen(exit);
        mvprintw(LINES/2-2, COLS/2-(len_error/2), "%s", error);
        mvprintw(LINES/2, COLS/2-(len_exit/2), "%s", exit);
        refresh();
        getchar();
    }
    endwin();    
    return EXIT_SUCCESS;
}

int game_level_1(char blocks[2][3][66], unsigned int score, unsigned int max_score_1, unsigned int max_score_2){
    int lvl = 1;
    int color = 1;
    int how_to_draw = 0;
    int logical_update = 0;
    int jump_time = 0;
    int y_player_position = LINES/2+4;
    int y_2 = 0;
    int block_y = LINES/2+4;
    int where_am_i = LINES/2+4;
    
    float time = 134.8; // cause i want speed max 100.00
    float speed = 0;
    
    clear();
    draw_game_box(color, how_to_draw);
    attron(COLOR_PAIR(4));
    mvprintw((LINES/2 - 11/2)-2, (COLS/2 - 66/2)-3, "Press 'q' to pause/unpause the game.");
    attroff(COLOR_PAIR(4));
    
    while (true){
        score += 3;
        if (score % 3 == 0){ //change decimal to change speed.
            if (time >= 35){ 
                time -= 0.25; //reduce latency
                speed += 0.25; //increase speed (on display only)
                mvprintw((LINES/2 - 11/2)+3, (COLS/2 - 66/2)+3, "speed: %.2f",speed);
                if (time <= 100){
                    time += 0.1;
                    speed -= 0.1;
                }
                if (time <= 80){
                    time += 0.1;
                    speed -= 0.1;
                }
                if (time <= 50){
                    time += 0.025; //for smooth speed change
                    time -= 0.025;
                }
            }
        }

        draw_score(score, max_score_1, max_score_2, lvl);
        int lost_choice = update_blockage(block_y, where_am_i, y_player_position, y_2, lvl, logical_update, blocks, score, max_score_1, max_score_2);
        if (lost_choice == 1){
            return 1;
        }
        if (lost_choice == 2){
            return 2;
        }
        
        if(jump_time !=0){         
            mvprintw(LINES/2+3, (COLS/2-66/2)+10, ">");
            jump_time -=1;
            if(jump_time == 0){
                y_player_position = LINES/2+4;
            }
            int time_delay = time;
            while(time_delay>0){
                int input = getch();
                if (input == KEY_DOWN){
                    y_player_position = LINES/2+4;
                    if (y_player_position == lost_choice){ //check. if i jumping and press KEY_DOWN and there is an cactus under me, I will die.
                        attron(COLOR_PAIR(4));
                        mvprintw(LINES/2+4, (COLS/2-66/2)+10, "*");
                        attroff(COLOR_PAIR(4));
                        mvprintw(LINES/2+3, (COLS/2-66/2)+10, " ");
                        if (score > max_score_1){
                            max_score_1 = score;
                            write_max_score_to_file(max_score_1, max_score_2);
                        }
                        int choice = lost();
                        if (choice == 1){
                            return 1;
                        }  
                        else if (choice == 2){
                            return 2;
                        }
                    }
                    mvprintw(LINES/2+3, (COLS/2-66/2)+10, " ");
                    mvprintw(y_player_position, (COLS/2-66/2)+10, ">");
                    jump_time = 0;
                    refresh();
                }
                if (input == 'q'){ //pause the game
                    attron(COLOR_PAIR(4));
                    mvprintw(LINES/2-1, (COLS/2)-7,"game is paused");
                    mvprintw(LINES/2, (COLS/2)-15,"press 'e' to exit to the menu.");
                    while (true){
                        input = getch();
                        if (input == 'q'){
                            mvprintw(LINES/2-1, (COLS/2)-7,"              ");
                            mvprintw(LINES/2, (COLS/2)-15,"                              ");
                            break;
                        }
                        else if (input == 'e'){
                            if (score > max_score_1){
                                max_score_1 = score;
                                write_max_score_to_file(max_score_1, max_score_2);
                            }
                            return 2;
                        }
                    }
                    attroff(COLOR_PAIR(4));
                }
                time_delay -=1;
                napms(1);
            }
            napms(time_delay);
        }   
        else{
            mvprintw(LINES/2+4, (COLS/2-66/2)+10,">");
            mvprintw(LINES/2+3, (COLS/2-66/2)+10," ");
            refresh();
            int time_delay = time;
            while(time_delay>0){
                int key_pressed = 1;
                int input = getch();
                if(input == KEY_UP && key_pressed){
                    key_pressed = 0;
                    jump_time = 6;
                    if (speed == 100){
                        jump_time = 11;
                    }
                    else if (speed >= 90){
                        jump_time = 10;
                    }
                    else if (speed >= 70){
                        jump_time = 9;
                    }
                    else if (speed >= 60){
                        jump_time = 8;
                    }
                    else if (speed >= 50){
                        jump_time = 7; 
                    }
                    y_player_position = LINES/2+3;
                }
                if (input == 'q'){
                    attron(COLOR_PAIR(4));
                    mvprintw(LINES/2-1, (COLS/2)-7,"game is paused");
                    mvprintw(LINES/2, (COLS/2)-15,"press 'e' to exit to the menu.");
                    while (true){
                        input = getch();
                        if (input == 'q'){
                            mvprintw(LINES/2-1, (COLS/2)-7,"              ");
                            mvprintw(LINES/2, (COLS/2)-15,"                              ");
                            break;
                        }
                        else if (input == 'e'){
                            if (score > max_score_1){
                                max_score_1 = score;
                                write_max_score_to_file(max_score_1, max_score_2);
                            }
                            return 2;
                        }
                    }
                    attroff(COLOR_PAIR(4));
                }
                time_delay -=1;
                napms(1);
            }
            napms(time_delay);
        }
        refresh();
    }
}

int game_level_2(char blocks[2][3][66], unsigned int score, unsigned int max_score_1, unsigned int max_score_2){
    int color = 1;
    int how_to_draw = 1;
    int logical_update;
    
    int where_am_i = LINES/2+4;
    int position = 1;
    int block_y = 5;
    int block_y2 = LINES/2+3;
    bool sit = false;
    //int input = getch();
    float time = 134.8; //cause i want speed max 100.00
    float speed = 0;
    draw_game_box(color, how_to_draw);
    attron(COLOR_PAIR(4));
    mvprintw((LINES/2 - 11/2)-2, (COLS/2 - 66/2)-3, "Press 'q' to pause/unpause the game.");
    attroff(COLOR_PAIR(4));
    
    while (true){
        int jump_time = 0;
        int lvl = 2;
        int y_player_position = LINES/2+4;
        int y_2 = LINES/2+3;
        while (true){
            score += 3;
            if (score % 3 == 0){ //change decimal to change speed.
                if (time >= 35){ 
                    time -= 0.25; //reduce latency
                    speed += 0.25; //increase speed (on display only)
                    mvprintw((LINES/2 - 11/2)+3, (COLS/2 - 66/2)+3, "speed: %.2f",speed);
                    if (time <= 100){
                        time += 0.1;
                        speed -= 0.1;
                    }
                    if (time <= 80){ //smooth change speed
                        time += 0.1;
                        speed -= 0.1;
                    }
                    if (time <= 50){
                        time += 0.025;
                        time -= 0.025;
                    }
                }
            }
            
            draw_score(score, max_score_1, max_score_2, lvl);
            int change = 0;
            logical_update = 0;
            
            int lost_choice = update_blockage(block_y, where_am_i, y_player_position, y_2, lvl, logical_update, blocks, score, max_score_1, max_score_2);// return block_y 4tobi potom sravnit pri bistrom prishke ili padenii
            if (lost_choice == 1){
                return 1;
            }
            else if (lost_choice == 2){
                return 2;
            }
            
            if(jump_time !=0){
                //mvprintw(LINES/2+4, (COLS/2-66/2)+10, " ");
                mvprintw(LINES/2+3, (COLS/2-66/2)+10, "Z");
                mvprintw(LINES/2+2, (COLS/2-66/2)+10, ">");
                refresh();
                jump_time -=1;
                
                int time_delay = time;
                while (time_delay>0){
                    int input = getch();
                    if (input == KEY_DOWN && sit == false){ //if im in jumping press key_down
                        sit = true;
                        jump_time = 0;
                        refresh();
                    }
                    if (input == 'q'){
                        attron(COLOR_PAIR(4));
                        mvprintw(LINES/2-1, (COLS/2)-7,"game is paused");
                        mvprintw(LINES/2, (COLS/2)-15,"press 'e' to exit to the menu.");
                        while (true){
                            input = getch();
                            if (input == 'q'){
                                mvprintw(LINES/2-1, (COLS/2)-7,"              ");
                                mvprintw(LINES/2, (COLS/2)-15,"                              ");
                                break;
                            }
                            else if (input == 'e'){
                                if (score > max_score_2){
                                    max_score_2 = score;
                                    write_max_score_to_file(max_score_1, max_score_2);
                                }
                                return 2;
                            }
                        }
                        attroff(COLOR_PAIR(4));
                    }
                    time_delay -= 1;
                    napms(1);
                }
                if(jump_time == 0){
                    if (where_am_i == LINES/2+4){
                        y_player_position = LINES/2+4;
                        y_2 = LINES/2+3;
                        block_y2 = LINES/2+3;
                    }
                    else if (where_am_i == LINES/2+3){
                        y_player_position = LINES/2+3;
                        y_2 = LINES/2+2;
                        block_y2 = LINES/2+2;
                    }
                    else if (where_am_i == LINES/2+2){
                        y_player_position = LINES/2+2;
                        y_2 = LINES/2+1;
                        block_y2 = LINES/2+1;
                    }
                    mvprintw(LINES/2+4, (COLS/2-66/2)+10, "Z");
                    mvprintw(LINES/2+3, (COLS/2-66/2)+10, ">");
                    mvprintw(LINES/2+2, (COLS/2-66/2)+10, " ");
                    refresh();
                }
                if (sit == true){
                    if (where_am_i == LINES/2+4){
                        y_player_position = LINES/2+5;
                        y_2 = LINES/2+4;
                        block_y2 = LINES/2+3;
                    }
                    else if (where_am_i == LINES/2+3){
                        y_player_position = LINES/2+5;
                        y_2 = LINES/2+3;
                        block_y2 = LINES/2+2;
                    }
                    else if (where_am_i == LINES/2+2){
                        y_player_position = LINES/2+5;
                        y_2 = LINES/2+2;
                        block_y2 = LINES/2+1;
                    }
                    mvprintw(LINES/2+2, (COLS/2-66/2)+10, " ");
                    mvprintw(LINES/2+3, (COLS/2-66/2)+10, " ");
                    mvprintw(LINES/2+4, (COLS/2-66/2)+10, ">");
                    refresh();
                    if ((y_player_position == lost_choice) || (y_player_position == block_y2) || (y_2 == lost_choice) || (y_2 ==block_y2)){ //if in jump i pressed key_down and there was a obstacle under me -i die, else nothing
                        attron(COLOR_PAIR(4));
                        if ((y_player_position == lost_choice) || (y_2 == lost_choice)){
                            mvprintw(LINES/2+4, (COLS/2-66/2)+10, "*");
                            mvprintw(LINES/2+3, (COLS/2-66/2)+10, " ");
                        }
                        else{
                            mvprintw(LINES/2+4, (COLS/2-66/2)+10, " ");
                            mvprintw(LINES/2+3, (COLS/2-66/2)+10, "*");
                        }
                        attroff(COLOR_PAIR(4));
                        if (score > max_score_2){
                            max_score_2 = score;
                            write_max_score_to_file(max_score_1, max_score_2);
                        }
                        int choice = lost();
                        if (choice == 1){
                            return 1;
                        }
                        else if (choice == 2){
                            return 2;
                        }
                    }
                }
                else{
                    if (where_am_i == LINES/2+4){
                        y_player_position = LINES/2+3;
                        y_2 = LINES/2+2;
                        block_y2 = LINES/2+3;
                    }
                    else if (where_am_i == LINES/2+3){
                        y_player_position = LINES/2+2;
                        y_2 = LINES/2+1; 
                        block_y2 = LINES/2+2;
                    }
                    else if (where_am_i == LINES/2+2){
                        y_player_position = LINES/2+1;
                        y_2 = LINES/2;
                        block_y2 = LINES/2+1;
                    }
                }
                napms(time_delay);
            }
            else{ //if im not jumping
                if ((jump_time == 0) && (y_player_position != LINES/2+5)){
                    if (where_am_i == LINES/2+4){
                        y_player_position = LINES/2+4;
                        y_2 = LINES/2+3;
                        block_y2 = LINES/2+3;
                    }
                    else if (where_am_i == LINES/2+3){
                        y_player_position = LINES/2+3;
                        y_2 = LINES/2+2;
                        block_y2 = LINES/2+2;
                    }
                    else if (where_am_i == LINES/2+2){
                        y_player_position = LINES/2+2;
                        y_2 = LINES/2+1;
                        block_y2 = LINES/2+1;
                    }
                    mvprintw(LINES/2+4, (COLS/2-66/2)+10, "Z");
                    mvprintw(LINES/2+3, (COLS/2-66/2)+10, ">");
                    mvprintw(LINES/2+2, (COLS/2-66/2)+10, " ");
                    refresh();
                }
                else{
                    mvprintw(LINES/2+2, (COLS/2-66/2)+10,  " ");
                    //mvprintw(LINES/2+3, (COLS/2-66/2)+10,  " ");
                    mvprintw(LINES/2+4, (COLS/2-66/2)+10,  ">");
                    refresh();
                }
                int time_delay = time;
                while (time_delay>0){
                    int key_pressed = 1;
                    int input = getch();
                    if (input == ' '){
                        change = 1;
                        attron(COLOR_PAIR(3));
                        if (position == 1){
                            mvprintw(LINES/2+4, (COLS/2 - 66/2), "%s", blocks[0][0]);   
                        }
                        else if (position == 2){
                            mvprintw(LINES/2+4, (COLS/2 - 66/2), "%s", blocks[0][1]);   
                        }
                        else if (position == 3){
                            mvprintw(LINES/2+4, (COLS/2 - 66/2), "%s", blocks[0][2]);   
                        }
                        attroff(COLOR_PAIR(3));
                    }
                    if (input == 'q'){
                        attron(COLOR_PAIR(4));
                        mvprintw(LINES/2-1, (COLS/2)-7,"game is paused");
                        mvprintw(LINES/2, (COLS/2)-15,"press 'e' to exit to the menu.");
                        while (true){
                            input = getch();
                            if (input == 'q'){
                                mvprintw(LINES/2-1, (COLS/2)-7,"              ");
                                mvprintw(LINES/2, (COLS/2)-15,"                              ");
                                break;
                            }
                            else if (input == 'e'){
                                if (score > max_score_2){
                                    max_score_2 = score;
                                    write_max_score_to_file(max_score_1, max_score_2);
                                }
                                return 2;
                            }
                        }
                        attroff(COLOR_PAIR(4));
                    }
                    if (input == KEY_UP && key_pressed){ 
                        key_pressed = 0;
                        jump_time=6;
                        if (speed == 100){
                            jump_time = 11;
                        }
                        else if (speed >= 90){
                            jump_time = 10;
                        }
                        else if (speed >= 70){
                            jump_time = 9;
                        }
                        else if (speed >= 60){
                            jump_time = 8;
                        }
                        else if (speed >= 50){
                            jump_time = 7; 
                        }
                        sit = false;
                        if (where_am_i == LINES/2+4){
                            y_player_position = LINES/2+3;
                            y_2 = LINES/2+2;
                            block_y2 = LINES/2+3;
                        }
                        else if (where_am_i == LINES/2+3){
                            y_player_position = LINES/2+2;
                            y_2 = LINES/2+1; 
                            block_y2 = LINES/2+2;
                        }
                        else if (where_am_i == LINES/2+2){
                            y_player_position = LINES/2+1;
                            y_2 = LINES/2;
                            block_y2 = LINES/2+1;
                        }
                    }
                    if (input == KEY_DOWN && sit == false && key_pressed){
                        key_pressed = 0;
                        sit = true;
                    }
                    time_delay -= 1;
                    napms(1);
                }
                if (change == 1){
                    break;
                }
                if (sit == true){
                        if (where_am_i == LINES/2+4){
                            y_player_position = LINES/2+5;
                            y_2 = LINES/2+4;
                            block_y2 = LINES/2+3;
                        }
                        else if (where_am_i == LINES/2+3){
                            y_player_position = LINES/2+5;
                            y_2 = LINES/2+3;
                            block_y2 = LINES/2+2;
                        }
                        else if (where_am_i == LINES/2+2){
                            y_player_position = LINES/2+5;
                            y_2 = LINES/2+2;
                            block_y2 = LINES/2+1;
                        }
                        mvprintw(LINES/2+2, (COLS/2-66/2)+10, " ");
                        mvprintw(LINES/2+3, (COLS/2-66/2)+10, " ");
                        mvprintw(LINES/2+4, (COLS/2-66/2)+10, ">");
                        refresh();
                }
                else {
                    if (jump_time == 0){
                    if (where_am_i == LINES/2+4){
                        y_player_position = LINES/2+4;
                        y_2 = LINES/2+3;
                        block_y2 = LINES/2+3;
                    }
                    else if (where_am_i == LINES/2+3){
                        y_player_position = LINES/2+3;
                        y_2 = LINES/2+2;
                        block_y2 = LINES/2+2;
                    }
                    else if (where_am_i == LINES/2+2){
                        y_player_position = LINES/2+2;
                        y_2 = LINES/2+1;
                        block_y2 = LINES/2+1;
                    }
                    mvprintw(LINES/2+4, (COLS/2-66/2)+10, "Z");
                    mvprintw(LINES/2+3, (COLS/2-66/2)+10, ">");
                    mvprintw(LINES/2+2, (COLS/2-66/2)+10, " ");
                    refresh();
                    }
                }
                napms(time_delay);
                refresh();
                if (lost_choice != 0){// x_block == x_player, now i klicked key_down and check == block or no, if yes - lose
                    if ((y_player_position == lost_choice) || (y_player_position == block_y2) || (y_2 == block_y) || (y_2 == block_y2)){ //check all variations...
                        attron(COLOR_PAIR(4));
                        if ((y_player_position == block_y2) || (y_2 == block_y2)){
                            mvprintw(LINES/2+3, (COLS/2-66/2)+10, "*");
                            mvprintw(LINES/2+4, (COLS/2-66/2)+10, " ");
                        }
                        else if ((y_player_position == lost_choice) || (y_2 == block_y)){
                            refresh();
                            mvprintw(LINES/2+4, (COLS/2-66/2)+10, "*");
                            mvprintw(LINES/2+3, (COLS/2-66/2)+10, " ");
                        }
                        attroff(COLOR_PAIR(4));
                        mvprintw(LINES/2+2, (COLS/2-66/2)+10, " ");
                        if (score > max_score_2){
                            max_score_2 = score;
                            write_max_score_to_file(max_score_1, max_score_2);
                        }
                        int choice = lost();
                        if (choice == 1){
                            return 1;
                        }
                        else if (choice == 2){
                            return 2;
                        }
                    }
                    if (((y_2 == lost_choice) && (y_player_position == lost_choice)) || (y_player_position == block_y2) || (y_2 == block_y) || (y_2 == block_y2)){
                        attron(COLOR_PAIR(4));
                        if ((y_player_position == lost_choice) || (y_2 == block_y) || (y_2 == lost_choice)){
                            refresh();
                            mvprintw(LINES/2+3, (COLS/2-66/2)+10, " ");
                            mvprintw(LINES/2+4, (COLS/2-66/2)+10, "*");
                            mvprintw(LINES/2+2, (COLS/2-66/2)+10, " ");
                        }
                        else if ((y_player_position == block_y2) || (y_2 == block_y2)){
                            mvprintw(LINES/2+3, (COLS/2-66/2)+10, "*");
                            mvprintw(LINES/2+4, (COLS/2-66/2)+10, " ");
                            mvprintw(LINES/2+2, (COLS/2-66/2)+10, " ");
                        }
                        attroff(COLOR_PAIR(4));
                        if (score > max_score_2){
                            max_score_2 = score;
                            write_max_score_to_file(max_score_1, max_score_2);
                        }
                        int choice = lost();
                        if (choice == 1){
                            return 1;
                        }
                        else if (choice == 2){
                            return 2;
                        }
                    }
                }
            }
            refresh();
        }

//...................second.view..........................of.............the..............................game............
        
        if (position == 1){    //to see ">", because i have " " all time..   
            mvprintw(LINES/2+4,(COLS/2-66/2)+10,"@");
        }
        else if (position == 2){
            mvprintw(LINES/2+3, (COLS/2-66/2)+10, "@");
        }
        else if (position == 3){
            mvprintw(LINES/2+2, (COLS/2-66/2)+10, "@");
        }

        while (true){
            score += 3;
            if (score % 3 == 0){ //change decimal to change speed.
                if (time >= 35){ 
                    time -= 0.25; //reduce latency
                    speed += 0.25; //increase speed (on display only)
                    mvprintw((LINES/2 - 11/2)+3, (COLS/2 - 66/2)+3, "speed: %.2f",speed);
                    if (time <= 100){
                        time += 0.1;
                        speed -= 0.1;
                    }
                    if (time <= 80){
                        time += 0.1;
                        speed -= 0.1;
                    }
                    if (time <= 50){
                        time += 0.025; //smooth speed changing
                        time -= 0.025;
                    }
                }
            }
            
            int change2 = 0;
            logical_update = 1;
            draw_score(score, max_score_1, max_score_2, lvl);
            
            int lost_choice = update_blockage(block_y, where_am_i, y_player_position, y_2, lvl, logical_update, blocks, score, max_score_1, max_score_2);
            refresh();
            if (lost_choice == 1){
                return 1;
            }
            if (lost_choice == 2){
                return 2;
            }
            if (position == 1){
                    mvprintw(LINES/2+4, (COLS/2-66/2)+10, "@");
            }
            else if (position == 2){
                    mvprintw(LINES/2+3, (COLS/2-66/2)+10, "@");
            }
            else if (position == 3){
                mvprintw(LINES/2+2, (COLS/2-66/2)+10, "@");
            }
            
            int time_delay = time;
            
            while(time_delay>0){
                int input = getch();
                switch(input){
                    case ' ' : {
                        if (position == 1){
                            y_player_position = LINES/2+4;
                            y_2 = LINES/2+3;
                            where_am_i = y_player_position;
                            mvprintw(LINES/2+4, (COLS/2-66/2)+10, "@");
                        }
                        else if (position == 2){
                            y_player_position = LINES/2+3;
                            y_2 = LINES/2+2;
                            where_am_i = y_player_position;
                        }
                        else if (position == 3){
                            y_player_position = LINES/2+2;
                            y_2 = LINES/2+1;
                            where_am_i = y_player_position;
                        }
                        change2 = 1;
                        break;
                    }
                    case 'q' : {
                        attron(COLOR_PAIR(4));
                        mvprintw(LINES/2-1, (COLS/2)-7,"game is paused");
                        mvprintw(LINES/2, (COLS/2)-15,"press 'e' to exit to the menu.");
                        while (true){
                            input = getch();
                            if (input == 'q'){
                                mvprintw(LINES/2-1, (COLS/2)-7,"              ");
                                mvprintw(LINES/2, (COLS/2)-15,"                              ");
                                break;
                            }
                            else if (input == 'e'){
                                if (score > max_score_2){
                                    max_score_2 = score;
                                    write_max_score_to_file(max_score_1, max_score_2);
                                }
                                return 2;
                            }
                        }
                        attroff(COLOR_PAIR(4));
                        break;
                    }
                    case KEY_UP : {
                        if (position == 1){
                            mvprintw(LINES/2+4, (COLS/2-66/2)+10, " ");
                            mvprintw(LINES/2+3, (COLS/2-66/2)+10, "@");
                            position = 2;
                            y_player_position = LINES/2+3;
                            if (lost_choice != 0){// x_block == x_player, now i klicked key_down and check == block or no, if yes - lose
                                if (y_player_position == lost_choice){
                                    attron(COLOR_PAIR(4));
                                    mvprintw(y_player_position, (COLS/2-66/2)+10, "*");
                                    attroff(COLOR_PAIR(4));
                                    if (score > max_score_2){
                                        max_score_2 = score;
                                        write_max_score_to_file(max_score_1, max_score_2);
                                    }
                                    int choice = lost();
                                    if (choice == 1){
                                        return 1;
                                    }
                                    else if (choice == 2){
                                        return 2;
                                     }
                                }
                            }
                        }
                        else if (position == 2){
                            mvprintw(LINES/2+3, (COLS/2-66/2)+10, " ");
                            mvprintw(LINES/2+2, (COLS/2-66/2)+10, "@");
                            position = 3;
                            y_player_position = LINES/2+2;
                            if (lost_choice != 0){
                                if (y_player_position == lost_choice){
                                    attron(COLOR_PAIR(4));
                                    mvprintw(y_player_position, (COLS/2-66/2)+10, "*");
                                    attroff(COLOR_PAIR(4));
                                    if (score > max_score_2){
                                        max_score_2 = score;
                                        write_max_score_to_file(max_score_1, max_score_2);
                                    }
                                    int choice = lost();
                                    if (choice == 1){
                                        return 1;
                                    }
                                    else if (choice == 2){
                                        return 2;
                                     }
                                }
                            }
                        }
                        break;
                    }
                    case KEY_DOWN : {
                        if (position == 3){
                            mvprintw(LINES/2+2, (COLS/2-66/2)+10, " ");
                            mvprintw(LINES/2+3, (COLS/2-66/2)+10, "@");
                            position = 2;
                            y_player_position = LINES/2+3;
                            if (lost_choice != 0){
                                if (y_player_position == lost_choice){
                                    attron(COLOR_PAIR(4));
                                    mvprintw(y_player_position, (COLS/2-66/2)+10, "*");
                                    attroff(COLOR_PAIR(4));
                                    if (score > max_score_2){
                                        max_score_2 = score;
                                        write_max_score_to_file(max_score_1, max_score_2);
                                    }
                                    int choice = lost();
                                    if (choice == 1){
                                        return 1;
                                    }
                                    else if (choice == 2){
                                        return 2;
                                     }
                                }
                            }
                        }

                        else if (position == 2){
                            mvprintw(LINES/2+3, (COLS/2-66/2)+10, " ");
                            mvprintw(LINES/2+4, (COLS/2-66/2)+10, "@");
                            position = 1;
                            y_player_position = LINES/2+4;
                            if (lost_choice != 0){
                                if (y_player_position == lost_choice){
                                    attron(COLOR_PAIR(4));
                                    mvprintw(y_player_position, (COLS/2-66/2)+10, "*");
                                    attroff(COLOR_PAIR(4));
                                    if (score > max_score_2){
                                        max_score_2 = score;
                                        write_max_score_to_file(max_score_1, max_score_2);
                                    }
                                    int choice = lost();
                                    if (choice == 1){
                                        return 1;
                                    }
                                    else if (choice == 2){
                                        return 2;
                                     }
                                }
                            }
                        }
                        break;
                    }
                    default: break;
                }
                time_delay -=1;
                napms(1);
                refresh();
            }
            if (change2 == 1){
                break;
            }
            napms(time_delay);
        }
    }
}

int update_blockage(int block_y, int where_am_i, int y_player_position, int y_2, int lvl, int logical_update, char blocks[2][3][66], unsigned int score, unsigned int max_score_1, unsigned int max_score_2){
    char block;
    bool what;
    int which_block_line;
    
    block_y = LINES/2+4;
    int player_x = (COLS/2-66/2)+10;
    int block_y2;
    int block_x;

//................................lvl.1.....................................
    if (lvl == 1 && logical_update == 0){
        for (int i = 0; i < 64; i++){
            blocks[0][0][i] = blocks[0][0][i+1];
        }
        what = rand () % 21; //assigning numbers from 1 to 21 TO BOOL, i know. if u are reading this, forgive and have mercy xd. Everything workds and i dont want to change it half an hour before the deadline
        if (what == 1){
            block = ' ';
        }
        else{
            block = '#';
        }
        blocks[0][0][64] = block;
        attron(COLOR_PAIR(3));
        mvprintw(LINES/2+4, (COLS/2 -66/2)+1, "%s", blocks);
        attroff(COLOR_PAIR(3));
        refresh();
        
        for (int i = 0; i < 64; i++){
            if (blocks[0][0][i] == '#'){
                block_x = (COLS/2 - 66/2)+1 + i;
                break;
            }
        }
        if (block_x == player_x){
            if (y_player_position == block_y){ 
                attron(COLOR_PAIR(4));
                mvprintw(LINES/2+4, (COLS/2-66/2)+10, "*");
                mvprintw(LINES/2+3, (COLS/2-66/2)+10, " ");
                attroff(COLOR_PAIR(4)); 
                refresh();
                if (score > max_score_1){
                    max_score_1 = score;
                    write_max_score_to_file(max_score_1, max_score_2);
                }
                int choise = lost();
                if (choise == 1){
                    return 1;
                }
                else if (choise == 2){
                    return 2;
                }
            }
            return block_y; //if x_plyaer == x_block but y isn't, return block_y to check where i will be if i'will do key_down. I need to know i lost cause of cactus or not
        }
    }
//......................lvl.2....................................................
    else if (lvl == 2){
    //initialization
        for (int n = 0; n < 2; n++){
            for (int j = 0; j <3; j++){
                for (int i = 0; i < 64; i++){
                    blocks[n][j][i] = blocks[n][j][i+1];
                }
            }
        }
        for (int j = 0; j < 3; j++){
            what = rand () % 25; //assigning numbers from 1 to 21 TO BOOL, i know. if u are reading this, forgive and have mercy xd. Everything workds and i dont want to change it half an hour before the deadline
            if (what == 1){
                block = ' ';
            }
            else{
                block = '#';
            }
            blocks[0][j][64] = block;
        }
        for (int j = 0; j < 3; j++){
            what = rand () % 25; //assigning numbers from 1 to 21 TO BOOL, i know. if u are reading this, forgive and have mercy xd. Everything workds and i dont want to change it half an hour before the deadline
            if (what == 1){
                block = ' ';
            }
            else{
                block = '~';
            }
            blocks[1][j][64] = block;
        }
        refresh();
//.............if................game........view............to.......side.................
        if (logical_update == 0){
            mvprintw((LINES/2 - 11/2)+3, (COLS/2 + 66/2)-20, "    view: size ");
            attron(COLOR_PAIR(5));
            mvprintw((LINES/2+11/2)+1, (COLS/2-66/2)-2, "      Switch the game view to avoid cactuses in a different way.      ");
            attroff(COLOR_PAIR(5));
            refresh();
            if (where_am_i == LINES/2+4){
                attron(COLOR_PAIR(5));
                mvprintw(LINES/2+3, (COLS/2 -66/2), "%s", blocks[1][0]);
                attroff(COLOR_PAIR(5));
                attron(COLOR_PAIR(3));
                mvprintw(LINES/2+2, (COLS/2 -66/2), "%s", "                                                                 ");
                mvprintw(LINES/2+4, (COLS/2 -66/2), "%s", blocks[0][0]);   
                attroff(COLOR_PAIR(3));
                which_block_line = 0;
                block_y = LINES/2+4;
                block_y2 = LINES/2+3; //added to correctly check all cases
                refresh();
            }
            else if (where_am_i == LINES/2+3){
                attron(COLOR_PAIR(5));
                mvprintw(LINES/2+3, (COLS/2 - 66/2), "%s", blocks[1][1]);
                attroff(COLOR_PAIR(5));
                attron(COLOR_PAIR(3));
                mvprintw(LINES/2+2, (COLS/2 - 66/2), "%s", "                                                                 ");
                mvprintw(LINES/2+4, (COLS/2 - 66/2), "%s", blocks[0][1]);    
                attroff(COLOR_PAIR(3));
                which_block_line = 1;
                block_y = LINES/2+3;
                block_y2 = LINES/2+2;
                refresh();
            }
            else if (where_am_i == LINES/2+2){
                attron(COLOR_PAIR(5));
                mvprintw(LINES/2+3, (COLS/2 -66/2), "%s", blocks[1][2]);
                attroff(COLOR_PAIR(5));
                attron(COLOR_PAIR(3));
                mvprintw(LINES/2+2, (COLS/2 -66/2), "%s", "                                                                 ");
                mvprintw(LINES/2+4, (COLS/2 -66/2), "%s", blocks[0][2]);
                attroff(COLOR_PAIR(3));
                which_block_line = 2;
                block_y = LINES/2+2;
                block_y2 = LINES/2+1;
                refresh();
            }
            for (int i = 0; i < 64; i++){
                if (blocks[0][which_block_line][i] == '#'){
                    block_x = (COLS/2 - 66/2) + i;
                    break;
                }
            }
            if(block_x == player_x){ //to die. like if i was on 2 line and changed 3d game to 2d i need to be on 2 line again and i need to die from 2 line, not from 1. and i need to know at which line i was.
                //if ((y_player_position == block_y) || (y_player_position == block_y2 && (y_2....)) || (y_2 == block_y) || (y_2 == block_y2)){
                if ((y_player_position == block_y) || (y_2 == block_y) || (y_2 == block_y2)){ //check all cases..
                    attron(COLOR_PAIR(4));
                    mvprintw(LINES/2+4, (COLS/2-66/2)+10, "*");
                    attroff(COLOR_PAIR(4));
                    if (score > max_score_2){
                        max_score_2 = score;
                        write_max_score_to_file(max_score_1, max_score_2);
                    }
                    int choice = lost();
                    if (choice == 1){
                        return 1;
                    }
                    else if (choice == 2){
                        return 2;
                    }
                }
                return block_y;
            }
            for (int i = 0; i < 64; i++){
                if (blocks[1][which_block_line][i] == '~'){
                    block_x = (COLS/2 - 66/2) + i;
                    break;
                }
            }
            if ( block_x == player_x){
                block_y = LINES/2+3;
                y_2 = y_player_position -1;
                if ((y_player_position == block_y) || (y_player_position == block_y2) || (y_2 == block_y) || (y_2 == block_y2)){
                    attron(COLOR_PAIR(4));
                    mvprintw(LINES/2+3, (COLS/2 -66/2)+10, "*");
                    attroff(COLOR_PAIR(4));
                    if (score > max_score_2){
                        max_score_2 = score;
                        write_max_score_to_file(max_score_1, max_score_2);
                    }
                    int choice = lost();
                    if (choice == 1){
                        return 1;
                    }
                    else if (choice == 2){
                        return 2;
                    }
                }
                return block_y;
            }
        }
  
//...................if..........i.........changed.......game....view.....to....above...........
        else if (logical_update == 1){
            mvprintw((LINES/2 - 11/2)+3, (COLS/2 + 66/2)-20, "    view: top ");
            refresh();
            //print line with blocks(cactuses) that will be on top, if you will change the view
            attron(COLOR_PAIR(5));
            if (y_player_position == LINES/2+4){
                mvprintw(LINES/2+6, (COLS/2 - 66/2), "%s", blocks[1][0]);
                refresh();
            }
            else if (y_player_position == LINES/2+3){
                mvprintw(LINES/2+6, (COLS/2 - 66/2), "%s", blocks[1][1]);
                refresh();
            }
            else if (y_player_position == LINES/2+2){
                mvprintw(LINES/2+6, (COLS/2 - 66/2), "%s", blocks[1][2]);
                refresh();
            }
            attroff(COLOR_PAIR(5));
            int tmp = 4;
            for (int i = 0; i < 3; i++, tmp--){
                attron(COLOR_PAIR(3));
                mvprintw(LINES/2+tmp, (COLS/2 - 66/2), "%s", blocks[0][i]);
                attroff(COLOR_PAIR(3));
            }
            for (int i = 0; i < 64; i++){
                if (blocks[0][0][i] == '#'){
                    block_x = (COLS/2 - 66/2) + i;
                    block_y = LINES/2+4;
                    if(block_x == player_x){
                        if (y_player_position == block_y){
                            attron(COLOR_PAIR(4));
                            mvprintw(block_y, block_x, "*");
                            attroff(COLOR_PAIR(4));
                            if (score > max_score_2){
                                max_score_1 = score;
                                write_max_score_to_file(max_score_1, max_score_2);
                            }
                            int choice = lost();
                            if (choice == 1){
                                return 1;
                            } 
                            else if (choice == 2){
                                return 2;
                            }
                        }
                        return block_y; //if x_player == x_block but y not, return block_y to check where i will be if i'll klick key_up or ky_down. i need to know i lost or not. :)
                        break;
                    }
                }
            }
            for (int i = 0; i < 64; i++){
                if (blocks[0][1][i] == '#'){
                    block_x = (COLS/2 - 66/2) + i;
                    block_y = LINES/2+3;
                    if(block_x == player_x){
                        if (y_player_position == block_y){
                            attron(COLOR_PAIR(4));
                            mvprintw(block_y, block_x, "*");
                            attroff(COLOR_PAIR(4));
                            if (score > max_score_2){
                                max_score_2 = score;
                                write_max_score_to_file(max_score_1, max_score_2);
                            }
                            int choice = lost();
                            if (choice == 1){
                                return 1;
                            }
                            else if (choice == 2){
                                return 2;
                            }
                        }
                        return block_y;
                        break;
                    }
                }
            }
            for (int i = 0; i < 64; i++){
                if (blocks[0][2][i] == '#'){
                    block_x = (COLS/2-66/2) + i;
                    block_y = LINES/2+2;
                    if(block_x == player_x){
                        if (y_player_position == block_y){
                            attron(COLOR_PAIR(4));
                            mvprintw(block_y, block_x, "*");
                            attroff(COLOR_PAIR(4));
                            if (score > max_score_2){
                                max_score_2 = score;
                                write_max_score_to_file(max_score_1, max_score_2);
                            }
                            int choice = lost();
                            if (choice == 1){
                                return 1;
                            }
                            else if (choice == 2){
                                return 2;
                            }   
                        }
                        return block_y;
                        break;
                    } 
                }
            }   
        }
    }
    return 0;
}

void put_spaces_in_blocks(char blocks[2][3][66]){
    for (int n = 0; n < 2; n++){
        for (int j = 0; j < 3; j++){
            for (int i = 0; i < 65; i++){
                blocks[n][j][i] = ' ';
            }
            blocks[n][j][65] = '\0';
        }
    }
}

int lost(){
    attron(COLOR_PAIR(4));
    mvprintw((LINES/2), COLS/2-4, "You lost.");
    attroff(COLOR_PAIR(4));
    refresh();
    sleep(2);
    int choose = lost_menu();
    if (choose == 1){
        return 1;
    }
    else if (choose == 2){
        return 2;
    }
    getch();   
    return 0;
}

void write_max_score_to_file(unsigned int max_score_1, unsigned int max_score_2){
    FILE *fp = fopen ("max_score.txt", "w");
    fprintf (fp, "%u\n%u", max_score_1, max_score_2);
    fclose(fp);
}

int menu(){
    draw_menu_box();
    attron(COLOR_PAIR(1));//logo
    mvprintw(LINES/2-9, COLS/2-27, "  __| |   __ _   _   _       __| | (_)  _ __     ___  ");
    mvprintw(LINES/2-8, COLS/2-27, " / _` |  / _` | | | | |     / _` | | | | '_ \\   / _ \\ ");
    mvprintw(LINES/2-7, COLS/2-27, "| (_| | | (_| | | |_| |    | (_| | | | | | | | | (_) |");
    mvprintw(LINES/2-6, COLS/2-27, " \\__,_|  \\__,_|  \\__, |     \\__,_| |_| |_| |_|  \\___/ ");
    mvprintw(LINES/2-5, COLS/2-27, "                 |___/                                ");
    refresh();
    attroff(COLOR_PAIR(1));
    int button_num = 1;
    bool key_1 = true;
    bool key_2 = false;
    //walls
    draw_start_game();
    draw_exit();
    refresh();
    //button move
    while (true){
        if (key_1 == true){
            attron(COLOR_PAIR(2));
            draw_start_game();
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
            draw_exit();
            refresh();
            attroff(COLOR_PAIR(1));  
        }
        else if (key_2 == true){
            attron(COLOR_PAIR(1));
            draw_start_game();
            attroff(COLOR_PAIR(1));
            attron(COLOR_PAIR(2));
            draw_exit();
            refresh();
            attroff(COLOR_PAIR(2));
        }     
        //working with keyboard
        int input = getch();
        switch(input){
               case '\n' :{
                if (button_num == 1){
                    return 1;
                }
                else if(button_num == 2){
                    return 2;
                }
                break;
            }
            case KEY_DOWN : {
                if (key_1 == true){
                    key_2 = true;
                    key_1 = false;
                    button_num = 2;
                }
                else if(key_2 == true){
                    key_1 = true;
                    key_2 = false;
                    button_num = 1;
                }
                break;
            }
            case KEY_UP : {
                if (key_1 == true){
                    key_2 = true;
                    key_1 = false;
                    button_num = 2;
                }
                else if (key_2 == true){
                    key_1 = true;
                    key_2 = false;
                    button_num = 1;
                }
                break;
            }
        }
    }
}

int levels(){
    clear();
    sleep(0.1);
    int color = 2;
    int how_to_draw = 0;
    draw_game_box(color, how_to_draw);
    
    bool key_1 = true;
    bool key_2 = false;
    int level = 1;
    draw_level_1();
    draw_level_2();
    refresh();
    //buttons_move
    while (true){
        if (key_1 == true){
            attron(COLOR_PAIR(2));
            draw_level_1();
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
            draw_level_2();
            refresh();
            attroff(COLOR_PAIR(1));
        }
        else if (key_2 == true){
            attron(COLOR_PAIR(1));
            draw_level_1();
            attroff(COLOR_PAIR(1));
            attron(COLOR_PAIR(2));
            draw_level_2();
            refresh();
            attroff(COLOR_PAIR(2));
        }    
        int input = getch();
        switch(input){
            case '\n' :{
                if (level == 1){
                    return 1;
                }
                else if (level == 2){
                    return 2;
                }
                break;
            }
            case KEY_DOWN : {
                if (key_1 == true){ 
                    key_2 = true;
                    key_1 = false;
                    level = 2;
                }
                else if(key_2 == true){
                    key_1 = true;
                    key_2 = false;  
                    level = 1;
                }                    
                break;
            }
            case KEY_UP : {
                if (key_1 == true){
                    key_2  = true;                        
                    key_1 = false;
                    level = 2;                       
                }                  
                else if (key_2 == true){
                    key_1 = true;
                    key_2 = false;
                    level = 1;    
                }
                break;
            }
        }
    }
}

void draw_score(unsigned int score, unsigned int max_score_1, unsigned int max_score_2, int lvl){
    mvprintw((LINES/2 - 11/2)+2, (COLS/2 - 66/2)+3, "score: %u", score);
    if (lvl == 1){
        mvprintw((LINES/2 - 11/2)+2, (COLS/2 + 66/2)-19, "max.result: %u", max_score_1);
    }
    else if (lvl == 2){
        mvprintw((LINES/2 - 11/2)+2, (COLS/2 + 66/2)-19, "max.result: %u", max_score_2);
    }
}

int lost_menu(){
    int color = 2;
    int how_to_draw = 0;
    draw_game_box(color, how_to_draw);
    int button_num = 1;
    bool key_1 = true;
    bool key_2 = false;
    draw_restart_game();
    draw_go_to_menu();
    while (true){
        if (key_1 == true){
            attron(COLOR_PAIR(2));
            draw_restart_game();
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
            draw_go_to_menu();
            attroff(COLOR_PAIR(1));
            refresh();
        }
        else if (key_2 == true){
            attron(COLOR_PAIR(1));
            draw_restart_game();
            attroff(COLOR_PAIR(1));  
            attron(COLOR_PAIR(2));
            draw_go_to_menu();
            attroff(COLOR_PAIR(2));
            refresh();
        }
        int input = getch();
        switch(input){
            case '\n' :{
                if (button_num == 1){
                    return 1;
                }
                else if (button_num == 2){
                    return 2;
                }
                break;
            }
            case KEY_DOWN : {
                if (key_1 == true){
                    key_2 = true;
                    key_1 = false;
                    button_num = 2;
                }
                else if(key_2 == true){
                    key_1 = true;
                    key_2 = false;
                    button_num = 1;
                }
                break;
            }
            case KEY_UP : {
                if (key_1 == true){
                    key_2 = true;
                    key_1 = false;
                    button_num = 2;
                }
                else if (key_2 == true){
                    key_1 = true;
                    key_2 = false;
                    button_num = 1;
                }
                break;
            }
        }
    }
}

void draw_start_game(){
    mvprintw(LINES/2-1, COLS/2-7, "+------------+");
    mvprintw(LINES/2+0, COLS/2-7, "| start game |");
    mvprintw(LINES/2+1, COLS/2-7, "+------------+");
}

void draw_exit(){
    mvprintw(LINES/2+4, COLS/2-7, "+------------+");
    mvprintw(LINES/2+5, COLS/2-7, "|    exit    |");
    mvprintw(LINES/2+6, COLS/2-7, "+------------+");
}

void draw_restart_game(){
    mvprintw(LINES/2-3, COLS/2-7, "+------------+");
    mvprintw(LINES/2-2, COLS/2-7, "| play again |");
    mvprintw(LINES/2-1, COLS/2-7, "+------------+");
}

void draw_go_to_menu(){
    mvprintw(LINES/2+1, COLS/2-7, "+------------+");
    mvprintw(LINES/2+2, COLS/2-7, "|    menu    |");
    mvprintw(LINES/2+3, COLS/2-7, "+------------+");
}

void draw_level_1(){
    mvprintw(LINES/2-3, COLS/2-7,"+------------+");
    mvprintw(LINES/2-2, COLS/2-7, "|   level 1  |");
    mvprintw(LINES/2-1, COLS/2-7, "+------------+");
}

void draw_level_2(){
    mvprintw(LINES/2+1, COLS/2-7, "+------------+");
    mvprintw(LINES/2+2, COLS/2-7, "|   level 2  |");
    mvprintw(LINES/2+3, COLS/2-7, "+------------+");
}

void draw_menu_box(){
    int x_right = COLS/2 + 66/2;
    int x_left = (COLS/2 - 66/2)-1;
    int y_lower = LINES/2 + 28/2;
    int y_higher = LINES/2 - 28/2;
    
    attron(COLOR_PAIR(1));
    mvprintw(y_lower, x_left, "+");
    mvprintw(y_lower, x_right, "+");
    for (int x = x_left+1; x < x_right; x++){
        mvprintw(y_higher, x, "-");
        mvprintw(y_lower, x, "-");
    }
    mvprintw(y_higher, x_left, "+");
    mvprintw(y_higher, x_right, "+");
    for (int y = y_higher+1; y < y_lower; y++){
        mvprintw(y, x_left, "|");
        mvprintw(y, x_right, "|");
    }

    attroff(COLOR_PAIR(1));
    //second
    attron(COLOR_PAIR(2));
    mvprintw(y_lower+1, x_left-2, "+");
    mvprintw(y_lower+1, x_right+2, "+");
    for (int x = x_left-1; x < x_right+2; x++){
        mvprintw(y_higher-1, x, "-");
        mvprintw(y_lower+1, x, "-");
    }
    mvprintw(y_higher-1, x_left-2, "+");
    mvprintw(y_higher-1, x_right+2, "+");
    for (int y = y_higher; y < y_lower+1; y++){
        mvprintw(y, x_left-2, "|");
        mvprintw(y, x_right+2, "|");
    }
    attroff(COLOR_PAIR(2));
}

void draw_game_box(int color, int how_to_draw){
    int x_right = COLS/2 + 66/2;
    int x_left = (COLS/2 - 66/2)-1;
    int y_lower = LINES/2 + 11/2;
    int y_higher = LINES/2 - 11/2;
    clear();
    refresh();
    sleep(0.5);
    //first
    
    refresh();
    color == 1 ? attron(COLOR_PAIR(2)) : attron(COLOR_PAIR(1));
    mvprintw(y_lower, x_left, "+");
    mvprintw(y_lower, x_right, "+");
    for (int x = x_left+1; x < x_right; x++){
        mvprintw(y_higher, x, "-");
        mvprintw(y_lower, x, "-");
    }
    mvprintw(y_higher, x_left, "+");
    mvprintw(y_higher, x_right, "+");
    for (int y = y_higher+1; y < y_lower; y++){
        mvprintw(y, x_left, "|");
        mvprintw(y, x_right, "|");
    }
    color == 1 ? attroff(COLOR_PAIR(2)) : attron(COLOR_PAIR(1));
    //second
    color == 1 ? attron(COLOR_PAIR(1)) : attron(COLOR_PAIR(2));
    if (how_to_draw == 0){
        mvprintw(y_lower+1, x_left-2, "+");
        mvprintw(y_lower+1, x_right+2, "+");
        for (int x = x_left-1; x < x_right+2; x++){
            mvprintw(y_higher-1, x, "-");
            mvprintw(y_lower+1, x, "-");
        }
        mvprintw(y_higher-1, x_left-2, "+");
        mvprintw(y_higher-1, x_right+2, "+");
        for (int y = y_higher; y < y_lower+1; y++){
            mvprintw(y, x_left-2, "|");
            mvprintw(y, x_right+2, "|");
        }
    }
    else{
        mvprintw(y_lower+2, x_left-2, "+");
        mvprintw(y_lower+2, x_right+2, "+");
        for (int x = x_left-1; x < x_right+2; x++){
            mvprintw(y_higher-1, x, "-");
            mvprintw(y_lower+2, x, "-");
        }
        mvprintw(y_higher-1, x_left-2, "+");
        mvprintw(y_higher-1, x_right+2, "+");
        for (int y = y_higher; y < y_lower+2; y++){
            mvprintw(y, x_left-2, "|");
            mvprintw(y, x_right+2, "|");
        }
    }
    color == 1 ? attroff(COLOR_PAIR(1)) : attroff(COLOR_PAIR(2));
}
