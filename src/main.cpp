#include <BearLibTerminal.h>
#include <string>
#include <chrono>
#include <thread>
#include "soundManager.cpp"

// Game constants
const int WINDOW_WIDTH = 64;
const int WINDOW_HEIGHT = 32;
const int PLAYER_START_X = WINDOW_WIDTH / 2;
const int PLAYER_START_Y = WINDOW_HEIGHT / 2;

// Player structure
struct Player
{
    int x;
    int y;
    char symbol;
    color_t color;
};

// Game state
struct GameState
{
    bool running;
    Player player;
};

/**
 * @brief Initialize the BearLibTerminal window
 * @return true if initialization was successful
 */
bool initTerminal()
{
    if (!terminal_open())
    {
        return false;
    }

    // Configure terminal window
    std::string config = "window: size=" + std::to_string(WINDOW_WIDTH) + "x" + std::to_string(WINDOW_HEIGHT) + ", title='Spelet Python';";
    config += "font: default;";
    config += "input: filter=[keyboard];";

    terminal_set(config.c_str());

    return true;
}

/**
 * @brief Handle input events
 * @param state Game state to modify
 */
void handleInput(GameState &state)
{
    if (terminal_has_input())
    {
        int key = terminal_read();

        switch (key)
        {
        case TK_CLOSE:
        case TK_ESCAPE:
            state.running = false;
            break;
        case TK_UP:
        case TK_W:
            if (state.player.y > 0)
            {
                state.player.y--;
            }
            break;
        case TK_DOWN:
        case TK_S:
            if (state.player.y < WINDOW_HEIGHT - 1)
            {
                state.player.y++;
            }
            break;
        case TK_LEFT:
        case TK_A:
            if (state.player.x > 0)
            {
                state.player.x--;
            }
            break;
        case TK_RIGHT:
        case TK_D:
            if (state.player.x < WINDOW_WIDTH - 1)
            {
                state.player.x++;
            }
            break;
        case TK_SPACE:
            // Space: no audio action configured
            soundManager::get().play("sounds/ShopTheme.mp3");
            soundManager::get().play("sounds/DudeatronMoneyMoneyMoneyMoney.wav");
            std::cout << "Space Space Space" << std::endl;
            break;
        case TK_TAB:
            // TODO: Open inventory
            // Och hur gör vi det egentligen. Fört måste det ritas på någotvis? Det bör ta upp hela skärmen, eller endast en låda på skärmen.

            break;
        default:
            break;
        }
    }
}

/**
 * @brief Update game logic
 * @param state Game state to update
 * @param deltaTime Time since last update in seconds
 */
void update(GameState &state, float deltaTime)
{
    // Add game logic here
    // For now, this is a simple starter with no complex updates
    (void)deltaTime;

    // No audio cleanup required in this build
    (void)state;
}

/**
 * @brief Render the game
 * @param state Game state to render
 */
void render(const GameState &state)
{
    terminal_clear();

    // Draw border
    terminal_color(color_from_name("gray"));
    for (int x = 0; x < WINDOW_WIDTH; x++)
    {
        terminal_put(x, 0, '-');
        terminal_put(x, WINDOW_HEIGHT - 1, '-');
    }
    for (int y = 0; y < WINDOW_HEIGHT; y++)
    {
        terminal_put(0, y, '|');
        terminal_put(WINDOW_WIDTH - 1, y, '|');
    }
    terminal_put(0, 0, '+');
    terminal_put(WINDOW_WIDTH - 1, 0, '+');
    terminal_put(0, WINDOW_HEIGHT - 1, '+');
    terminal_put(WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1, '+');

    // Draw player
    terminal_color(state.player.color);
    terminal_put(state.player.x, state.player.y, state.player.symbol);

    // (Audio removed) No status message shown

    // Draw instructions
    terminal_color(color_from_name("white"));
    terminal_print(2, WINDOW_HEIGHT - 1, " WASD/Arrows: Move | Space: Action | Tab: Inventory | ESC: Quit ");

    // Draw title
    terminal_print(2, 0, " Spelet Python");

    terminal_refresh();
}

/**
 * @brief Main game loop
 */
int main()
{
    // Initialize terminal
    if (!initTerminal())
    {
        return 1;
    }
    // Init soundManager
    soundManager::get().init();
    // Show a simple splash and block until the user presses a key so the
    // BearLibTerminal window stays open for inspection.
    terminal_clear();
    terminal_color(color_from_name("white"));
    terminal_print(WINDOW_WIDTH / 2 - 10, WINDOW_HEIGHT / 2, " Press any key to start ");
    terminal_refresh();
    // This blocks until an input event (keystroke or close) is received
    terminal_read();

    // Initialize game state
    GameState state;
    state.running = true;
    state.player.x = PLAYER_START_X;
    state.player.y = PLAYER_START_Y;
    state.player.symbol = '@';
    state.player.color = color_from_name("yellow");

    // Game timing
    auto lastTime = std::chrono::high_resolution_clock::now();
    const float targetFPS = 60.0f;
    const float targetFrameTime = 1.0f / targetFPS;

    // Sound manager test

    // Main game loop
    while (state.running)
    {
        // Calculate delta time
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // Process input
        handleInput(state);

        // Update game
        update(state, deltaTime);

        // Render
        render(state);

        // Frame rate limiting
        auto frameEnd = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float>(frameEnd - currentTime).count();
        if (frameTime < targetFrameTime)
        {
            std::this_thread::sleep_for(
                std::chrono::duration<float>(targetFrameTime - frameTime));
        }
    }

    // Cleanup
    terminal_close();

    return 0;
}
