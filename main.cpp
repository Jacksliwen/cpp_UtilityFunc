#include <iostream>
#include <unistd.h>
#include "func.h"

int main(int argc, char** argv){

    while(getchar() != 'q'){
        usleep(500000);
    }
    return 0;
}