/*
 * File      : gui_img.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <string.h>
#include <gui_matrix.h>
#include <math.h>

void matrix_identity(struct gui_matrix *matrix)
{
    /* Set identify matrix. */
    matrix->m[0][0] = 1.0f;
    matrix->m[0][1] = 0.0f;
    matrix->m[0][2] = 0.0f;
    matrix->m[1][0] = 0.0f;
    matrix->m[1][1] = 1.0f;
    matrix->m[1][2] = 0.0f;
    matrix->m[2][0] = 0.0f;
    matrix->m[2][1] = 0.0f;
    matrix->m[2][2] = 1.0f;
}
void multiply(struct gui_matrix *matrix, struct gui_matrix *mult)
{
    struct gui_matrix temp;
    int row, column;

    /* Process all rows. */
    for (row = 0; row < 3; row++)
    {
        /* Process all columns. */
        for (column = 0; column < 3; column++)
        {
            /* Compute matrix entry. */
            temp.m[row][column] = (matrix->m[row][0] * mult->m[0][column])
                                  + (matrix->m[row][1] * mult->m[1][column])
                                  + (matrix->m[row][2] * mult->m[2][column]);
        }
    }

    /* Copy temporary matrix into result. */
    memcpy(matrix, &temp, sizeof(temp));
}

void pox_mul(struct gui_matrix *matrix, struct gui_pox *pox)
{
    struct gui_pox temp;
    int row;

    /* Process all rows. */
    for (row = 0; row < 3; row++)
    {
        temp.p[row] = (matrix->m[row][0] * pox->p[0])
                      + (matrix->m[row][1] * pox->p[1])
                      + (matrix->m[row][2] * pox->p[2]);
    }
    temp.p[0] = temp.p[0] / temp.p[2];
    temp.p[1] = temp.p[1] / temp.p[2];
    temp.p[2] = 1;

    memcpy(pox, &temp, sizeof(temp));
}

void matrix_translate(float x, float y, struct gui_matrix *matrix)
{
    /* Set translation matrix. */
    struct gui_matrix t = { { {1.0f, 0.0f, x},
            {0.0f, 1.0f, y},
            {0.0f, 0.0f, 1.0f}
        }
    };

    /* Multiply with current matrix. */
    multiply(matrix, &t);
}

void matrix_rotate(float degrees, struct gui_matrix *matrix)
{
    /* Convert degrees into radians. */
    float angle = degrees / 180.0f * 3.1415926f;

    /* Compuet cosine and sine values. */
    float cos_angle = cosf(angle);
    float sin_angle = sinf(angle);

    /* Set rotation matrix. */
    struct gui_matrix r = { { {cos_angle, -sin_angle, 0.0f},
            {sin_angle, cos_angle, 0.0f},
            {0.0f, 0.0f, 1.0f}
        }
    };

    /* Multiply with current matrix. */
    multiply(matrix, &r);
}

void matrix_inverse(struct gui_matrix *matrix)
{
    struct gui_matrix temp;

    float detal = matrix->m[0][0] * matrix->m[1][1] * matrix->m[2][2] + \
                  matrix->m[0][1] * matrix->m[1][2] * matrix->m[2][0] + \
                  matrix->m[0][2] * matrix->m[1][0] * matrix->m[2][1] - \
                  matrix->m[0][0] * matrix->m[1][2] * matrix->m[2][1] - \
                  matrix->m[0][1] * matrix->m[1][0] * matrix->m[2][2] - \
                  matrix->m[0][2] * matrix->m[1][1] * matrix->m[2][0];

    temp.m[0][0] = (matrix->m[1][1] * matrix->m[2][2] - matrix->m[1][2] * matrix->m[2][1]) / detal;
    temp.m[1][0] = -(matrix->m[2][2] * matrix->m[1][0] - matrix->m[1][2] * matrix->m[2][0]) / detal;
    temp.m[2][0] = (matrix->m[1][0] * matrix->m[2][1] - matrix->m[2][0] * matrix->m[1][1]) / detal;

    temp.m[0][1] = -(matrix->m[0][1] * matrix->m[2][2] - matrix->m[0][2] * matrix->m[2][1]) / detal;
    temp.m[1][1] = (matrix->m[2][2] * matrix->m[0][0] - matrix->m[2][0] * matrix->m[0][2]) / detal;
    temp.m[2][1] = -(matrix->m[0][0] * matrix->m[2][1] - matrix->m[2][0] * matrix->m[0][1]) / detal;

    temp.m[0][2] = (matrix->m[0][1] * matrix->m[1][2] - matrix->m[1][1] * matrix->m[0][2]) / detal;
    temp.m[1][2] = -(matrix->m[0][0] * matrix->m[1][2] - matrix->m[0][2] * matrix->m[1][0]) / detal;
    temp.m[2][2] = (matrix->m[0][0] * matrix->m[1][1] - matrix->m[0][1] * matrix->m[1][0]) / detal;


    /* Copy temporary matrix into result. */
    memcpy(matrix, &temp, sizeof(temp));
}

void matrix_scale(float scale_x, float scale_y, struct gui_matrix *matrix)
{
    /* Set scale matrix. */
    struct gui_matrix s = { { {scale_x, 0.0f, 0.0f},
            {0.0f, scale_y, 0.0f},
            {0.0f, 0.0f, 1.0f}
        }
    };

    /* Multiply with current matrix. */
    multiply(matrix, &s);
}

void matrix_perspective(float px, float py, struct gui_matrix *matrix)
{
    /* set prespective matrix */
    struct gui_matrix p = { { {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {px, py, 1.0f}
        }
    };
    /* Multiply with current matrix. */
    multiply(matrix, &p);
}
