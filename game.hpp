#include <SDL2/SDL.h>
#include <vector>
#include <utility>

const std::pair<int, int> UP(0, -1);
const std::pair<int, int> DOWN(0, 1);
const std::pair<int, int> LEFT(-1, 0);
const std::pair<int, int> RIGHT(1, 0);

struct Vector2D {
    float x, y;
};

struct Corner {
    int x, y;
};

class Game {
    public:
        Game() : 
            window(nullptr), 
            renderer(nullptr), 
            ticksCount(0),
            isRunning(true),
            currentDirection(UP),
            newDirection(DOWN)
        {};

        bool Initialize();
        void RunLoop();
        void Shutdown();

    private:
        void ProcessInput();
        void UpdateGame();
        void GenerateOutput();

        SDL_Renderer* renderer;
        SDL_Window* window;
        bool isRunning;

        Uint32 ticksCount;

        std::pair<int, int> currentDirection;
        std::pair<int, int> newDirection;

        std::vector<Corner> snake;
        Corner food;
        
};