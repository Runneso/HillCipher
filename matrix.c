#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>

#define ll long long
typedef ll **Matrix;
typedef ll *Vector;

static ll norm_mod(ll x, const ll mod) {
    x %= mod;
    if (x < 0) {
        x += mod;
    }
    return x;
}

static ll gcd(ll a, ll b) {
    while (b != 0) {
        const ll temp = a % b;
        a = b;
        b = temp;
    }
    return a < 0 ? -a : a;
}

static ll mod_inverse(ll a, const ll mod) {
    ll b = mod;
    ll x0 = 1, x1 = 0;

    a = norm_mod(a, mod);

    while (b != 0) {
        const ll q = a / b;

        const ll na = b;
        const ll nb = a - q * b;
        a = na;
        b = nb;

        const ll nx = x1;
        const ll ny = x0 - q * x1;
        x0 = nx;
        x1 = ny;
    }

    if (a != 1) {
        return -1;
    }

    return norm_mod(x0, mod);
}

static void swap_rows(Matrix matrix, const size_t row1, const size_t row2) {
    Vector tmp = matrix[row1];
    matrix[row1] = matrix[row2];
    matrix[row2] = tmp;
}

static ll determinant_int(const size_t n, Matrix matrix) {
    Matrix a = copy_matrix(n, n, matrix);

    ll sign = 1;
    ll prev = 1;

    for (size_t k = 0; k + 1 < n; k++) {
        size_t pivot = k;

        while (pivot < n && a[pivot][k] == 0) {
            pivot++;
        }

        if (pivot == n) {
            free_matrix(n, a);
            return 0;
        }

        if (pivot != k) {
            swap_rows(a, pivot, k);
            sign = -sign;
        }

        for (size_t i = k + 1; i < n; i++) {
            for (size_t j = k + 1; j < n; j++) {
                a[i][j] = (a[i][j] * a[k][k] - a[i][k] * a[k][j]) / prev;
            }
        }

        prev = a[k][k];
    }

    ll det = sign * a[n - 1][n - 1];

    free_matrix(n, a);
    return det;
}

static Matrix minor_matrix(const size_t n, Matrix matrix, const size_t row_skip, const size_t col_skip) {
    Matrix minor = allocate_matrix(n - 1, n - 1);

    size_t mi = 0;
    for (size_t i = 0; i < n; i++) {
        if (i == row_skip) {
            continue;
        }

        size_t mj = 0;
        for (size_t j = 0; j < n; j++) {
            if (j == col_skip) {
                continue;
            }

            minor[mi][mj] = matrix[i][j];
            mj++;
        }

        mi++;
    }

    return minor;
}

Matrix allocate_matrix(const size_t n, const size_t m) {
    Matrix matrix = malloc(n * sizeof(Vector));

    for (size_t index_i = 0; index_i < n; index_i++) {
        matrix[index_i] = malloc(m * sizeof(ll));
    }

    return matrix;
}

Matrix copy_matrix(const size_t n, const size_t m, Matrix matrix) {
    Matrix copy = allocate_matrix(n, m);

    for (size_t index_i = 0; index_i < n; index_i++) {
        for (size_t index_j = 0; index_j < m; index_j++) {
            copy[index_i][index_j] = matrix[index_i][index_j];
        }
    }

    return copy;
}

void free_matrix(const size_t n, Matrix matrix) {
    if (matrix == NULL) {
        return;
    }

    for (size_t index_i = 0; index_i < n; index_i++) {
        free(matrix[index_i]);
    }

    free(matrix);
}

void print_matrix(const size_t n, const size_t m, Matrix matrix) {
    for (size_t index_i = 0; index_i < n; index_i++) {
        for (size_t index_j = 0; index_j < m; index_j++) {
            printf("%lld ", matrix[index_i][index_j]);
        }
        printf("\n");
    }
}

ll determinant_matrix_mod(const size_t n, Matrix matrix, const ll mod) {
    const Matrix a = copy_matrix(n, n, matrix);

    ll sign = 1;
    ll prev = 1;

    for (size_t index_k = 0; index_k + 1 < n; index_k++) {
        size_t pivot = index_k;

        while (pivot < n && a[pivot][index_k] == 0) {
            pivot++;
        }

        if (pivot == n) {
            free_matrix(n, a);
            return 0;
        }

        if (pivot != index_k) {
            swap_rows(a, pivot, index_k);
            sign = -sign;
        }

        for (size_t index_i = index_k + 1; index_i < n; index_i++) {
            for (size_t index_j = index_k + 1; index_j < n; index_j++) {
                a[index_i][index_j] = (a[index_i][index_j] * a[index_k][index_k] - a[index_i][index_k] * a[index_k][
                                           index_j]) / prev;
            }
        }

        prev = a[index_k][index_k];
    }

    ll det = sign * a[n - 1][n - 1];

    free_matrix(n, a);
    return norm_mod(det, mod);
}

Matrix inverse_matrix_mod(const size_t n, Matrix matrix, const ll mod) {
    const ll det = determinant_int(n, matrix);
    const ll det_inv = mod_inverse(det, mod);

    if (det_inv == -1) {
        return NULL;
    }

    const Matrix inv = allocate_matrix(n, n);

    if (n == 1) {
        inv[0][0] = det_inv;
        return inv;
    }

    for (size_t index_i = 0; index_i < n; index_i++) {
        for (size_t index_j = 0; index_j < n; index_j++) {
            const Matrix minor = minor_matrix(n, matrix, index_i, index_j);
            const ll minor_det = determinant_int(n - 1, minor);
            free_matrix(n - 1, minor);

            const ll sign = ((index_i + index_j) % 2 == 0 ? 1 : -1);
            inv[index_j][index_i] = norm_mod(sign * minor_det * det_inv, mod);
        }
    }

    return inv;
}

Matrix multiply_matrix_mod(
    const size_t n,
    const size_t m,
    const size_t k,
    Matrix a,
    Matrix b,
    const ll mod
) {
    const Matrix res = allocate_matrix(n, k);

    for (size_t index_i = 0; index_i < n; index_i++) {
        for (size_t index_j = 0; index_j < k; index_j++) {
            res[index_i][index_j] = 0;

            for (size_t index_k = 0; index_k < m; index_k++) {
                res[index_i][index_j] = norm_mod(
                    res[index_i][index_j] + a[index_i][index_k] * b[index_k][index_j],
                    mod
                );
            }
        }
    }

    return res;
}

void multiply_vector_mod(
    const size_t n,
    Matrix matrix,
    const Vector vector,
    Vector result,
    const ll mod
) {
    for (size_t index_i = 0; index_i < n; index_i++) {
        result[index_i] = 0;

        for (size_t index_j = 0; index_j < n; index_j++) {
            result[index_i] = norm_mod(result[index_i] + matrix[index_i][index_j] * vector[index_j], mod);
        }
    }
}

int is_invertible_matrix_mod(const size_t n, Matrix matrix, const ll mod) {
    const ll det = determinant_matrix_mod(n, matrix, mod);
    return gcd(det, mod) == 1;
}

#undef ll
