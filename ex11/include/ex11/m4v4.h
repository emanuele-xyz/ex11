#pragma once

/*
    Conventions: row-major matrices, row vectors, and pre-multiplication
*/

/*
    Writes into m the identity matrix
*/
inline void m4_identity(float *restrict m)
{
    m[0]  = 1.0f; m[1]  = 0.0f; m[2]  = 0.0f; m[3]  = 0.0f;
    m[4]  = 0.0f; m[5]  = 1.0f; m[6]  = 0.0f; m[7]  = 0.0f;
    m[8]  = 0.0f; m[9]  = 0.0f; m[10] = 1.0f; m[11] = 0.0f;
    m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;
}

/*
   Writes into m the translation matrix given by a
*/
inline void m4_translation(float *restrict a, float *restrict m)
{
    m[0]  = 1.0f; m[1]  = 0.0f; m[2]  = 0.0f; m[3]  = 0.0f;
    m[4]  = 0.0f; m[5]  = 1.0f; m[6]  = 0.0f; m[7]  = 0.0f;
    m[8]  = 0.0f; m[9]  = 0.0f; m[10] = 1.0f; m[11] = 0.0f;
    m[12] = a[0]; m[13] = a[1]; m[14] = a[2]; m[15] = 1.0f;
}

/*
    Writes into m the scaling matrix given by a
*/
inline void m4_scaling(float *restrict a, float *restrict m)
{
    m[0]  = a[0]; m[1]  = 0.0f; m[2]  = 0.0f; m[3]  = 0.0f;
    m[4]  = 0.0f; m[5]  = a[1]; m[6]  = 0.0f; m[7]  = 0.0f;
    m[8]  = 0.0f; m[9]  = 0.0f; m[10] = a[2]; m[11] = 0.0f;
    m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;
}

/*
    Writes into b the result of multiplying a by m
*/
inline void v4m4_multiply(float *restrict a, float *restrict m, float *restrict b)
{
    b[0] = a[0]*m[0] + a[1]*m[4] + a[2]*m[8]  + a[3]*m[12];
    b[1] = a[0]*m[1] + a[1]*m[5] + a[2]*m[9]  + a[3]*m[13];
    b[2] = a[0]*m[2] + a[1]*m[6] + a[2]*m[10] + a[3]*m[14];
    b[3] = a[0]*m[3] + a[1]*m[7] + a[2]*m[11] + a[3]*m[15];
}
