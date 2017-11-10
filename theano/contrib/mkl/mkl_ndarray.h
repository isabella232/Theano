#ifndef _MKL_NDARRAY_H_
#define _MKL_NDARRAY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <numpy/arrayobject.h>
#include "mkl.h"
#include "mkl_dnn.h"
#include "theano_mod_helper.h"

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)(-1))
#endif

#ifndef Py_TYPE
#define Py_TYPE(o) ((o)->ob_type)
#endif

// MNDA stands for MklNDArray
#define MNDA_MAX_NDIM   (16)
#define MNDA_DATA       (0)
#define MNDA_WORKSPACE  (1)
#define MNDA_FLOAT32    (11)
#define MNDA_FLOAT64    (12)

// Flag
#define MNDA_VIEW_FROM_MKL   (0x00000001)
#define MNDA_VIEW_FROM_NP    (0x00000002)

// Placeholder for dtype name. Only support FP32 and FP64 currently.
// Other types will be supported in future.
char* MNDA_TYPE[] = {"", "", "", "", "", "", "", "",
                     "", "", "", "float32", "float64", ""};

#if PY_MAJOR_VERSION >= 3
// Py3k treats all ints as longs. This one is not caught by npy_3kcompat.h.
#define PyNumber_Int PyNumber_Long

#include "numpy/npy_3kcompat.h"

// Py3k strings are unicode, these mimic old functionality.
//
// NOTE: npy_3kcompat.h replaces PyString_X with PyBytes_X, which breaks
// compatibility with some functions returning text.
#define PyString_Check PyUnicode_Check
#define PyString_FromString PyUnicode_FromString
#define PyString_AsString PyUnicode_AsUTF8
#define PyString_FromStringAndSize PyUnicode_FromStringAndSize
#define PyString_Size PyUnicode_GET_SIZE
#define PyInt_FromSize_t PyLong_FromSize_t

// Python 3 expects a PyObject* as the first argument to PySlice_GetIndicesEx().
#define SLICE_CAST(x) (x)
#else
// Python 2 expects a PySliceObject* as the first argument to PySlice_GetIndicesEx().
#define SLICE_CAST(x) ((PySliceObject*)(x))
#endif // end #if PY_MAJOR_VERSION >= 3


typedef struct __MKL_NDARRAY__{

    PyObject_HEAD
    PyObject * base;                // reference for data

    int     flag;
    int     nd;                     // the number of dimensions of the tensor, maximum is 16 (MNDA_MAX_NDIM).
    int     dtype;                  // an integer type number is given here.
    size_t  data_size;              // the number of bytes allocated for data
    size_t  user_structure[2 * MNDA_MAX_NDIM];  // user layout: [size0, size1, ..., stride0, stride1, ..., 0, 0].
    dnnLayout_t layout;
    void*   data;           // data buffer
}MKLNdarray;


// common APIs
MOD_PUBLIC int MKLNdarray_Check(const PyObject* ob);
MOD_PUBLIC int MKLNdarray_set_structure(MKLNdarray* self, int nd, const size_t* dims, const size_t* strides);
MOD_PUBLIC int MKLNdarray_CopyFromArray(MKLNdarray* self, PyArrayObject* obj);
MOD_PUBLIC int MKLNdarray_ViewFromArray(MKLNdarray* self, PyArrayObject* obj);
MOD_PUBLIC PyObject* MKLNdarray_New(int nd, int typenum);
MOD_PUBLIC PyObject* MKLNdarray_CreateArrayObj(const MKLNdarray* self);
MOD_PUBLIC void* MKLNdarray_DATA(const MKLNdarray* self);
MOD_PUBLIC dnnLayout_t MKLNdarray_LAYOUT(const MKLNdarray* self);
MOD_PUBLIC const size_t* MKLNdarray_DIMS(const MKLNdarray* self);
MOD_PUBLIC const size_t* MKLNdarray_STRIDES(const MKLNdarray* self);
MOD_PUBLIC int MKLNdarray_NDIM(const MKLNdarray* self);
MOD_PUBLIC int MKLNdarray_TYPE(const MKLNdarray* self);

MOD_PUBLIC int MKLNdarray_create_buffer_from_primitive(MKLNdarray *self,
                                                       const dnnPrimitive_t *prim,
                                                       dnnResourceType_t res_type);
MOD_PUBLIC int MKLNdarray_create_buffer_from_layout(MKLNdarray *self);
MOD_PUBLIC int MKLNdarray_create_buffer_from_structure(MKLNdarray *self);

#endif