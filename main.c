#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define PLAYER -1
#define EMPTY_FIELD 0
#define COMPUTER 1

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
        if (winner == PLAYER)
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
 *  If a player has won, return that player
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

int8_t Min(int8_t a, int8_t b)
{
        return (a > b ? b : a);
}

int8_t Max(int8_t a, int8_t b)
{
        return (a > b ? a : b);
}

/*
 *  Minimax calculations 
 */
int8_t Minimax(int8_t board[9], int8_t player, int8_t depth)
{
        int8_t score = 0; /* Initial value is score if tied */
        int8_t current_score;
        int8_t winner = Winner(board);
        if (winner != EMPTY_FIELD || depth == 0)
        {
                /* Let a fast win be worth more than a slower one */
                return winner * depth;
        }

        for (uint8_t i = 0; i < 9; i++)
        {
                if (board[i] == EMPTY_FIELD)
                {
                        board[i] = player;
                        if (player == PLAYER)
                        {
                                current_score = Minimax(board, COMPUTER, depth - 1) - depth;
                                score = Min(current_score, score);
                        }
                        else
                        {
                                current_score = Minimax(board, PLAYER, depth - 1) + depth;
                                score = Max(current_score, score);
                        }
                        board[i] = EMPTY_FIELD;
                }
        }
        return score;
}

/*
 *  Let the computer calculate the best move 
 */
void ComputerMove(int8_t board[9])
{
        int8_t move = 0;
        int8_t score = -20;
        int8_t current_score;
        for (uint8_t i = 0; i < 9; i++)
        {
                if (board[i] == EMPTY_FIELD)
                {
                        board[i] = COMPUTER;
                        current_score = Minimax(board, PLAYER, 8);
                        if (current_score > score)
                        {
                                score = current_score;
                                move = i;
                        }
                        /* If the same score, choose one of the moves randomly */
                        else if (current_score == score)
                        {
                                if (rand() % 2 == 0)
                                {
                                        move = i;
                                }
                        }
                        board[i] = EMPTY_FIELD;
                }
        }
        board[move] = COMPUTER;
}

int main(int argc, char *argv[])
{
        int32_t mouse_position_x;
        int32_t mouse_position_y;
        uint32_t mouse_button;
        int8_t board[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        int8_t winner = EMPTY_FIELD;
        int8_t turn = COMPUTER; /* Starting player */
        int8_t current_position;
        int8_t i;

        srand(time(NULL));

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
                                        case PLAYER:
                                                DrawX(i, renderer);
                                                break;
                                        case COMPUTER:
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

                                if (winner == EMPTY_FIELD && turn == COMPUTER)
                                {
                                        ComputerMove(board);
                                        winner = Winner(board);
                                        turn = PLAYER;
                                }

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
                                                        if (winner == EMPTY_FIELD && turn == PLAYER)
                                                        {
                                                                current_position = GridPosition(mouse_position_x, mouse_position_y);
                                                                if (board[current_position] == 0)
                                                                {
                                                                        board[current_position] = PLAYER;
                                                                        turn = COMPUTER;
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
