#include <stdio.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
int main()
{
    char x[10];
    scanf("%s",x);
    printf("%s",x);
    return 0;
}
#pragma clang diagnostic pop