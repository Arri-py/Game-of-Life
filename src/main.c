// clear && clang-format -style=file:../materials/linters/.clang-format -i main.c && gcc -Wall -Werror -Wextra
// main.c -lncurses -o out && ./out < file1.txt && rm out
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WIDTH 80
#define HEIGHT 25
#define MIN_SPEED 1
#define MAX_SPEED 10
#define DEFAULT_SPEED 3

char **create_world(int width, int height);  // создает мир
void init_world(char **world, int width, int height);  // кушает мир и заполняет его мертвыми клетками
void print_world(char **world, int width, int height);  // кушает мир и выводит его в консольку
void kill_world(char **world, int height);  // почему без width? да все просто зачем нам ширина когда мы можем
                                            // удалять по целым строкам типа free(номер строки)
void load_world(char **world);  // загрузка...
void game_loop(char **world, char **next_world, int *speed, int *running);
void update_world(char **current, char **next);
int count_neighbors(char **world, int x, int y);
void print_world_ncurses(char **world, int width, int height, int speed);

char **create_world(int width, int height) {
    char **world = malloc(height * sizeof(char *));

    if (world == NULL) {
        printf("n/a create_world");
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        world[i] = malloc(width * sizeof(char *));
        if (world[i] == NULL) {
            printf("n/a create_world");

            // как бы уже выделили память если будет ошибочка то очищаем выделенную память)
            for (int j = 0; j < 1; j++) {
                free(world[j]);
            }
            free(world);
            return NULL;
        }
    }
    return world;
}

// крч кушает мир и заполняет мертвыми клетками
void init_world(char **world, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            world[i][j] = ' ';
        }
    }
}

// кушает мир и выводит его в консольку
void print_world(char **world, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%c", world[i][j]);
        }
        printf("\n");
    }
}

void kill_world(char **world, int height) {
    for (int i = 0; i < height; i++) {
        free(world[i]);
    }
    free(world);  // важный моментик мы удалили строчки с матрице но не саму матрицу
}

void load_world(char **world) {
    // чтение только 80 на 25 символов
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int c = getchar();
            world[y][x] = (c == '1') ? '*' : ' ';
        }
        // Пропускаем символ новой строки
        int newline = getchar();
        (void)newline;
    }
}

void game_loop(char **current, char **next, int *speed, int *running) {
    if (!*running) return;

    // Обработка ввода
    int ch = getch();
    switch (ch) {
        case 'a':
        case 'A':
            if (*speed < MAX_SPEED) (*speed)++;
            break;
        case 'z':
        case 'Z':
            if (*speed > MIN_SPEED) (*speed)--;
            break;
        case ' ':
            *running = 0;
            return;
    }

    // Обновление и отрисовка
    update_world(current, next);
    print_world_ncurses(current, WIDTH, HEIGHT, *speed);  // Передаем текущее значение скорости

    // Задержка и рекурсивный вызов
    usleep(1000000 / *speed);
    game_loop(current, next, speed, running);
}

void update_world(char **current, char **next) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int neighbors = count_neighbors(current, x, y);

            if (current[y][x] == '*' && (neighbors == 2 || neighbors == 3)) {
                next[y][x] = '*';
            } else if (current[y][x] == ' ' && neighbors == 3) {
                next[y][x] = '*';
            } else {
                next[y][x] = ' ';
            }
        }
    }

    // Копирование next в current
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            current[y][x] = next[y][x];
        }
    }
}

int count_neighbors(char **world, int x, int y) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;

            int nx = (x + dx + WIDTH) % WIDTH;
            int ny = (y + dy + HEIGHT) % HEIGHT;

            if (world[ny][nx] == '*') count++;
        }
    }
    return count;
}

void print_world_ncurses(char **world, int width, int height, int speed) {
    clear();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            mvaddch(y, x, world[y][x]);
        }
    }
    mvprintw(height, 0, "Speed: %d (A/Z - speed, SPACE - exit)", speed);
    refresh();
}

int main() {
    // создание мира :)
    char **world = create_world(WIDTH, HEIGHT);       // туда этот мир!!!
    char **next_world = create_world(WIDTH, HEIGHT);  // некст мир
    int speed = DEFAULT_SPEED;
    int running = 1;

    if (!world || !next_world) {
        if (world) kill_world(world, HEIGHT);
        if (next_world) kill_world(next_world, HEIGHT);
        return 1;
    }

    // if (world == NULL) {
    //     return 1;
    // }

    // или пусто или из файла
    if (!feof(stdin)) {
        load_world(world);
    } else {
        init_world(world, WIDTH, HEIGHT);
    }
    init_world(next_world, WIDTH, HEIGHT);  //

    // крутим ncurses
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);

    // по ссути сама игра
    game_loop(world, next_world, &speed, &running);

    // конец...
    endwin();
    kill_world(world, HEIGHT);
    kill_world(next_world, HEIGHT);

    // кормим миром init_world
    // init_world(world, WIDTH, HEIGHT);

    // тестовые данные
    // world[5][10] = '*';
    // world[6][11] = '*';
    // world[7][9] = '*';
    // world[7][10] = '*';
    // world[7][11] = '*';

    // print_world(world, WIDTH, HEIGHT);
    // kill_world(world, HEIGHT);

    return 0;
}