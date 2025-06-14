#include <stdio.h>
#include <unistd.h>

#define ALIVE 1
#define DEAD 0
#define HEIGHT 25
#define WIDTH 80

void matrix_init(int (*matrix)[WIDTH][HEIGHT], int, int);
int input(int (*matrix)[WIDTH][HEIGHT], int x,
          int y);  // принимает указатель на матрицу и указатели на переменные для размера по икс и игрек
int print_matrix(int matrix[WIDTH][HEIGHT]);
void prepare_map(int (*matrix)[WIDTH][HEIGHT]);

void matrix_copy(int (*mat1)[WIDTH][HEIGHT], int (*mat2)[WIDTH][HEIGHT], int, int);
int matrix_part_copy(int (*mat)[WIDTH][HEIGHT], int (*part)[3][3], int, int);
// int isLive(int (*mx)[3][3]);
int isLive(int *mx);

int main() {
    const int delay_ms = 100000;
    int matrix[WIDTH][HEIGHT];

    matrix_init(&matrix, WIDTH, HEIGHT);
    input(&matrix, WIDTH, HEIGHT);
    for (;;) {
        print_matrix(matrix);
        prepare_map(&matrix);
        usleep(delay_ms);
    }
    return 0;
}

void matrix_init(int (*matrix)[WIDTH][HEIGHT], int x, int y) {
    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < y; ++j) {
            (*matrix)[i][j] = 0;
        }
    }
}

int input(int (*matrix)[WIDTH][HEIGHT], int x, int y) {
    int nonstop = 1, xx = 0, yy = 1;
    while (nonstop) {
        if (xx > x || yy > y) {
            break;
        }
        char inputc = getchar();
        switch (inputc) {
            case '1':
                ++xx;
                (*matrix)[xx - 1][(yy)-1] = 1;
                break;
            case '0':
                ++xx;
                (*matrix)[xx - 1][(yy)-1] = 0;
                break;
            case '\n':
                ++yy;
                xx = 0;
                break;
            default:
                nonstop = 0;
                break;
        }
    }
    return 0;
}

int print_matrix(int matrix[WIDTH][HEIGHT]) {
    printf("\033[H");
    if (matrix == NULL) {
        printf("n/a\n");
        return 1;
    }
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (matrix[x][y] == 1) {
                printf("o");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
    return 0;
}

void prepare_map(int (*matrix)[WIDTH][HEIGHT]) {
    int old[WIDTH][HEIGHT];
    matrix_copy(&old, matrix, WIDTH, HEIGHT);
    for (int j = 0; j < HEIGHT; ++j) {
        for (int i = 0; i < WIDTH; ++i) {
            int part[3][3];
            matrix_part_copy(&old, &part, i, j);
            (*matrix)[(i + 1) % WIDTH][(j + 1) % HEIGHT] = isLive(*part);
        }
    }
}

void matrix_copy(int (*mat1)[WIDTH][HEIGHT], int (*mat2)[WIDTH][HEIGHT], int x, int y) {
    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < y; ++j) {
            (*mat1)[i][j] = (*mat2)[i][j];
        }
    }
}

int matrix_part_copy(int (*mat)[WIDTH][HEIGHT], int (*part)[3][3], int x, int y) {
    for (int j = 0; j < 3; ++j) {
        for (int i = 0; i < 3; ++i) {
            (*part)[i][j] = (*mat)[(x + i) % WIDTH][(y + j) % HEIGHT];
        }
    }

    return 0;
}

int isLive(int *mx) {
    // Принимает указатель на матрицу размером 3х3, где в центре проверяемая клетка
    // Выдает 1 - если на след ход клетка будет жить (рисуется). 0 - если не рисуется
    int size = 9;
    int isLive = mx[4];               // середина матрицы 3х3
    int neighbors_qty = 0;            // кол-во живых соседей
    for (; 0 < size; --size, ++mx) {  // считает кол-во живых клеток
        neighbors_qty += *mx;
    }
    neighbors_qty -= isLive;

    if (isLive) {
        if (neighbors_qty < 2 || 3 < neighbors_qty) isLive = DEAD;
    } else {
        if (3 == neighbors_qty) isLive = ALIVE;
    }

    return isLive;
}

