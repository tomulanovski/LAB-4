#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int digCount(char *string) {
    int count = 0;
    for (int i =0;i<strlen(string);i++) {
        if (string[i]>='0' && string[i]<='9'){
            count +=1;
        }
    }
    printf("%d\n",count);
    return count;
}

int main(int argc, char **argv)
{
    digCount(argv[1]);
    return 0;
}

