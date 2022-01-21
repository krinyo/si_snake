#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#define SNAKE_TEXTURE '*'
#define FOOD_TEXTURE '@'
struct snake_part{
	int x;
	int y;
	struct snake_part *next;
};
struct food{
	int x;
	int y;
	int located;
};
enum directions{
	right, bottom, left, top
};
struct snake_part *add_part(struct snake_part *last, int x, int y)
{
	struct snake_part *tmp;
	tmp = last == NULL ? last : last->next;
	tmp = malloc(sizeof(struct snake_part));
	tmp->x = x;
	tmp->y = y;
	tmp->next = last;
	return tmp;
}
void put_snake(struct snake_part *last)
{
	while(last != NULL){
		move(last->y, last->x);
		addch(SNAKE_TEXTURE);
		last = last->next;
	}
}
void hide_snake(struct snake_part *last)
{
	while(last != NULL){
		move(last->y, last->x);
		addch(' ');
		last = last->next;
	}
}
void move_snake(struct snake_part *first, struct snake_part *last, enum directions dir)
{
	/*DO the handler for collision here*/
	while(last->next != NULL){
		last->x = last->next->x;
		last->y = last->next->y;
		last = last->next;
	}
	switch(dir){
		case right:
			first->x ++;
			break;
		case bottom:
			first->y ++;
			break;
		case left:
			first->x --;
			break;
		case top:
			first->y --;
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
			dir = bottom;
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
struct food *try_to_spawn_food(int rows, int cols, struct food *apple)
{
	//int rndY = 1 + (int)(rows * rand()/(RAND_MAX+1.0));
	//int rndX = 1 + (int)(cols * rand()/(RAND_MAX+1.0)); just dont know why it doesn't work
	if(!apple->located){
		apple->y = rand() % rows;
		apple->x = rand() % cols;
		move(apple->y, apple->x);
		addch(FOOD_TEXTURE);
		apple->located = 1;
		return apple;
	}
	return apple;
}
struct snake_part *try_to_eat(struct snake_part *first,
		struct snake_part *last, struct food *apple)
{
	if(first->x == apple->x && first->y == apple->y){
		last = add_part(last, last->x - 1, last->y);
	}
	return last;
}
struct food *permit_food_spawn(struct food *apple)
{
	apple->located = 0;
	return apple;
}

int main()
{        
	srand(time(NULL));

        initscr();
        cbreak();
        noecho();
        keypad(stdscr, 1);
        /*terminal and lib setup*/
	
        int row, col;
        getmaxyx(stdscr, row, col);
        /*getting width and height of screen*/

	struct timespec tim;
	tim.tv_sec = 0;
	//tim.tv_nsec = 500000000L;
	//0.5 seconds
	tim.tv_nsec = 80000000L;
	//0.08 seconds

	struct snake_part *head = add_part(NULL, col/2, row/2);
	struct snake_part *tail = head;
	struct snake_part *tmp;
	struct food *apple = malloc(sizeof(struct food));
	apple->located = 0;
	enum directions current_direction = right;
	tail = add_part(tail, tail->x - 1, tail->y);
	
	put_snake(tail);
	refresh();
	while(true){
		apple = try_to_spawn_food(row, col, apple);
		tmp = tail;
		tail = try_to_eat(head, tail, apple);
		if(tmp != tail){
			apple = permit_food_spawn(apple);
		}
		hide_snake(tail);
		current_direction = get_direction(current_direction);
		move_snake(head, tail, current_direction);
		put_snake(tail);
		
		curs_set(0);
		refresh();
		nanosleep(&tim, &tim);	
	}
	endwin();
	return 0;
}
