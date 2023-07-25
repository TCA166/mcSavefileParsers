#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "../model.h"

//Benchmark for reallocBy multiplier size. Haven't found much difference in benchmarks

#define size 15
#define iterations 50

float test(model* input){
    float startTime = (float)clock()/CLOCKS_PER_SEC;

    free(generateModel(input, NULL, NULL, NULL));

    float endTime = (float)clock()/CLOCKS_PER_SEC;

    return endTime - startTime;
}

int main(){
    struct cubeModel cModel = initCubeModel(size, size, size);
    dimensionalFor(size, size, size){
        cModel.cubes[x][y][z] = malloc(sizeof(struct cube));
        *(cModel.cubes[x][y][z]) = createGenericCube(2);
        cModel.cubes[x][y][z]->x = x;
        cModel.cubes[x][y][z]->y = y;
        cModel.cubes[x][y][z]->z = z;
    }
    model nModel = cubeModelToModel(&cModel, NULL);
    float res = 0;
    for(int i = 0; i < iterations; i++){
        res += test(&nModel);
        res /= i+1;
    }
    printf("\n%.2f\n", res);
    return 0;
}