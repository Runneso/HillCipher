#include "matrix.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>
#include <time.h>
#include <string.h>

#define ll long long

int validate_text(const size_t n, const char *text) {
    for (size_t index = 0; index < n; index++) {
        if (text[index] < 'A' || text[index] > 'Z') {
            return 0;
        }
    }

    return 1;
}

int validate_length_key(const size_t n, const size_t m) {
    size_t root = 1;

    while (root * root < m) {
        root++;
    }

    return root * root == m && n % root == 0;
}

Vector parse_text_to_vector(const size_t n, const char *text) {
    Vector vector = malloc(n * sizeof(ll));

    for (size_t index = 0; index < n; index++) {
        vector[index] = text[index] - 'A';
    }

    return vector;
}

Matrix parse_text_to_matrix(const size_t m, const char *text) {
    const size_t n = sqrt(m);
    Matrix matrix = allocate_matrix(n, n);

    for (size_t index_i = 0; index_i < n; index_i++) {
        for (size_t index_j = 0; index_j < n; index_j++) {
            matrix[index_i][index_j] = text[index_i * n + index_j] - 'A';
        }
    }

    return matrix;
}

Matrix generate_valid_matrix(const size_t n) {
    Matrix matrix = allocate_matrix(n, n);
    if (matrix == NULL) {
        return NULL;
    }

    do {
        for (size_t index_i = 0; index_i < n; index_i++) {
            for (size_t index_j = 0; index_j < n; index_j++) {
                matrix[index_i][index_j] = rand() % 26;
            }
        }
    } while (!is_invertible_matrix_mod(n, matrix, 26));

    return matrix;
}

int main(const int argc, char **argv) {
    srand(time(NULL));

    if (argc != 2) {
        fprintf(stderr, "Usage: %s standard|recursive\n", argv[0]);
        return 1;
    }

    const int is_recursive = strcmp(argv[1], "recursive") == 0;
    const int is_standard = strcmp(argv[1], "standard") == 0;

    if (!is_standard && !is_recursive) {
        fprintf(stderr, "Mode must be standard or recursive.\n");
        return 1;
    }

    size_t n, m;

    printf("Enter the size of text: \n");
    scanf("%zu", &n);

    char *text = malloc((n + 1) * sizeof(char));
    if (text == NULL) {
        return 1;
    }

    printf("Enter text: \n");
    scanf("%s", text);

    if (!validate_text(n, text)) {
        fprintf(stderr, "Text must consist of uppercase letters only.\n");
        free(text);
        return 1;
    }

    Vector vector = parse_text_to_vector(n, text);
    free(text);

    printf(
        "1. Enter cipher key \n"
        "2. Generate random cipher key \n"
    );

    ll key_choice;
    scanf("%lld", &key_choice);

    if (key_choice != 1 && key_choice != 2) {
        fprintf(stderr, "Invalid choice.\n");
        free(vector);
        return 1;
    }

    printf("Enter size of cipher key: \n");
    scanf("%zu", &m);

    if (!validate_length_key(n, m)) {
        fprintf(stderr, "Text size must be divisible by cipher key size.\n");
        free(vector);
        return 1;
    }

    size_t block_size = 1;
    while (block_size * block_size < m) {
        block_size++;
    }

    Matrix key1 = NULL;
    Matrix key2 = NULL;

    if (key_choice == 1) {
        char *key_text = malloc((m + 1) * sizeof(char));
        if (key_text == NULL) {
            free(vector);
            return 1;
        }

        printf("Enter cipher key: \n");
        scanf("%s", key_text);

        if (!validate_text(m, key_text)) {
            fprintf(stderr, "Cipher key must consist of uppercase letters only.\n");
            free(key_text);
            free(vector);
            return 1;
        }

        key1 = parse_text_to_matrix(m, key_text);
        free(key_text);

        if (is_recursive) {
            key_text = malloc((m + 1) * sizeof(char));
            if (key_text == NULL) {
                free_matrix(block_size, key1);
                free(vector);
                return 1;
            }

            printf("Enter second cipher key: \n");
            scanf("%s", key_text);

            if (!validate_text(m, key_text)) {
                fprintf(stderr, "Cipher key must consist of uppercase letters only.\n");
                free(key_text);
                free_matrix(block_size, key1);
                free(vector);
                return 1;
            }

            key2 = parse_text_to_matrix(m, key_text);
            free(key_text);
        }
    } else {
        key1 = generate_valid_matrix(block_size);

        if (is_recursive) {
            key2 = generate_valid_matrix(block_size);
        }

        printf("Generated key 1:\n");
        print_matrix(block_size, block_size, key1);

        if (is_recursive) {
            printf("Generated key 2:\n");
            print_matrix(block_size, block_size, key2);
        }
    }

    if (key1 == NULL || (is_recursive && key2 == NULL)) {
        free_matrix(block_size, key1);
        free_matrix(block_size, key2);
        free(vector);
        return 1;
    }

    if (!is_invertible_matrix_mod(block_size, key1, 26) ||
        (is_recursive && !is_invertible_matrix_mod(block_size, key2, 26))) {
        fprintf(stderr, "Cipher key must be invertible modulo 26.\n");
        free_matrix(block_size, key1);
        free_matrix(block_size, key2);
        free(vector);
        return 1;
    }

    printf(
        "1. Encrypt \n"
        "2. Decrypt \n"
    );

    ll action;
    scanf("%lld", &action);

    if (action != 1 && action != 2) {
        fprintf(stderr, "Invalid choice.\n");
        free_matrix(block_size, key1);
        free_matrix(block_size, key2);
        free(vector);
        return 1;
    }

    Vector result = malloc(n * sizeof(ll));
    Vector block_result = malloc(block_size * sizeof(ll));

    if (result == NULL || block_result == NULL) {
        free(result);
        free(block_result);
        free_matrix(block_size, key1);
        free_matrix(block_size, key2);
        free(vector);
        return 1;
    }

    if (is_standard) {
        Matrix work_key = key1;

        if (action == 2) {
            work_key = inverse_matrix_mod(block_size, key1, 26);

            if (work_key == NULL) {
                fprintf(stderr, "Cipher key must be invertible modulo 26.\n");
                free(result);
                free(block_result);
                free_matrix(block_size, key1);
                free(vector);
                return 1;
            }
        }

        for (size_t index_i = 0; index_i < n; index_i += block_size) {
            multiply_vector_mod(block_size, work_key, vector + index_i, block_result, 26);

            for (size_t index_j = 0; index_j < block_size; index_j++) {
                result[index_i + index_j] = block_result[index_j];
            }
        }

        if (action == 2) {
            free_matrix(block_size, work_key);
        }
    } else {
        Matrix prev2 = copy_matrix(block_size, block_size, key1);
        Matrix prev1 = copy_matrix(block_size, block_size, key2);

        for (size_t block = 0, index_i = 0; index_i < n; block++, index_i += block_size) {
            Matrix current_key = NULL;

            if (block == 0) {
                current_key = copy_matrix(block_size, block_size, key1);
            } else if (block == 1) {
                current_key = copy_matrix(block_size, block_size, key2);
            } else {
                current_key = multiply_matrix_mod(block_size, block_size, block_size, prev2, prev1, 26);
                free_matrix(block_size, prev2);
                prev2 = prev1;
                prev1 = copy_matrix(block_size, block_size, current_key);
            }

            Matrix work_key = current_key;

            if (action == 2) {
                work_key = inverse_matrix_mod(block_size, current_key, 26);
            }

            if (work_key == NULL) {
                fprintf(stderr, "Cipher key must be invertible modulo 26.\n");
                free_matrix(block_size, current_key);
                free_matrix(block_size, prev1);
                free_matrix(block_size, prev2);
                free(result);
                free(block_result);
                free_matrix(block_size, key1);
                free_matrix(block_size, key2);
                free(vector);
                return 1;
            }

            multiply_vector_mod(block_size, work_key, vector + index_i, block_result, 26);

            for (size_t index_j = 0; index_j < block_size; index_j++) {
                result[index_i + index_j] = block_result[index_j];
            }

            if (action == 2) {
                free_matrix(block_size, work_key);
            }

            free_matrix(block_size, current_key);
        }

        free_matrix(block_size, prev1);
        free_matrix(block_size, prev2);
    }

    printf("Result:\n");

    for (size_t index = 0; index < n; index++) {
        printf("%c", (char) (result[index] + 'A'));
    }

    printf("\n");

    free(result);
    free(block_result);
    free_matrix(block_size, key1);
    free_matrix(block_size, key2);
    free(vector);

    return 0;
}

#undef ll
