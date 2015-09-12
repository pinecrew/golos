# Game Of Life On Sphere

[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/pinecrew/golos?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Визуализация игры в жизнь на сфере во имя веселья, C++, SDL2, OpenGL и многого другого.

## Этапы разработки

### Январь 2015

Реализация на SDL2 без использования OpenGL.

Возможности:

 * Оживление и убийство клеток по клику на них
 * Изменение скорости смены поколений
 * Вращение сферы

Особенности:

 * Велосипед: преобразования координат
 * Велосипед: поклеточное освещение

### Август 2015

Реализация с использованием OpenGL.

Возможности:

 * Всё те же

Особенности

 * Текстурирование и освещение на шейдерах (версия 1.20)
 * Луна с normal mapping

## Хотелось бы сделать

 * Шейдеры версии 1.50 или 3.30
 * Тени

## Компиляция проекта
### Windows
Необходимо установить MSYS2 по следующей [инструкции](https://msys2.github.io/) и следующие пакеты

Для 32-битной системы:
```
$ pacman -S make mingw-w64-i686-gcc mingw-w64-i686-SDL2 mingw-w64-i686-SDL2_image
            mingw-w64-i686-SDL2_ttf mingw-w64-i686-glew mingw-w64-i686-glm
```

Для 64-битной системы:
```
$ pacman -S make mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image
            mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-glew mingw-w64-x86_64-glm
```

### Linux
Обратитесь к документации вашего дистрибутива для подробной информации.

Необходимые компоненты:
 * SDL2
 * SDL2_image, SDL2_ttf
  * zlib
  * libpng
  * FreeType2
 * GLM
 * GLEW

### Компиляция
```
$ make RELEASE=yes
```