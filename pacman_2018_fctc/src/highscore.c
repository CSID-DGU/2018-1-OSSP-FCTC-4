#include "highscore.h"

int readScoreFromFile(){
    int score_from_file;
    FILE *file_pointer;

    file_pointer=fopen("resources/highscore.txt", "r");

    fscanf(file_pointer, "%d", &score_from_file);
    fclose(file_pointer);

    return score_from_file;
}

void writeScoreToFile(int highscore){
    FILE *file_pointer;

    file_pointer=fopen("resources/highscore.txt", "w");

    fprintf(file_pointer, "%d", highscore);
    fclose(file_pointer);
}