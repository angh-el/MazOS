#include "calculator.hpp"

int calculator(){

    Keyboard::setCurrentMode(Mode::MODE_CALCULATOR);

    printf("Welcome to the Keyboard Calculator!\n");
    printf("Enter a basic arithmetic expression (e.g., 2+3*4):\n");
    printf("> ");


    return 1;
}



void apppend_to_buffer(char c) {

    if(c == 'q'){
        Keyboard::setCurrentMode(Mode::MODE_CLI);
        printf("Quitting Calculator ...\n");
        return;
    }

    if(c == '\n'){
        int result = evaluate_expression(input);
        printf(" = %d", result);
        printf("\n> ");
        return;
    }

    if (input_index < sizeof(input) - 1) { 
        input[input_index++] = c;
        input[input_index] = '\0'; 
        printf("%c ", c);
    } 
    else {
        printf("Input buffer full\n");
    }
    
}



int evaluate_expression(const char *expr) {
    int result = 0;
    int current_number = 0;
    char current_operator = '+';

    for (const char *p = expr; *p != '\0'; p++) {
        char c = *p;

        if (c >= '0' && c <= '9') {
            current_number = current_number * 10 + (c - '0');
        } else if (c == '+' || c == '-' || c == '*' || c == '/') {
            switch (current_operator) {
                case '+':
                    result += current_number;
                    break;
                case '-':
                    result -= current_number;
                    break;
                case '*':
                    result *= current_number;
                    break;
                case '/':
                    if (current_number != 0) {
                        result /= current_number;
                    } else {
                        printf("Error: Cannot Divide by zero!\n");
                        return 0;
                    }
                    break;
            }

            current_operator = c;
            current_number = 0;
        }
    }

    switch (current_operator) {
        case '+':
            result += current_number;
            break;
        case '-':
            result -= current_number;
            break;
        case '*':
            result *= current_number;
            break;
        case '/':
            if (current_number != 0) {
                result /= current_number;
            } else {
                printf("Error: Cannot Divide by zero!\n");
                return 0;
            }
            break;
    }

    input_index = 0;
    input[0] = '\0';
    return result;
}