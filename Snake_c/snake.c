#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#define WIDTH 70
#define HEIGHT 20
#define UNIT_SIZE 1
#define GAME_UNITS ((WIDTH * HEIGHT) / (UNIT_SIZE * UNIT_SIZE))
#define DELAY 50 // Reduced delay for smoother game play
#define FRAME_RATE 10
#define FRAME_INTERVAL (1000 / FRAME_RATE)

typedef struct
{
    int x, y;//x and y axis
} Point;

typedef struct Snake
{
    Point *body;
    int length;
    char direction;
} Snake;

Point food;
int score;
int running;

CHAR_INFO screenBuffer[WIDTH * HEIGHT]; // Double buffering

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void init()
{
    system("cls");
    CONSOLE_CURSOR_INFO cursor;
    cursor.dwSize = 100;
    cursor.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
}

void setup(Snake *snake)
{
    snake->length = 1;
    snake->body = (Point *)malloc(GAME_UNITS * sizeof(Point));
    snake->body[0].x = WIDTH / 2;
    snake->body[0].y = HEIGHT / 2;
    snake->direction = 'R';

    food.x = rand() % (WIDTH / UNIT_SIZE) * UNIT_SIZE;
    food.y = rand() % (HEIGHT / UNIT_SIZE) * UNIT_SIZE;

    score = 0;
    running = 1;
}




void draw(Snake *snake) {
    // Create a CHAR_INFO array to represent the entire screen buffer
    CHAR_INFO screenBuffer[WIDTH * HEIGHT];

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        screenBuffer[i].Char.AsciiChar = ' '; // Initialize all characters as empty space
        screenBuffer[i].Attributes = 0; // Initialize all attributes as 0 (default console color)
    }

    // Draw the game boundaries
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                screenBuffer[i * WIDTH + j].Char.AsciiChar = '|';
            }
        }
    }

    // Draw the snake
    for (int i = 0; i < snake->length; i++) {
        screenBuffer[snake->body[i].y * WIDTH + snake->body[i].x].Char.AsciiChar = '*';
        screenBuffer[snake->body[i].y * WIDTH + snake->body[i].x].Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    }

    // Set the color attribute for the food
    screenBuffer[food.y * WIDTH + food.x].Attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;

    // Draw the food character ('$')
    WCHAR foodChar = L'$'; // Use wide character for the food
    screenBuffer[food.y * WIDTH + food.x].Char.UnicodeChar = foodChar;

    // Copy the screen buffer to the console
    COORD bufferSize = { WIDTH, HEIGHT };
    COORD bufferCoord = { 0, 0 };
    SMALL_RECT writeRegion = { 0, 0, WIDTH - 1, HEIGHT - 1 };
    WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), screenBuffer, bufferSize, bufferCoord, &writeRegion);

    // Print the score
    gotoxy(0, HEIGHT);
    printf("Score: %d", score);
}


void input(Snake *snake)
{
    if (_kbhit())
    {
        int ch = _getch();
        switch (ch)
        {
            case 72: // Up arrow key
                if (snake->direction != 'D')
                    snake->direction = 'U';
                break;
            case 80: // Down arrow key
                if (snake->direction != 'U')
                    snake->direction = 'D';
                break;
            case 75: // Left arrow key
                if (snake->direction != 'R')
                    snake->direction = 'L';
                break;
            case 77: // Right arrow key
                if (snake->direction != 'L')
                    snake->direction = 'R';
                break;
            case 'x':
                running = 0;
                break;
        }
    }
}

void moveSnake(Snake *snake) {
    Point tail = snake->body[snake->length - 1];
    Point head = snake->body[0];
    Point newHead;

    switch (snake->direction) {
        case 'U':
            newHead.x = head.x;
            newHead.y = head.y - UNIT_SIZE;
            break;
        case 'D':
            newHead.x = head.x;
            newHead.y = head.y + UNIT_SIZE;
            break;
        case 'L':
            newHead.x = head.x - UNIT_SIZE;
            newHead.y = head.y;
            break;
        case 'R':
            newHead.x = head.x + UNIT_SIZE;
            newHead.y = head.y;
            break;
    }

    // Check if the snake collides with itself
    for (int i = 1; i < snake->length; i++) {
        if (newHead.x == snake->body[i].x && newHead.y == snake->body[i].y) {
            running = 0;
            break;
        }
    }

    // Check if the snake collides with the wall (updated condition)
    if (newHead.x >= WIDTH - UNIT_SIZE || newHead.x < UNIT_SIZE || newHead.y >= HEIGHT - UNIT_SIZE || newHead.y < UNIT_SIZE) {
        running = 0;
    }

    // Check if the snake eats the food
    if (newHead.x == food.x && newHead.y == food.y) {
        // Increase the snake's length and generate new food
        score += 1;
        snake->length++;
        snake->body = (Point *)realloc(snake->body, snake->length * sizeof(Point));
        snake->body[snake->length - 1] = tail; // Add a new tail segment
        food.x = (rand() % ((WIDTH - 2 * UNIT_SIZE) / UNIT_SIZE)) * UNIT_SIZE + UNIT_SIZE; // Generate random x-coordinate within bounds
        food.y = (rand() % ((HEIGHT - 2 * UNIT_SIZE) / UNIT_SIZE)) * UNIT_SIZE + UNIT_SIZE; // Generate random y-coordinate within bounds
    } else {
        // Move the snake as usual
        for (int i = snake->length - 1; i > 0; i--) {
            snake->body[i] = snake->body[i - 1];
        }
    }

    snake->body[0] = newHead;
}

int main() {
    srand(time(NULL));
    Snake snake;
    init();
    setup(&snake);

    DWORD lastUpdateTime = GetTickCount();
    DWORD deltaTime = 0;

    while (running) {
        DWORD currentTime = GetTickCount();
        deltaTime += currentTime - lastUpdateTime;
        lastUpdateTime = currentTime;

        input(&snake);

        while (deltaTime >= FRAME_INTERVAL) {
            moveSnake(&snake);
            deltaTime -= FRAME_INTERVAL;
        }

        draw(&snake);

        // Sleep to control frame rate (optional)
        //Sleep(10); // Adjust this value for smoother or faster gameplay
    }

    gotoxy(WIDTH / 2 - 10, HEIGHT / 2);
    printf("Game Over");
    gotoxy(WIDTH / 2 - 10, HEIGHT / 2 + 1);
    printf("Your Score: %d", score);
    Sleep(2000); // Sleep for 2 seconds
    free(snake.body);
    return 0;
}
