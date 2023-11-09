#include <stdio.h>
#include <string.h>

#define MAX_STRING_SIZE 128
char buffer[MAX_STRING_SIZE];

void input()
{
    printf("Enter Text: ");
    memset(buffer, 0, MAX_STRING_SIZE);
    fgets(buffer, MAX_STRING_SIZE, stdin);
}

void output()
{
    printf("Entered Text: %s\n", buffer);
}

int main(int argc, char **argv)
{
    printf("Communication test(exit with CTRL + c)\n");
    while (1)
    {
        input();
        output();
    }
    return 0;
}