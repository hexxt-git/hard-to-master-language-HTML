#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINELEN 1024
#define true 1
#define false 0

struct INSTRUCTION{
    char*   instruction   ;
    char*   argument_str  ;
    int     argument_int  ;
};

int main(int argc, char** argv){
    if(argc != 2) {
        printf("please enter exactly one .html file name as argument\n");
        return 1;
    }

    FILE *fptr;
    fptr = fopen(argv[1], "r");
    if (fptr == NULL) {
        printf("Failed to open file.\n");
        return 1;
    }

    char** program_arr = malloc(sizeof(char*));
    char line[MAXLINELEN];
    int i = 0, program_size = 1;

    //we dont know how big the program will be so we scale up the array each time
    while (fgets(line, MAXLINELEN, fptr)) {
        if (line[0] == '\n') {
            continue; // dont even read if line is empty
        }
        if (i >= program_size) {
            program_size *= 2; // doubling each time apparently fastest way
            program_arr = realloc(program_arr, program_size * sizeof(char*));
        }
        program_arr[i] = strdup(line); // makes a unique copy
        i++;
    }
    program_size = i;
    fclose(fptr);

    
    struct INSTRUCTION* instruction_arr = malloc(sizeof(struct INSTRUCTION)*program_size);

    for(i = 0 ; i < program_size ; i++){
        char* instruction = malloc(sizeof(char) * 21);
        int instruction_index = 0;
        int set_instruction = false;
        char* argument_str = malloc(sizeof(char) * 101);
        int argument_index = 0;
        int argument_int = 0;
        for(int j = 0 ; j < strlen(program_arr[i]) ; j++){
            if(program_arr[i][j] == '\n') break; // ignore newlines (they are not part of the instruction or argument
            if(program_arr[i][j] == '#'){ // ignore comments
                break;                    // todo make this register the instruction too and add a way to escape comments
            }

            if(set_instruction == false){
                if(program_arr[i][j] == ' '){
                    if(instruction_index != 0) set_instruction = true; // instruction cant have white spaces
                    continue; // this also ignores indentation in the beginning
                    // this skips the white space between instruction and argument aswell
                }
                if(instruction_index >= 20){
                    printf("Instruction too long\n%s", program_arr[i]);
                    return 1;
                }
                instruction[instruction_index] = program_arr[i][j];
                instruction_index++;
                instruction[instruction_index] = '\0'; // i do be not knowing what this is
            } else {
                if(instruction_index >= 100){
                    printf("Argument too long\n%s", program_arr[i]);
                    return 1;
                }
                argument_str[argument_index] = program_arr[i][j];
                argument_index++;
                argument_str[argument_index] = '\0';
            }
        }
            // case where argument is empty
            if(argument_index == 0){
                argument_str = "null";
            }
            
            char *end;
            argument_int = strtol(argument_str, &end, 10); // the 10 is for decimal

            instruction_arr[i].instruction = instruction;
            instruction_arr[i].argument_str = argument_str;
            instruction_arr[i].argument_int = argument_int;            
    }

    printf("broken down program structure \n");
    for(int j = 0 ; j < program_size ; j++){
        printf("instruction: %s ", instruction_arr[j].instruction);
        printf("argument: %s ", instruction_arr[j].argument_str);
        printf("argument int: %d ", instruction_arr[j].argument_int);
        printf("\n");

    }

    // we're done with the program_arr so we free it
    for (int j = 0; j < program_size; j++) {
        free(program_arr[j]);
    }
    free(program_arr);






    free(instruction_arr);

    return 0;
}