#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

const int ScreenWidth = 800;
const int ScreenHeight = 600;
bool isRunning;
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture* playertex;
SDL_Texture* enemytex;
SDL_Rect p_srcR, p_destR;
SDL_Rect e_srcR, e_destR;
static SDL_Event event;
int counter;

void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
void handleEvents(void);
static bool AABB(SDL_Rect recA, SDL_Rect recB);
void update(void);
static SDL_Texture* LoadTexture(const char* fileName, SDL_Renderer* ren);
void render(void);
void clean(void);
bool running(void) { return isRunning; }



int main( int argc, char* args[] )
{
	(void) argc;
	(void) args;

	const int FPS = 60;
	const int frameDelay = 500 / FPS;
	uint32_t frameStart;
	int frameTime;

	init("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenWidth, ScreenHeight, false);

	bool flag = true;

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
	p_destR.w = 100;
	p_destR.h = 100;
	p_destR.y = ScreenHeight / 2;

	//check the movement of the player
	if(event.type == SDL_KEYDOWN && event.key.repeat == 0) {
		switch(event.key.keysym.sym) {
			case SDLK_RIGHT:
				p_destR.x += 5;
				break;
			case SDLK_UP:
				p_destR.y -= 150;
				p_destR.x += 3;
				break;
			default:
				break;
		}
	}

	else if(event.type == SDL_KEYUP && event.key.repeat == 0) {
		switch(event.key.keysym.sym) {
			case SDLK_UP:
				p_destR.y = ScreenHeight / 2;
				break;
			default:
				break;
		}
	}


	e_destR.w = 45;
	e_destR.h = 45;
	e_destR.x = ScreenWidth / 2 - e_destR.w / 2;
	e_destR.y = ScreenHeight / 1.6 - e_destR.h / 1.6;

	//checking if hit the car
	if(AABB(p_destR, e_destR)) {
		printf("\n\t*Collision Detected!*\n");
		printf("\t*Game Over!*\n\n");
		isRunning = false;
	}

}

void render(void){
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, playertex, NULL, &p_destR);
	SDL_RenderCopy(renderer, enemytex, NULL, &e_destR);
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
