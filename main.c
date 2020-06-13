#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define X_FIELD -1
#define EMPTY_FIELD 0
#define O_FIELD 1

#define SCREEN_WIDTH_THIRD SCREEN_WIDTH / 3
#define SCREEN_HEIGHT_THIRD SCREEN_HEIGHT / 3

/*
 *  Draw x (cross) player
 */
void DrawX(uint8_t square, SDL_Renderer *renderer)
{
        // Draw NW to SE
        SDL_RenderDrawLine(renderer,
                           SCREEN_WIDTH_THIRD * (square % 3),
                           SCREEN_HEIGHT_THIRD * (square / 3),
                           SCREEN_WIDTH_THIRD * ((square % 3) + 1),
                           SCREEN_HEIGHT_THIRD * ((square / 3) + 1));
        // Draw NE to SW
        SDL_RenderDrawLine(renderer,
                           SCREEN_WIDTH_THIRD * ((square % 3) + 1),
                           SCREEN_HEIGHT_THIRD * (square / 3),
                           SCREEN_WIDTH_THIRD * (square % 3),
                           SCREEN_HEIGHT_THIRD * ((square / 3) + 1));
}

/*
 *  Draw o (circle) player
 */
void DrawO(uint8_t square, SDL_Renderer *renderer)
{
        int32_t centre_x = SCREEN_WIDTH_THIRD * (square % 3) + (SCREEN_WIDTH_THIRD / 2);
        int32_t centre_y = SCREEN_HEIGHT_THIRD * (square / 3) + (SCREEN_HEIGHT_THIRD / 2);
        int32_t x = (SCREEN_WIDTH < SCREEN_HEIGHT ? SCREEN_WIDTH_THIRD / 2 : SCREEN_HEIGHT_THIRD / 2);
        int32_t y = 0;
        int32_t error = 0;

        // Mid-Point Circle Drawing Algorithm
        while (x >= y)
        {
                SDL_RenderDrawPoint(renderer, centre_x + x, centre_y - y);
                SDL_RenderDrawPoint(renderer, centre_x + x, centre_y + y);
                SDL_RenderDrawPoint(renderer, centre_x - x, centre_y - y);
                SDL_RenderDrawPoint(renderer, centre_x - x, centre_y + y);
                SDL_RenderDrawPoint(renderer, centre_x + y, centre_y - x);
                SDL_RenderDrawPoint(renderer, centre_x + y, centre_y + x);
                SDL_RenderDrawPoint(renderer, centre_x - y, centre_y - x);
                SDL_RenderDrawPoint(renderer, centre_x - y, centre_y + x);

                if (error <= 0)
                {
                        ++y;
                        error += 2 * y + 1;
                }

                if (error > 0)
                {
                        --x;
                        error -= 2 * x + 1;
                }
        }
}

/*
 *  Draw the winner in the center on a rectangle covering the middle on the screen 
 */
void DrawWinner(int8_t winner, SDL_Renderer *renderer)
{
        SDL_Rect rectangle;
        rectangle.x = SCREEN_WIDTH / 4;
        rectangle.y = SCREEN_HEIGHT / 4;
        rectangle.w = SCREEN_WIDTH / 2;
        rectangle.h = SCREEN_HEIGHT / 2;

        SDL_SetRenderDrawColor(renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rectangle);

        SDL_SetRenderDrawColor(renderer, 20, 20, 20, SDL_ALPHA_OPAQUE);
        if (winner == X_FIELD)
        {
                DrawX(4, renderer);
        }
        else
        {
                DrawO(4, renderer);
        }
}

/*
 *  Convert x, y coordinates to grid number (0-8).
 *  Grids are numbered in a left to right, top to bottom pattern
 *  from top left (0) to bottom right (8)
 */
int8_t GridPosition(int32_t x, int32_t y)
{
        x /= SCREEN_WIDTH_THIRD;
        y /= SCREEN_HEIGHT_THIRD;
        return y * 3 + x;
}

/*
 *  Check for a winner.
 *  If a player has won, return that player, otherwise a space (' ')
 */
int8_t Winner(int8_t board[9])
{
        uint8_t win_conditions[8][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};
        for (uint8_t i = 0; i < 8; ++i)
        {
                if (board[win_conditions[i][0]] != EMPTY_FIELD &&
                    board[win_conditions[i][0]] == board[win_conditions[i][1]] &&
                    board[win_conditions[i][0]] == board[win_conditions[i][2]])
                        return board[win_conditions[i][2]];
        }
        return EMPTY_FIELD;
}

int main(int argc, char *argv[])
{
        int32_t mouse_position_x;
        int32_t mouse_position_y;
        uint32_t mouse_button;
        int8_t board[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        int8_t winner = EMPTY_FIELD;
        int8_t turn = X_FIELD; /* Starting player */
        int8_t current_position;
        int8_t i;

        if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
        {
                SDL_Window *window = NULL;
                SDL_Renderer *renderer = NULL;

                if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer) == 0)
                {
                        SDL_bool done = SDL_FALSE;

                        while (!done)
                        {
                                SDL_Event event;

                                SDL_SetRenderDrawColor(renderer, 127, 127, 127, SDL_ALPHA_OPAQUE);
                                SDL_RenderClear(renderer);

                                // Draw grid
                                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                                SDL_RenderDrawLine(renderer, SCREEN_WIDTH_THIRD, 0, SCREEN_WIDTH_THIRD, SCREEN_HEIGHT);
                                SDL_RenderDrawLine(renderer, SCREEN_WIDTH_THIRD * 2, 0, SCREEN_WIDTH_THIRD * 2, SCREEN_HEIGHT);
                                SDL_RenderDrawLine(renderer, 0, SCREEN_HEIGHT_THIRD, SCREEN_WIDTH, SCREEN_HEIGHT_THIRD);
                                SDL_RenderDrawLine(renderer, 0, SCREEN_HEIGHT_THIRD * 2, SCREEN_WIDTH, SCREEN_HEIGHT_THIRD * 2);

                                // Draw claimed positions
                                for (i = 0; i < 9; i++)
                                {
                                        switch (board[i])
                                        {
                                        case X_FIELD:
                                                DrawX(i, renderer);
                                                break;
                                        case O_FIELD:
                                                DrawO(i, renderer);
                                                break;
                                        default:
                                                // Draw nothing
                                                break;
                                        }
                                }

                                if (winner != EMPTY_FIELD)
                                {
                                        DrawWinner(winner, renderer);
                                }

                                // Send to screen
                                SDL_RenderPresent(renderer);

                                mouse_button = SDL_GetMouseState(&mouse_position_x, &mouse_position_y);

                                while (SDL_PollEvent(&event))
                                {
                                        if (event.type == SDL_QUIT)
                                        {
                                                done = SDL_TRUE;
                                        }

                                        // Player making a move
                                        if (event.type == SDL_MOUSEBUTTONUP)
                                        {
                                                if (mouse_button & SDL_BUTTON(SDL_BUTTON_LEFT))
                                                {
                                                        if (winner == EMPTY_FIELD)
                                                        {
                                                                current_position = GridPosition(mouse_position_x, mouse_position_y);
                                                                if (board[current_position] == 0)
                                                                {
                                                                        board[current_position] = turn;
                                                                        turn = (turn == X_FIELD ? O_FIELD : X_FIELD);
                                                                        winner = Winner(board);
                                                                }
                                                        }
                                                }
                                        }
                                }

                                SDL_Delay(1000 / 60);
                        }
                }

                if (renderer)
                {
                        SDL_DestroyRenderer(renderer);
                }
                if (window)
                {
                        SDL_DestroyWindow(window);
                }
        }
        SDL_Quit();
        return 0;
}
