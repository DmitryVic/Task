#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef DEBUG_MAP
#define DEBUG_MAP 0
#endif

#define WIDTH 100
#define HEIGHT 100

int16_t map[WIDTH][HEIGHT];
int16_t radius_blast; // Радиус попадания | от `MIN_RADIUS_BLAST` (при 0 учитывается
                      // только точка попадания) до `MAX_RADIUS_BLAST`
#define MAX_RADIUS_BLAST 99
#define MIN_RADIUS_BLAST 0

typedef struct {
  int16_t x;
  int16_t y;
} Point_t;

typedef struct {
  Point_t point;
  int16_t count_in_radius;
} Best_point_t;

static Best_point_t best_point = {{0, 0}, 0};

/*!
 * @brief Обход зоны попадания | `++1` каждой клетке в радиусе r | обновление
 * `best_point`, если клетка имеет большее значение
 *
 * @param cx Центр цели по оси Х
 * @param cy Центр цела по оси У
 * @param r Радиус попадания
 */
void circle_hit(int16_t cx, int16_t cy, int16_t r) {

  if (cx < 0 || cx >= WIDTH || cy < 0 || cy >= HEIGHT || r < MIN_RADIUS_BLAST || r > MAX_RADIUS_BLAST)
    return;

  const int16_t r2 = r * r;

  // Обход квадрата R на R с заполнением ++1 в клетках попадания
  for (int16_t x = cx - r; x <= cx + r; ++x) {
    for (int16_t y = cy - r; y <= cy + r; ++y) {
      if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        continue;

      const int16_t dx = x - cx; // смещение текущей точки относительно центра по X
      const int16_t dy = y - cy; // смещение текущей точки относительно центра по Y

      if (dx * dx + dy * dy <= r2) { // Координата входит в окружность
        map[x][y] += 1;

        if (map[x][y] > best_point.count_in_radius) // Более весомая цель
        {
          best_point.count_in_radius = map[x][y];
          best_point.point.x = x;
          best_point.point.y = y;
        }
      }
    }
  }
}

int main(int argc, char **argv) {

#ifdef _WIN32
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);
#endif

  if (argc != 3) {
    fprintf(stderr, "Не верно введены начальные уловия: количество аргументов "
                    "должно равняться двум.\n");
    return 1;
  }

  char *end;
  errno = 0;
  long radius = strtol(argv[2], &end, 10);
  
  if (errno == ERANGE ||
    end == argv[2] ||
    *end != '\0' ||
    radius < MIN_RADIUS_BLAST ||
    radius > MAX_RADIUS_BLAST) {
    fprintf(stderr,
            "Неверно введены начальные условия: радиус попадания не в "
            "диапазоне %d <= R <= %d\n",
            MIN_RADIUS_BLAST, MAX_RADIUS_BLAST);
    return 1;
  }
  radius_blast = (int16_t)radius;

  FILE *file_coords = fopen(argv[1], "r");
  if (file_coords == NULL) {
    perror(argv[1]);
    return 1;
  }

  char line_text[64];   // Строка из файла
  int target_count = 0; // Количество учтенных целей
  int line_count = 0;   // Обрабатываемая строка в файле
  while (fgets(line_text, sizeof line_text, file_coords) != NULL) {
    int x, y;
    char extra;
    ++line_count;
    if (sscanf(line_text, "%d , %d %c", &x, &y, &extra) != 2) {
      printf("Пропущена строка в файле %d: не удалось спарсить\n", line_count);
      continue;
    }

    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
      printf("Пропущена строка в файле %d: не верные данные\n", line_count);
      continue;
    }

    circle_hit((int16_t)x, (int16_t)y, (int16_t)radius_blast);
    ++target_count;
  }


  if (target_count == 0) {
    printf("Целей на поле нет\n");
  } else {
    printf("Координаты оптимальной точки: x: %d y: %d\n", best_point.point.x,
           best_point.point.y);
    printf("Количество пораженных целей: %d \n", best_point.count_in_radius);
  }


  // Отобразить карту
#if DEBUG_MAP
  rewind(file_coords);
  while (fgets(line_text, sizeof line_text, file_coords) != NULL) {
      int x, y;
      if (sscanf(line_text, "%d, %d", &x, &y) != 2) 
        continue;

      if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        continue;

      map[x][y] = -1000; 
    }


  printf("Строк в файле: %d \n", line_count);
  for (size_t print_x = 0; print_x < HEIGHT; print_x++) {
    for (size_t print_y = 0; print_y < HEIGHT; print_y++) {
      if (best_point.point.x == print_x && best_point.point.y == print_y)
        printf("\x1b[31m[X]\x1b[0m");
      else if (map[print_x][print_y] == -1000)
        printf("\x1b[33m[#]\x1b[0m");
      else if (map[print_x][print_y] == 0)
        printf("[%d]", map[print_x][print_y]);
      else
        printf("\x1b[34m[%d]\x1b[0m", map[print_x][print_y]);
    }
    printf("\n");
  }
#endif

  fclose(file_coords);
  
}