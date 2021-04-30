#include <stdlib.h>
#include <random>

#include "game.hpp"

const int WALL_THICKNESS = 20;
const int GRID_SIZE = 40;
const float PADDLE_HEIGHT = 100.0f;
const int WINDOW_WIDTH = 480 * 2;
const int WINDOW_HEIGHT = 480 * 2;

Corner randomCorner() {

    int actualWidth = WINDOW_WIDTH - 2 * WALL_THICKNESS;

    int i = rand() % (actualWidth / GRID_SIZE);
    int j = rand() % (actualWidth / GRID_SIZE); 

	return {
		WALL_THICKNESS + i * GRID_SIZE,
		WALL_THICKNESS + j * GRID_SIZE
	};

}



/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
bool Game::Initialize() {

	int sdlInitializationResult = SDL_Init(SDL_INIT_VIDEO);

    if(sdlInitializationResult != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    this->window = SDL_CreateWindow(
        "Willy's and Sebas's Snake", // Title
        100,                        // Top left x-coordinate of window
        100,                        // Top left y-coordinate of window
        WINDOW_WIDTH,               // Width of window
        WINDOW_HEIGHT,              // Height of window
        0                           // Window creation flags, ex SDL_WINDOW_FULLSCREEN, etc
    );

    if(!this->window) {
        SDL_Log("Failed to create Window: %s", SDL_GetError());
        return false;
    }

    this->renderer = SDL_CreateRenderer(
        this->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC // imwiwiim, you should read what vsync is
    );

    if(!this->renderer) {
        SDL_Log("Failed to create Renderer: %s", SDL_GetError());
        return false;
    }

    // Render food
    // Render initial position of snake

    Corner c1 = {WALL_THICKNESS + 3 * GRID_SIZE, WALL_THICKNESS + 4 * GRID_SIZE};
    Corner c2 = {WALL_THICKNESS + 3 * GRID_SIZE, WALL_THICKNESS + 5 * GRID_SIZE};
    Corner c3 = {WALL_THICKNESS + 3 * GRID_SIZE, WALL_THICKNESS + 6 * GRID_SIZE};

    this->snake.push_back(c1);
    this->snake.push_back(c2);
    this->snake.push_back(c3);

    this->food = randomCorner();

	return true;

}

/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void Game::Shutdown() {
    SDL_DestroyWindow(this->window);
    SDL_DestroyRenderer(this->renderer);
    SDL_Quit();
}

/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void Game::RunLoop() {
    
    while(this->isRunning) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }

}

/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void Game::ProcessInput() {

    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                this->isRunning = false;
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);

    if(state[SDL_SCANCODE_ESCAPE])
        this->isRunning = false;

    // this->paddleDirection1 = 0;

    if(state[SDL_SCANCODE_W])
        this->newDirection = UP;

    if(state[SDL_SCANCODE_S])
        this->newDirection = DOWN;

    if(state[SDL_SCANCODE_A])
        this->newDirection = LEFT;

    if(state[SDL_SCANCODE_D])
        this->newDirection = RIGHT;

	// this->paddleDirection2 = 0;

    // if(state[SDL_SCANCODE_I])
    //     this->paddleDirection2 -= 1;

    // if(state[SDL_SCANCODE_K])
    //     this->paddleDirection2 += 1;

}


/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void Game::UpdateGame() {

	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), this->ticksCount + 60 * 3))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - this->ticksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
		deltaTime = 0.05f;

	// Update tick counts (for next frame)
	this->ticksCount = SDL_GetTicks();

    //SDL_Log("newDirection: %d %d", this->newDirection.first, this->newDirection.second);
    //SDL_Log("currentDirection: %d %d", this->currentDirection.first, this->currentDirection.second);

    if(this->newDirection == UP) {
        SDL_Log("Current head position: %d %d", this->snake[0].x, this->snake[0].y);
        for(int i = this->snake.size(); i > 0; i--)
            this->snake[i] = this->snake[i - 1];
        // this->snake[2] = this->snake[1];
        // this->snake[1] = this->snake[0];
        this->snake[0].y = this->snake[0].y - GRID_SIZE;
        SDL_Log("New head position: %d %d", this->snake[0].x, this->snake[0].y);

        // for(int i = 1; i < this->snake.size() - 1; i++) {
        //     this->snake[i] = this->snake[i + 1];
        // }

        //this->snake.pop_back();

        //isRunning = false;
    }

    if(this->newDirection == LEFT) {
        SDL_Log("Current head position: %d %d", this->snake[0].x, this->snake[0].y);
        for(int i = this->snake.size(); i > 0; i--)
            this->snake[i] = this->snake[i - 1];
        this->snake[0].x = this->snake[0].x - GRID_SIZE;
        SDL_Log("New head position: %d %d", this->snake[0].x, this->snake[0].y);

        // for(int i = 1; i < this->snake.size() - 1; i++) {
        //     this->snake[i] = this->snake[i + 1];
        // }

        //this->snake.pop_back();

        //isRunning = false;
    }

    if(this->newDirection == RIGHT) {
        SDL_Log("Current head position: %d %d", this->snake[0].x, this->snake[0].y);
        for(int i = this->snake.size(); i > 0; i--)
            this->snake[i] = this->snake[i - 1];
        this->snake[0].x = this->snake[0].x + GRID_SIZE;
        SDL_Log("New head position: %d %d", this->snake[0].x, this->snake[0].y);

        // for(int i = 1; i < this->snake.size() - 1; i++) {
        //     this->snake[i] = this->snake[i + 1];
        // }

        //this->snake.pop_back();

        //isRunning = false;
    }

    if(this->newDirection == DOWN) {
        SDL_Log("Current head position: %d %d", this->snake[0].x, this->snake[0].y);
        for(int i = this->snake.size(); i > 0; i--)
            this->snake[i] = this->snake[i - 1];
        this->snake[0].y = this->snake[0].y + GRID_SIZE;
        SDL_Log("New head position: %d %d", this->snake[0].x, this->snake[0].y);

        // for(int i = 1; i < this->snake.size() - 1; i++) {
        //     this->snake[i] = this->snake[i + 1];
        // }

        //this->snake.pop_back();

        //isRunning = false;
    }

    if(this->snake[0].x == this->food.x && this->snake[0].y == this->food.y) {
        this->snake.insert(this->snake.begin(), {this->food.x, this->food.y});
        this->food = randomCorner();

    }
        SDL_Log("\n\n\n*************************\n\n\n");

	// updatePaddlePosition(this->paddleDirection1, deltaTime, this->paddlePosition1);
	// updatePaddlePosition(this->paddleDirection2, deltaTime, this->paddlePosition2);
	
	// // Update ball position based on ball velocity

	// for(int i = 0; i < this->currentBalls; i++) {
	// 	auto& ballPosition = this->ballPositions[i];
	// 	auto ballVelocity = this->ballVelocities[i];

	// 	ballPosition.x += ballVelocity.x * deltaTime;
	// 	ballPosition.y += ballVelocity.y * deltaTime;

    //     if(ballPosition.x <= 0) {
    //         this->currentBalls--;
    //         this->ballPositions.erase(this->ballPositions.begin() + i);
    //         this->ballVelocities.erase(this->ballVelocities.begin() + i);
    //     }

    //     if(ballPosition.x >= WINDOW_WIDTH) {
    //         this->currentBalls--;
    //         this->ballPositions.erase(this->ballPositions.begin() + i);
    //         this->ballVelocities.erase(this->ballVelocities.begin() + i);
    //     }

	// }

	// // How does the ball bounce with the walls, paddles and other balls

    // if(this->currentBalls == 0)
    //     isRunning = false;
	
}

/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void renderWall(SDL_Renderer* renderer, int x, int y, int w, int h) {

	SDL_Rect wall;

    wall.x = x;        // Top left x
    wall.y = y;        // Top left y
    wall.w = w;        // Width
    wall.h = h;        // Height

    
    SDL_RenderFillRect(renderer, &wall);

}

/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void Game::GenerateOutput() {

    /*
     * 1. Clear the back buffer to a color
     * 2. Draw the entire game scene
     * 3. Swap the front buffer and back buffer
    **/

    // Clear the back buffer to a color
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);

    SDL_RenderClear(this->renderer);

    // Draw the entire game scene

	// Draw walls, bottom and top
	SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);

	renderWall(this->renderer, 0, 0, WINDOW_WIDTH, WALL_THICKNESS);
	renderWall(this->renderer, 0, WINDOW_HEIGHT - WALL_THICKNESS, WINDOW_WIDTH, WALL_THICKNESS);
    renderWall(this->renderer, 0, 0, WALL_THICKNESS, WINDOW_HEIGHT);
    renderWall(this->renderer, WINDOW_WIDTH - WALL_THICKNESS, 0, WALL_THICKNESS, WINDOW_HEIGHT);

    int actualWidth = WINDOW_WIDTH - 2 * WALL_THICKNESS;
    int actualHeight = WINDOW_HEIGHT - 2 * WALL_THICKNESS;

    for(int i = 1; i <= actualWidth / GRID_SIZE; i++) {

        SDL_RenderDrawLine(this->renderer, 
            0, 
            WALL_THICKNESS + (GRID_SIZE * i), 
            WINDOW_WIDTH - WALL_THICKNESS, 
            WALL_THICKNESS + (GRID_SIZE * i));

        SDL_RenderDrawLine(this->renderer, 
            WALL_THICKNESS + (GRID_SIZE * i), 
            0, 
            WALL_THICKNESS + (GRID_SIZE * i), 
            WINDOW_HEIGHT - WALL_THICKNESS);

    }

    SDL_SetRenderDrawColor(this->renderer, 0, 255, 0, 255);
    
    for(Corner snakeCorner : snake) {

        SDL_Rect snakeGrid;

        snakeGrid.x = snakeCorner.x;            // Top left x
        snakeGrid.y = snakeCorner.y;            // Top left y
        snakeGrid.w = GRID_SIZE;                // Width
        snakeGrid.h = GRID_SIZE;                // Height

        SDL_RenderFillRect(this->renderer, &snakeGrid);

    }

    SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
    SDL_Rect foodGrid;

    foodGrid.x = this->food.x;              // Top left x
    foodGrid.y = this->food.y;              // Top left y
    foodGrid.w = GRID_SIZE;                 // Width
    foodGrid.h = GRID_SIZE;                 // Height

    SDL_RenderFillRect(this->renderer, &foodGrid);



    // Swap the front buffer and back buffer
    SDL_RenderPresent(this->renderer);

}