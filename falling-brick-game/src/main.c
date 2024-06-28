#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

const int ScreenWidth = 800;
const int ScreenHeight = 600;
bool isRunning;
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture* playertex;
SDL_Texture* enemytex;
SDL_Rect p_srcR, p_destR;
SDL_Rect e_srcR, e_destR[100];
static SDL_Event event;
int counter;
int r_cnt = 0;
int int_w_rock = 45;
int int_h_rock = 45;
int int_w_car = 90;
int int_h_car = 90;
bool mov_rock = false;
int ind_mov_rock = -1;
int rock_dir = 1;

void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
void handleEvents(void);
static bool AABB(SDL_Rect recA, SDL_Rect recB);
void update(void);
static SDL_Texture* LoadTexture(const char* fileName, SDL_Renderer* ren);
void render(void);
void clean(void);
void level_up(void);
bool running(void) { return isRunning; }
bool check_rocks(int r, int x);
void change_txt(int ind);


int main( int argc, char* args[] ) {

	(void) argc;
	(void) args;

	const int FPS = 60;
	const int frameDelay = 500 / FPS;
	uint32_t frameStart;
	int frameTime;

	srand(time(NULL));

	init("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenWidth, ScreenHeight, false);

	bool flag = true;

	level_up();

	while( running() )
	{
		frameStart = SDL_GetTicks();

		//check for quits
		handleEvents();

		//player movement and colission with cars
		update();

		//rendering the screen
		render();

		frameTime = SDL_GetTicks() - frameStart;

		if( frameDelay > frameTime )
		{
			SDL_Delay( frameDelay - frameTime );
		}

		if(p_destR.x >= ScreenWidth / 1.88 && flag)
		{
			printf("\n\t*No Collision Detected!*\n\t*You Won!*\n\n");
			flag = false;
		}
	}

	clean();

	return 0;
}


void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen){

	int flags = 0;
	//check for full screen
	if(fullscreen){
		flags = SDL_WINDOW_FULLSCREEN;
	}

	//initializing SDL
	if(SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		printf("SDL initialized!\n");
		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if(window) {
			printf("Window created!\n");
			}
		renderer = SDL_CreateRenderer(window, -1, 0);
		if(renderer) {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			printf("Renderer created!\n");
			}
		isRunning = true;
	}
	else {
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		isRunning = false;
	}

	//loading textures from assets folder
	playertex = LoadTexture("assets/player.bmp", renderer);
	enemytex = LoadTexture("assets/enemy.bmp", renderer);

}

void handleEvents(void){

	SDL_PollEvent(&event);
	switch(event.type){
		case SDL_QUIT:
			isRunning = false;
			break;
		default:
			break;
	}

}

void update(void){

	if(p_destR.x >= 700) level_up();

	if(mov_rock && ( e_destR[ind_mov_rock].y >= 600 || e_destR[ind_mov_rock].y <= 0 ) ) rock_dir *= -1;

	//check the movement of the player
	if(event.type == SDL_KEYDOWN && event.key.repeat == 0) {
		switch(event.key.keysym.sym) {
			case SDLK_RIGHT:
				p_destR.x += 50;
				break;
			case SDLK_UP :
				if( p_destR.y <= 100) break;
				p_destR.y -= 150;
				p_destR.x += 8;
				break;
			case SDLK_DOWN:
				if( p_destR.y >= 400 ) break;
				p_destR.y += 150;
				p_destR.x += 8;
			default:
				break;
		}
	}

	if(mov_rock) e_destR[ind_mov_rock].y += rock_dir * 3;

	//checking if hit the car
	for(int i = 1; i <= r_cnt; i++) {

		if(AABB(p_destR, e_destR[i])) {
			printf("\n\t*Collision Detected!*\n");
			printf("\t*Game Over!*\n\n");
			isRunning = false;
		}

	}

}

void render(void){

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, playertex, NULL, &p_destR);

	for(int i = 1; i <= r_cnt; i++) {
		if(mov_rock && ind_mov_rock == i) SDL_RenderCopy(renderer, playertex, NULL, &e_destR[i]);
		else SDL_RenderCopy(renderer, enemytex, NULL, &e_destR[i]);
	}

	SDL_RenderPresent(renderer);

}

//cleaning the scren
void clean(void){

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	printf("Game cleaned!\n");

}

//texture manager
static SDL_Texture* LoadTexture(const char* fileName, SDL_Renderer* ren) {

	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = SDL_LoadBMP(fileName);
	if(loadedSurface == NULL) {
		printf("Unable to load image %s! SDL Error: %s\n", fileName, SDL_GetError());
	} else {
		newTexture = SDL_CreateTextureFromSurface(ren, loadedSurface);
		if(newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n", fileName, SDL_GetError());
		}
		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;

}

//colision detection handler
static bool AABB(SDL_Rect recA, SDL_Rect recB) {

	if(
		recA.x + recA.w >= recB.x &&
		recB.x + recB.w >= recA.x &&
		recA.y + recA.h >= recB.y &&
		recB.y + recB.h >= recA.y) {
		return true;
	}
	return false;

}

bool check_rocks(int r, int x){

	int int_x = (ScreenWidth + r) / 2 - int_w_rock / 2;
	int int_y = (ScreenHeight + r) / 1.6 - int_h_rock / 1.6;

	for(int i = x - 1; i >= 1; i--) {

		if( abs(e_destR[i].x - int_x) <= 30 )return false;
		if( abs(e_destR[i].y - int_y) <= 30 )return false;

	}

	return true;

}

void change_txt(int ind){

	e_destR[ind].w = int_w_car;
	e_destR[ind].h = int_h_car;

}


void level_up(void){
	p_destR.w = int_w_car;
	p_destR.h = int_h_car;
	p_destR.x = 0;
	p_destR.y = ScreenHeight / 2;


	if(r_cnt < 5) r_cnt ++;

	for(int i = 1; i <= r_cnt; i++) {
		e_destR[i].w = int_w_rock;
		e_destR[i].h = int_h_rock;


		int r = ( rand() % 600 ) - 300;

		while( ! check_rocks(r, i) ){
			r = ( rand() % 600 ) - 300;
		}

		e_destR[i].x = (ScreenWidth + r) / 2 - e_destR[i].w / 2;
		e_destR[i].y = (ScreenHeight + r) / 1.6 - e_destR[i].h / 1.6;

	}

	if(r_cnt == 5) mov_rock = true;

	if(mov_rock) {
		ind_mov_rock = rand() % 5;
		change_txt(ind_mov_rock);
	}

}

