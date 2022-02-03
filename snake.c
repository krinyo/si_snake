#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#define GREEN_SKIN 1
#define MAGENTA_SKIN 2
#define YELLOW_SKIN 3
#define BLACK_SKIN 4
#define RED_SKIN 5
#define CYAN_SKIN 6
#define WHITE_SKIN 7
#define BLUE_SKIN 8
#define ACCELERATION_VALUE 30000L
//#define SNAKE_TEXTURE 'O'
//#define FOOD_TEXTURE 'X'
/*main element of the list is the snake tail*/
enum directions{
	right, bot, left, top
};
struct snake_part{
	int x;
	int y;
	struct snake_part *next;
	int skin_number;	
};
struct food{
	int x;
	int y;
	int located;
};
struct snake_part *add_snake_part(struct snake_part *last, int x, int y)
{
	struct snake_part *tmp;
	int skin;
        tmp = last == NULL ? last : last->next;
        skin = last == NULL ? GREEN_SKIN : last->skin_number;
	tmp = malloc(sizeof(struct snake_part));
	tmp->x = x;
        tmp->y = y;
        tmp->next = last;
	tmp->skin_number = skin;
        return tmp;
}
void put_snake(struct snake_part *last)
{
        while(last != NULL){
                move(last->y, last->x);
		attrset(COLOR_PAIR(last->skin_number));
                addch(' ');
                last = last->next;
        }
}
void hide_snake(struct snake_part *last)
{
        while(last != NULL){
                move(last->y, last->x);
		attrset(COLOR_PAIR(BLUE_SKIN));
                addch(' ');
                last = last->next;
        }
}
void move_snake(struct snake_part *first, struct snake_part *last,
		enum directions dir, int cols, int rows)
{
        while(last->next != NULL){
                last->x = last->next->x;
                last->y = last->next->y;
                last = last->next;
        }
        switch(dir){
                case right:
                        first->x = first->x == cols - 1 ? 0 : first->x + 1;
                        break;
                case bot:
                        first->y = first->y == rows - 1 ? 0 : first->y + 1;
                        break;
                case left:
                        first->x = (first->x - 1) == 0 ? cols - 1 : first->x - 1;
                        break;
                case top:
                        first->y = (first->y - 1) == 0 ? rows - 1 : first->y - 1;
                        break;
        }
}
enum directions get_direction(enum directions dir)
{
        timeout(0);
        int ch = getch();
        switch(ch){
                case KEY_RIGHT:
                        dir = right;
                        break;
                case KEY_DOWN:
                        dir = bot;
                        break;
                case KEY_LEFT:
                        dir = left;
                        break;
                case KEY_UP:
                        dir = top;
                        break;
                default:
                        dir = dir;
                        break;
        }
        return dir;
}
int try_to_spawn_food(int rows, int cols, struct food *apple,
		struct snake_part *last)
{
        if(!apple->located){
                apple->y = (rand() % (rows - 1)) + 1;
                apple->x = (rand() % (cols - 1)) + 1;
                move(apple->y, apple->x);
		attrset(COLOR_PAIR(RED_SKIN));
                addch(' ');
                apple->located = 1;
		return 1;
        }
	return 0;
}
void change_snake_skin(struct snake_part *last)
{
	if(last->skin_number == 8){
		last->skin_number = 1;
	}
	else{
		last->skin_number++;
	}
}
void snake_acceleration(struct timespec *tim, long number)
{
	if(tim->tv_nsec > 20000000L){
		tim->tv_nsec = tim->tv_nsec - number;
	}
}

struct snake_part *try_to_eat(struct snake_part *first, struct snake_part *last,
		struct food *apple)
{
        if(first->x == apple->x && first->y == apple->y){
                last = add_snake_part(last, last->x - 1, last->y);
		apple->located = 0;
		
		change_snake_skin(last);
		return last;
        }
        return last;
}
int main()
{
	srand(time(NULL));
	initscr();
        cbreak();
        noecho();
        keypad(stdscr, 1);
	start_color();//ADDING COLOR
	init_pair(1, COLOR_GREEN, COLOR_GREEN);//SNAKE SKIN 1
	init_pair(2, COLOR_MAGENTA, COLOR_MAGENTA);//SNAKE SKIN 2
	init_pair(3, COLOR_YELLOW, COLOR_YELLOW);//SNAKE SKIN 3
	init_pair(4, COLOR_BLACK, COLOR_BLACK);//BACKGROUND
	init_pair(5, COLOR_RED, COLOR_RED);//APPLE
	init_pair(6, COLOR_CYAN, COLOR_CYAN);//??
	init_pair(7, COLOR_WHITE, COLOR_WHITE);//??
	init_pair(8, COLOR_BLUE, COLOR_BLUE);//??

	//init_pair(4, COLOR_BLACK, COLOR_WHITE);
	wbkgd(stdscr, COLOR_PAIR(BLUE_SKIN));
        /*terminal and lib setup*/

        struct timespec tim;
        tim.tv_sec = 0;
        //tim.tv_nsec = 500000000L;
        //0.5 seconds
        tim.tv_nsec = 80000000L;
        //0.08 seconds

        int row, col;
        getmaxyx(stdscr, row, col);
        /*getting width and height of screen*/	

	struct snake_part *head = add_snake_part(NULL, col/2, row/2);
	struct snake_part *tail = head;
	tail->skin_number = 1;

	struct food *apple = malloc(sizeof(struct food));
	apple->located = 0;
	enum directions curr_dir = right;

	//char debug_info[256];
	int i;
	for(i = 0; i < 3; i ++){
		tail = add_snake_part(tail, tail->x-1, tail->y);
		change_snake_skin(tail);
	}
	while(true){
		//sprintf(debug_info,"%ld", tim.tv_nsec);
		//move(0, 0);
		//attrset(COLOR_PAIR(4));
		//addstr(debug_info);

		hide_snake(tail);
		curr_dir = get_direction(curr_dir);
		move_snake(head, tail, curr_dir, col, row);
		if(try_to_spawn_food(row, col, apple, tail)){
			apple->located = 1;
		}
		tail = try_to_eat(head, tail, apple);
		put_snake(tail);
		
		snake_acceleration(&tim, ACCELERATION_VALUE);

		curs_set(0);
		refresh();
		nanosleep(&tim, &tim);
	}
	endwin();
	return 0;
}
