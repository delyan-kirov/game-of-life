#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libxtr.h"

typedef struct
{
    unsigned char state;
} Cell;

enum CellState
{
    Alive = 1,
    Dead = 0,
    /* CellDimension must be a power of 2, otherwise indexing doesn't work
       properly */
    CellDimension = 1 << 2,
    CellCount = CellDimension * CellDimension,
    CellNeighborCount = 8,
};

static Cell WorldBuffer[CellCount];
static Cell NextWorldBuffer[CellCount];

size_t alive_neighbours_count(size_t id)
{
    size_t count = 0;
    int row = id / CellDimension;
    int col = id % CellDimension;

    // [0] [1] [2]  |  [- -] [- 0] [- +]
    // [3] [4] [5]  |  [0 -] [0 0] [0 +]
    // [6] [7] [8]  |  [+ -] [+ 0] [+ +]

    static const size_t offset_row[CellNeighborCount] = {-1, -1, -1, 0, 1, 1, 1, 0};
    static const size_t offset_col[CellNeighborCount] = {-1, 0, 1, 1, 1, 0, -1, -1};

    for (size_t i = 0; i < CellNeighborCount; ++i)
    {
        size_t new_row_id = (row + offset_row[i]) % CellDimension;
        size_t new_col_id = (col + offset_col[i]) % CellDimension;

        size_t new_id = new_row_id * CellDimension + new_col_id;
        count += WorldBuffer[new_id].state;
    }

    return count;
}

void update_cell_state(size_t id)
{
    bool cell_alive = WorldBuffer[id].state;
    Cell *cell = &NextWorldBuffer[id];

    size_t alive_neighbors = alive_neighbours_count(id);

    if (!cell_alive && (3 == alive_neighbors)) // Reproduction
        cell->state = Alive;
    else if (!cell_alive) // Cell stays dead
        ;
    else
        switch (alive_neighbors)
        {
        case 0:
        case 1: // Under population
            cell->state = Dead;
            break;
        case 2:
        case 3: // Lives on
            cell->state = Alive;
            break;
        default: // Over population
            cell->state = Dead;
        }
}

void print_state(void)
{
    for (size_t i = 0; i < CellCount; ++i)
    {
        if (i % CellDimension == 0) printf("\n");
        printf("%d ", WorldBuffer[i].state);
    }
    printf("\n");
}

int main(void)
{
    // WorldBuffer[1 * CellDimension].state = Alive;
    // WorldBuffer[2 * CellDimension].state = Alive;
    // WorldBuffer[3 * CellDimension].state = Alive;

    // printf ("\033[2J"); // clear console

    // for (;;)
    // {
    //   printf ("\033[10;10H");
    //   print_state ();
    //   for (size_t i = 0; i < CellCount; ++i)
    //     update_cell_state (i);
    //   sleep (1);
    //   memcpy (WorldBuffer, NextWorldBuffer, sizeof (NextWorldBuffer));
    // }

    StrBld strBld = {0};
    StrBld_new(&strBld);
    StrBld_add_many(&strBld, "Hello", " ", "World", "!", "\n", '\0');

    String str = StrBld_fuse(&strBld);
    printf("%s", str.buf);
    StrBld_free(&strBld);
}
