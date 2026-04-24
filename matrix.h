#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

#define ll long long
typedef ll **Matrix;
typedef ll *Vector;

Matrix allocate_matrix(size_t n, size_t m);

Matrix copy_matrix(size_t n, size_t m, Matrix matrix);

void free_matrix(size_t n, Matrix matrix);

void print_matrix(size_t n, size_t m, Matrix matrix);

ll determinant_matrix_mod(size_t n, Matrix matrix, ll mod);

Matrix inverse_matrix_mod(size_t n, Matrix matrix, ll mod);

Matrix multiply_matrix_mod(size_t n, size_t m, size_t k, Matrix a, Matrix b, ll mod);

void multiply_vector_mod(size_t n, Matrix matrix, Vector vector, Vector result, ll mod);

int is_invertible_matrix_mod(size_t n, Matrix matrix, ll mod);
#undef  ll
#endif
