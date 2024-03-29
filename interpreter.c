#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINELEN 1024
#define STACKSIZE 1024
#define MAXJUMPDEPTH 1024
#define VERBOSE 0
#define true 1
#define false 0

struct INSTRUCTION{
    char*   instruction   ;
    char*   argument_str  ;
    int     argument_int  ;
};
struct Label{
    char*   label;
    int     inst_ptr;
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
    struct Label* label_arr = malloc(sizeof(char*)*program_size);
    int label_arr_index = 0;

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
                if (program_arr[i][j] == '\\' && program_arr[i][j+1] == 'n'){
                    argument_str[argument_index] = '\n';
                    argument_index++;
                    j++;
                    argument_str[argument_index] = '\0';
                    continue;
                }
                argument_str[argument_index] = program_arr[i][j];
                argument_index++;
                argument_str[argument_index] = '\0';
            }
        }
        // replacing new lines with pss instruction
        if(instruction_index == 0){
            instruction = "pss";
        }
        // case where argument is empty
        if(argument_index == 0){
            argument_str = "null";
        }
        // parsing incase argument needs to be int            
        char *end;
        argument_int = strtol(argument_str, &end, 10); // the 10 is for decimal


        if(instruction[strlen(instruction)-1] == ':'){
            for(int j = 0 ; j < label_arr_index ; j++){
                if(strcmp(label_arr[j].label, instruction) == 0){
                    printf("label %s already exists\n", instruction);
                    return 1;
                }
            }
            char* label_name = malloc(sizeof(char) * 21);
            for(int j = 0 ; j < strlen(instruction)-1 ; j++){
                label_name[j] = instruction[j];
            }
            label_name[strlen(instruction)-1] = '\0';
            label_arr[label_arr_index].label = label_name;
            label_arr[label_arr_index].inst_ptr = i;
            label_arr_index++;
            instruction = "pss";
            if(VERBOSE == 1) printf("Found label %s at %d\n", label_name, i);
        }

        instruction_arr[i].instruction = instruction;
        instruction_arr[i].argument_str = argument_str;
        instruction_arr[i].argument_int = argument_int;
    }
    if(VERBOSE == 1){
        printf("broken down program structure \n");
        for(int j = 0 ; j < program_size ; j++){
            printf("instruction: %s ", instruction_arr[j].instruction);
            printf("argument: %s ", instruction_arr[j].argument_str);
            printf("argument int: %d ", instruction_arr[j].argument_int);
            printf("\n");
        }
        printf("execution:\n");
    }

    // we're done with the program_arr so we free it
    for (int j = 0; j < program_size; j++) {
        free(program_arr[j]);
    }
    free(program_arr);

    int* stack = malloc(sizeof(int) * STACKSIZE);
    int stack_ptr = -1;
    int inst_ptr  = 0;
    int* jump_history = malloc(sizeof(int) * MAXJUMPDEPTH);
    int jump_history_index = -1;

    for(int j = 0 ; j < label_arr_index ; j++){
        if(strcmp(label_arr[j].label, "main") == 0){
            jump_history_index++;
            jump_history[jump_history_index] = j;
            inst_ptr = label_arr[j].inst_ptr;
            break;
        }
    }
    if(jump_history_index == -1){
        printf("no main label found\n");
        return 1;
    }

    while(jump_history_index >= 0 && inst_ptr < program_size){
        
        if(VERBOSE == 1){
            printf("safely arrived at instruction %d %s %s\n", inst_ptr, instruction_arr[inst_ptr].instruction, instruction_arr[inst_ptr].argument_str);
        }

        if(strcmp(instruction_arr[inst_ptr].instruction, "psh") == 0){
            stack_ptr++;
            stack[stack_ptr] = instruction_arr[inst_ptr].argument_int;
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "pop") == 0){
            stack_ptr--;
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "add") == 0){
            int a = stack[stack_ptr];
            stack_ptr--;
            int b = stack[stack_ptr];
            stack_ptr--;

            stack_ptr++;
            stack[stack_ptr] = a + b;
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "sub") == 0){
            int a = stack[stack_ptr];
            stack_ptr--;
            int b = stack[stack_ptr];
            stack_ptr--;

            stack_ptr++;
            stack[stack_ptr] = a - b;
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "mul") == 0){
            int a = stack[stack_ptr];
            stack_ptr--;
            int b = stack[stack_ptr];
            stack_ptr--;

            stack_ptr++;
            stack[stack_ptr] = a * b;
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "div") == 0){
            int a = stack[stack_ptr];
            stack_ptr--;
            int b = stack[stack_ptr];
            stack_ptr--;

            stack_ptr++;
            stack[stack_ptr] = a % b;
            stack_ptr++;
            stack[stack_ptr] = a / b;
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "ife") == 0){
            int a = stack[stack_ptr];
            stack_ptr--;
            int b = stack[stack_ptr];
            stack_ptr--;
            if(a == b){
                inst_ptr++;
            }
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "ifn") == 0){
            int a = stack[stack_ptr];
            stack_ptr--;
            int b = stack[stack_ptr];
            stack_ptr--;
            if(a != b){
                inst_ptr++;
            }
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "ifg") == 0){
            int a = stack[stack_ptr];
            stack_ptr--;
            int b = stack[stack_ptr];
            stack_ptr--;
            if(a > b){
                inst_ptr++;
            }
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "ifl") == 0){
            int a = stack[stack_ptr];
            stack_ptr--;
            int b = stack[stack_ptr];
            stack_ptr--;
            if(a < b){
                inst_ptr++;
            }
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "red") == 0){
            int input;
            scanf("%d", &input);
            stack_ptr++;
            stack[stack_ptr] = input;
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "prt") == 0){
            printf("%d", stack[stack_ptr]);
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "wrt") == 0){
            printf("%c", stack[stack_ptr]);
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "log") == 0){
            printf("%s", instruction_arr[inst_ptr].argument_str);
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "jmp") == 0){
            inst_ptr = instruction_arr[inst_ptr].argument_int;
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "jml") == 0){
            for(int j = 0 ; j < label_arr_index ; j++){
                if(strcmp(label_arr[j].label, instruction_arr[inst_ptr].argument_str) == 0){
                    jump_history_index++;
                    jump_history[jump_history_index] = inst_ptr + 1;
                    inst_ptr = label_arr[j].inst_ptr;
                    break;
                }
            }
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "pss") == 0){
            // do nothing
        } else if (strcmp(instruction_arr[inst_ptr].instruction, "hlt") == 0){
            inst_ptr = jump_history[jump_history_index] - 1;
            jump_history_index--;
        } else {
            printf("unknown instruction %s\n", instruction_arr[inst_ptr].instruction);
            return 1;
        }

        if(VERBOSE == 1){
            printf("stack state after \"%s\": [", instruction_arr[inst_ptr].instruction);
            if(stack_ptr == -1){
                printf("empty");
            } else {
                for(int j = 0 ; j <= stack_ptr-1 ; j++){
                    printf("%d, ", stack[j]);
                }
                printf("%d", stack[stack_ptr]);
            }
            printf("]\n");
            printf("jump history: [");
            for(int j = 0 ; j <= jump_history_index ; j++){
                printf("%d, ", jump_history[j]);
            }
            printf("]\n");
        }
        inst_ptr++;
    }


    free(instruction_arr);

    return 0;
}