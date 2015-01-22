#include "logics.hpp"

void nextStep(field & f){
    // массив для подсчёта соседей
    auto tmp = new char*[f.height];
    for (unsigned int i = 0; i < f.height; ++i)
        tmp[i] = new char[f.width];

    // 1. Обрабатываем первую строку
    //    в ней все соседи друг другу, так как есть общая вершина (полюс)
    //    поэтому проще всего посчитать сумму в этой строке
    unsigned int s = 0;
    for (unsigned int i = 0; i < f.width; ++i)
        s += f[0][i];

    for (unsigned int i = 0; i < f.width; ++i)
        if ( f[0][i] ){
            tmp[0][i] = s - 1;
            tmp[1][( i + f.width - 1 ) % f.width] += 1;
            tmp[1][i] += 1;
            tmp[1][( i + f.width + 1 ) % f.width] += 1;
        } else {
            tmp[0][i] = s;
        }

    // 2. Между первой и последней
    for (unsigned int j = 1; j < f.height - 1; ++j)
        for (unsigned int i = 0; i < f.width; ++i)
            if ( f[j][i] ){
                tmp[j - 1][( i + f.width - 1 ) % f.width] += 1;
                tmp[j - 1][i]                             += 1;
                tmp[j - 1][( i + f.width + 1 ) % f.width] += 1;

                tmp[j][( i + f.width - 1 ) % f.width] += 1;
                tmp[j][i]                             += 1;
                tmp[j][( i + f.width + 1 ) % f.width] += 1;

                tmp[j + 1][( i + f.width - 1 ) % f.width] += 1;
                tmp[j + 1][i]                             += 1;
                tmp[j + 1][( i + f.width + 1 ) % f.width] += 1;
            }

    // 3. Последняя аналогично первой
    unsigned int p = f.height - 1;
    s = 0;
    for (unsigned int i = 0; i < f.width; ++i)
        s += f[p][i];

    for (unsigned int i = 0; i < f.width; ++i)
        if ( f[0][i] ){
            tmp[0][i] += s - 1;

            tmp[p - 1][( i + f.width - 1 ) % f.width] += 1;
            tmp[p - 1][i] += 1;
            tmp[p - 1][( i + f.width + 1 ) % f.width] += 1;
        } else {
            tmp[p][i] += s;
        }

    // 4. Установка нового состояния
    for (unsigned int j = 0; j < f.height; ++j)
        for (unsigned int i = 0; i < f.width; ++i)
            f[j][i] = (tmp[j][i] == 2 && f[j][i]) || (tmp[j][i] == 3);
    delete[] tmp;
}
