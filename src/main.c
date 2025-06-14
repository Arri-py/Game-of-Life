#include <stdio.h>
#include <stdlib.h>

#define WIDTH 80
#define HEIGHT 25

char **create_world(int width, int height);  // создает мир
void init_world(char **world, int width, int height);  // кушает мир и заполняет его мертвыми клетками
void print_world(char **world, int width, int height);  // кушает мир и выводит его в консольку
void kill_world(char **world, int height);  // почему без width? да все просто зачем нам ширина когда мы можем
                                            // удалять по целым строкам типа free(номер строки)
void load_world(char **world);  // загрузка...


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

int main() {
    // создание мира :)
    char **world = create_world(WIDTH, HEIGHT);  // туда этот мир!!!
    if (world == NULL) {
        return 1;
    }

    // или пусто или из файла
    if (!feof(stdin)) {
        load_world(world);
    } else {
        init_world(world, WIDTH, HEIGHT);
    }

    // кормим миром init_world
    // init_world(world, WIDTH, HEIGHT);

    // тестовые данные
    // world[5][10] = '*';
    // world[6][11] = '*';
    // world[7][9] = '*';
    // world[7][10] = '*';
    // world[7][11] = '*';

    print_world(world, WIDTH, HEIGHT);
    kill_world(world, HEIGHT);

    return 0;
}