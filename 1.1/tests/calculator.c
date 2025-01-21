#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_INPUT_LENGTH 100

// Function prototypes
void read_input(char *buffer, int max_length);
int evaluate_expression(const char *expr);

int main() {
    char input[MAX_INPUT_LENGTH];

    printf("Welcome to the Keyboard Calculator!\n");
    printf("Enter a basic arithmetic expression (e.g., 2+3*4):\n");

    while (1) {
        printf("> ");
        read_input(input, MAX_INPUT_LENGTH);

        // If the user types "exit", break the loop
        if (strcmp(input, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        }

        // Evaluate the expression
        int result = evaluate_expression(input);
        printf("Result: %d\n", result);
    }

    return 0;
}

// Function to read user input from the keyboard
void read_input(char *buffer, int max_length) {
    int index = 0;
    char c;

    while (index < max_length - 1) {
        c = getchar();

        if (c == '\n') {
            break;
        }

        buffer[index++] = c;
    }

    buffer[index] = '\0'; // Null-terminate the string
}

// Function to evaluate a simple arithmetic expression
int evaluate_expression(const char *expr) {
    int result = 0;
    int current_number = 0;
    char current_operator = '+';

    for (const char *p = expr; *p != '\0'; p++) {
        char c = *p;

        if (c >= '0' && c <= '9') {
            current_number = current_number * 10 + (c - '0');
        } else if (c == '+' || c == '-' || c == '*' || c == '/') {
            // Apply the current operator to the result
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
                        printf("Error: Division by zero!\n");
                        return 0;
                    }
                    break;
            }

            // Update the operator and reset the current number
            current_operator = c;
            current_number = 0;
        }
    }

    // Apply the last number
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
                printf("Error: Division by zero!\n");
                return 0;
            }
            break;
    }

    return result;
}