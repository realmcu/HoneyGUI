#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdlib.h>
#include <string.h>

/* RTK FastLz constants - must match Windows tool */
#define MAX_COPY       32
#define MAX_LEN       264
#define MAX_DISTANCE   64

#define LITERAL_RUN    1
#define SHORT_MATCH    2
#define LONG_MATCH     3

typedef unsigned char  flzuint8;
typedef unsigned short flzuint16;
typedef unsigned int   flzuint32;

#define FASTLZ_READU16(p) ((p)[0] | (p)[1]<<8)
#define HASH_LOG  13
#define HASH_SIZE (1<< HASH_LOG)
#define HASH_MASK  (HASH_SIZE-1)
#define HASH_FUNCTION(v,p) { v = FASTLZ_READU16(p); v ^= FASTLZ_READU16(p+1)^(v>>(16-HASH_LOG));v &= HASH_MASK; }

/* RTK FastLz Level 1 Compressor - copied from Windows tool */
static int RTK_FASTLZ_COMPRESSOR_LEVEL1(const void *input, int length, void *output)
{
    const flzuint8 *ip = (const flzuint8 *)input;
    const flzuint8 *ip_bound = ip + length - 2;
    const flzuint8 *ip_limit = ip + length - 12;
    flzuint8 *op = (flzuint8 *)output;
    flzuint32 instruction_type;

    /* declare and initialize HASH table */
    const flzuint8 **hslot;
    flzuint32 hval;
    flzuint32 copy;
    const flzuint8 **htab = (const flzuint8 **)malloc(sizeof(flzuint8 *) * HASH_SIZE);
    if (htab == NULL)
    {
        return -1;
    }
    for (hslot = htab; hslot < htab + HASH_SIZE; hslot++)
    {
        *hslot = ip;
    }

    /* sanity check */
    if (length < 4)
    {
        if (length)
        {
            *op++ = length - 1;
            ip_bound++;
            while (ip <= ip_bound)
            {
                *op++ = *ip++;
            }
            free(htab);
            return op - (flzuint8 *)output;
        }
        else
        {
            free(htab);
            return 0;
        }
    }

    /* start compressor with literal copy */
    instruction_type = LITERAL_RUN;
    copy = 2;
    *op++ = MAX_COPY - 1;
    *op++ = *ip++;
    *op++ = *ip++;

    /* main loop of compress procedure */
    while (ip < ip_limit)
    {
        const flzuint8 *anchor = ip;
        const flzuint8 *ref;
        flzuint32 distance;
        flzuint32 len = 3;

        /* find potential match through HASH FUNCTION */
        HASH_FUNCTION(hval, ip);
        hslot = htab + hval;
        ref = htab[hval];
        distance = anchor - ref;

        /* update hash table */
        *hslot = anchor;

        /* verify if the potential match is an actual match */
        if ((distance == 0) || (distance >= MAX_DISTANCE) ||
            (*ref++ != *ip++) || (*ref++ != *ip++) || (*ref++ != *ip++))
        {
            instruction_type = LITERAL_RUN;
        }
        else
        {
            /* match has been found */
            if (copy)
            {
                *(op - copy - 1) = copy - 1;
            }
            else
            {
                op--;
            }
            copy = 0;

            distance--;
            ip = anchor + len;

            /* seek for the largest match length */
            while (ip < ip_bound)
                if (*ref++ != *ip++) { break; }
            ip -= 1;
            len = ip - anchor;

            if ((len - 2) < 7)
            {
                instruction_type = SHORT_MATCH;
            }
            else
            {
                instruction_type = LONG_MATCH;
            }
        }

        /* output code for each instruction */
        if (instruction_type == LITERAL_RUN)
        {
            *op++ = *anchor++;
            ip = anchor;
            copy++;
            if (copy == MAX_COPY)
            {
                copy = 0;
                *op++ = MAX_COPY - 1;
            }
        }
        else if (instruction_type == SHORT_MATCH)
        {
            *op++ = ((len - 2) << 5) + (distance >> 8);
            *op++ = (distance & 255);

            /* update the hash at match boundary */
            HASH_FUNCTION(hval, (ip - 1));
            htab[hval] = (ip - 1);
            HASH_FUNCTION(hval, (ip - 2));
            htab[hval] = (ip - 2);

            *op++ = MAX_COPY - 1;
        }
        else
        {
            if (len > MAX_LEN)
            {
                while (len > MAX_LEN)
                {
                    *op++ = (7 << 5) + (distance >> 8);
                    *op++ = MAX_LEN - 2 - 7 - 2;
                    *op++ = (distance & 255);
                    len -= MAX_LEN - 2;
                }
            }
            len -= 2;
            if (len < 7)
            {
                *op++ = (len << 5) + (distance >> 8);
                *op++ = (distance & 255);
            }
            else
            {
                *op++ = (7 << 5) + (distance >> 8);
                *op++ = len - 7;
                *op++ = (distance & 255);
            }

            /* update the hash at match boundary */
            HASH_FUNCTION(hval, (ip - 1));
            htab[hval] = (ip - 1);
            HASH_FUNCTION(hval, (ip - 2));
            htab[hval] = (ip - 2);

            *op++ = MAX_COPY - 1;
        }
    }

    /* left-over as literal copy */
    ip_bound++;
    while (ip <= ip_bound)
    {
        *op++ = *ip++;
        copy++;
        if (copy == MAX_COPY)
        {
            copy = 0;
            *op++ = MAX_COPY - 1;
        }
    }

    /* adjust the last copy length */
    if (copy)
    {
        *(op - copy - 1) = copy - 1;
    }
    else
    {
        op--;
    }

    free(htab);
    return op - (flzuint8 *)output;
}

/* Python wrapper function */
static PyObject *rtk_fastlz_compress(PyObject *self, PyObject *args)
{
    const char *input;
    Py_ssize_t input_len;
    char *output;
    int output_len;
    PyObject *result;

    if (!PyArg_ParseTuple(args, "y#", &input, &input_len))
    {
        return NULL;
    }

    /* Allocate output buffer */
    output_len = (int)(input_len * 1.05) + 66;
    output = (char *)malloc(output_len);
    if (output == NULL)
    {
        return PyErr_NoMemory();
    }

    /* Compress using RTK FastLz Level 1 */
    output_len = RTK_FASTLZ_COMPRESSOR_LEVEL1(input, (int)input_len, output);

    if (output_len < 0)
    {
        free(output);
        PyErr_SetString(PyExc_RuntimeError, "Compression failed");
        return NULL;
    }

    /* Return compressed data as bytes */
    result = PyBytes_FromStringAndSize(output, output_len);
    free(output);
    return result;
}

/* Module method definitions */
static PyMethodDef RtkFastlzMethods[] =
{
    {
        "compress", rtk_fastlz_compress, METH_VARARGS,
        "Compress data using RTK FastLz (MAX_DISTANCE=64)\n\n"
        "Args:\n"
        "    data (bytes): Input data to compress\n\n"
        "Returns:\n"
        "    bytes: Compressed data (without header)\n"
    },
    {NULL, NULL, 0, NULL}
};

/* Module definition */
static struct PyModuleDef rtk_fastlz_module =
{
    PyModuleDef_HEAD_INIT,
    "rtk_fastlz",
    "RTK FastLz compression module - compatible with Windows tool (MAX_DISTANCE=64)",
    -1,
    RtkFastlzMethods
};

/* Module initialization */
PyMODINIT_FUNC PyInit_rtk_fastlz(void)
{
    return PyModule_Create(&rtk_fastlz_module);
}
