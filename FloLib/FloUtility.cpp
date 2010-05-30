#include "FloUtility.h"

void get_line(FILE* f, char* string)
{
    do
    {
        fgets(string, 255, f);
    }
    while ((string[0] == '/') || (string[0] == ';') || (string[0] == '\n'));
}
