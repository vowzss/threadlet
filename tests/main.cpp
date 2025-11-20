#include <cstdio>

int main()
{
    float goon[1000];

    for (int i = 0; i < sizeof(goon) / sizeof(goon[0]); i++)
        printf("%d\n", i);

    return 0;
}
