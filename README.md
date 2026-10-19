# Сборка

## CMake

```sh
cmake -S . -B build
cmake --build build --config Release
```

С выводом отладочной карты:

```sh
cmake -S . -B build/map -DDEBUG_MAP=ON
cmake --build build/map --config Debug
```

Запуск в Linux:

```sh
./build/Task coords_example.txt 10
```

Запуск в Windows:

```powershell
.\build\Release\Task.exe .\coords_example.txt 10
```

Запуск с отладочной картой в Linux:

```bash
./build/map/Task coords_example.txt 10
```

Запуск с отладочной картой в Windows:

```powershell
.\build\map\Debug\Task.exe .\coords_example.txt 10
```

## Сложность

Для каждой цели программа проверяет примерно `r^2` точек, поэтому сложность алгоритма — `O(n * r^2)`, где `n` — количество целей, `r` — радиус поражения. 
Поскольку поле имеет фиксированный размер 100×100, радиус ограничен его размерами и не превышает 99 клеток. Таким образом, `r` можно считать константой, а временная сложность алгоритма составляет `O(n)`.