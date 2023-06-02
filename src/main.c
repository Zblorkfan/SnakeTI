#include <graphx.h>
#include <keypadc.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/rtc.h>
#include <fileioc.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SNAKE_SIZE 8
#define FOOD_SIZE 8

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    int length;
    Point body[100];
    int dx;
    int dy;
} Snake;

typedef struct {
    int x;
    int y;
    bool active;
} Food;

void drawSnake(Snake* snake) {
    gfx_SetColor(gfx_green);
    for (int i = 0; i < snake->length; i++) {
        gfx_FillRectangle(snake->body[i].x, snake->body[i].y, SNAKE_SIZE, SNAKE_SIZE);
    }
}

void updateSnake(Snake* snake) {
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }
    snake->body[0].x += snake->dx;
    snake->body[0].y += snake->dy;
}

void delay(uint16_t msec);

void generateFood(Food* food, Snake* snake) {
    bool validPos = false;
    while (!validPos) {
        food->x = (rand() % (SCREEN_WIDTH / FOOD_SIZE)) * FOOD_SIZE;
        food->y = (rand() % (SCREEN_HEIGHT / FOOD_SIZE)) * FOOD_SIZE;

        validPos = true;
        for (int i = 0; i < snake->length; i++) {
            if (food->x == snake->body[i].x && food->y == snake->body[i].y) {
                validPos = false;
                break;
            }
        }
    }
    food->active = true;
}

bool checkCollision(Snake* snake, Food* food, int* score) {
    for (int i = 1; i < snake->length; i++) {
        if (snake->body[i].x == snake->body[0].x && snake->body[i].y == snake->body[0].y) {
            return true;  // Collision with snake's body
        }
    }
    if (snake->body[0].x < 0 || snake->body[0].x >= SCREEN_WIDTH || snake->body[0].y < 0 || snake->body[0].y >= SCREEN_HEIGHT) {
        return true;  // Collision with screen boundaries
    }
    if (snake->body[0].x == food->x && snake->body[0].y == food->y) {
        food->active = false;  // Collision with food
        snake->length++;
        (*score)++;
    }
    return false;
}

void drawFood(Food* food) {
    if (food->active) {
        gfx_SetColor(gfx_red);
        gfx_FillRectangle(food->x, food->y, FOOD_SIZE, FOOD_SIZE);
    }
}

void drawGameOverScreen(int score) {
    gfx_ZeroScreen();
    gfx_SetTextScale(2, 2);
    gfx_FillScreen(gfx_white);
    gfx_SetTextFGColor(gfx_black);

    char scoreStr[6] = "0\0\0\0\0\0";
    real_t scored = os_FloatToReal(score);
    os_RealToStr(scoreStr, &scored, 6, 1, 0);

    gfx_PrintStringXY("Game Over", (SCREEN_WIDTH - gfx_GetStringWidth("Game Over")) / 2, SCREEN_HEIGHT / 2 - 56);
    gfx_PrintStringXY("Score: ", (SCREEN_WIDTH - gfx_GetStringWidth("Score: ")) / 2, SCREEN_HEIGHT / 2 + 16);
    gfx_PrintStringXY(scoreStr, (SCREEN_WIDTH - gfx_GetStringWidth(scoreStr)) / 2, SCREEN_HEIGHT / 2 + 48);
    gfx_PrintStringXY("Play Again: [2nd]", (SCREEN_WIDTH - gfx_GetStringWidth("Play Again: [2nd]")) / 2, SCREEN_HEIGHT / 2 + 70);
    gfx_PrintStringXY("Quit: [annul]", (SCREEN_WIDTH - gfx_GetStringWidth("Quit: [annul]")) / 2, SCREEN_HEIGHT / 2 + 90);
}

int main(void) {
    Snake snake;
    snake.length = 3;
    snake.body[0].x = SCREEN_WIDTH / 2;
    snake.body[0].y = SCREEN_HEIGHT / 2;
    snake.body[1].x = snake.body[0].x - SNAKE_SIZE;
    snake.body[1].y = snake.body[0].y;
    snake.body[2].x = snake.body[1].x - SNAKE_SIZE;
    snake.body[2].y = snake.body[1].y;
    snake.dx = SNAKE_SIZE;
    snake.dy = 0;

    Food food;
    food.active = false;

    gfx_Begin();
    gfx_SetDrawBuffer();

    srand(rtc_Time()); // Initialisation de la séquence de nombres aléatoires


    bool gameOver = false;
    int score = 0;

    while (!kb_IsDown(kb_KeyClear)) {
        gfx_ZeroScreen();

        if (!gameOver) {
            if (!food.active) {
                generateFood(&food, &snake);
            }

            if (kb_IsDown(kb_Left) && snake.dx != SNAKE_SIZE) {
                snake.dx = -SNAKE_SIZE;
                snake.dy = 0;
            }
            if (kb_IsDown(kb_Right) && snake.dx != -SNAKE_SIZE) {
                snake.dx = SNAKE_SIZE;
                snake.dy = 0;
            }
            if (kb_IsDown(kb_Up) && snake.dy != SNAKE_SIZE) {
                snake.dx = 0;
                snake.dy = -SNAKE_SIZE;
            }
            if (kb_IsDown(kb_Down) && snake.dy != -SNAKE_SIZE) {
                snake.dx = 0;
                snake.dy = SNAKE_SIZE;
            }

            updateSnake(&snake);
            drawSnake(&snake);
            drawFood(&food);

            if (checkCollision(&snake, &food, &score)) {
                gameOver = true;
            }
        } else {
            drawGameOverScreen(score);
            if (kb_IsDown(kb_2nd)) {
                // Restart the game
                snake.length = 3;
                snake.body[0].x = SCREEN_WIDTH / 2;
                snake.body[0].y = SCREEN_HEIGHT / 2;
                snake.body[1].x = snake.body[0].x - SNAKE_SIZE;
                snake.body[1].y = snake.body[0].y;
                snake.body[2].x = snake.body[1].x - SNAKE_SIZE;
                snake.body[2].y = snake.body[1].y;
                snake.dx = SNAKE_SIZE;
                snake.dy = 0;

                food.active = false;
                score = 0;
                gameOver = false;
            }
        }

        gfx_SwapDraw();

        delay(100);
    }

    gfx_End();

    return 0;
}

