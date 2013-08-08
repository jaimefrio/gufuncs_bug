#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include "Python.h"
#include "numpy/ndarraytypes.h"
#include "numpy/ufuncobject.h"
#include "numpy/npy_3kcompat.h"
#include<stdio.h>

static const char* gufuncs_bug_version_string = "0.1";
#define GUFUNCS_BUG_MODULE_NAME "gufuncs_bug"

/*
 *****************************************************************************
 **                              GUFUNC LOOPS                               **
 *****************************************************************************
 */
 
/*
    Expects two input arguments with shapes (m, n), (o, p),
    returns a scalar, shape ().
 */
static void
gufuncs_bug_different_dim(char **args,
                          npy_intp *dimensions,
                          npy_intp *steps,
                          void *data)
{
    char *in1 = args[0],
         *in2 = args[1],
         *out = args[2];
    npy_intp loop_len = dimensions[0],
             m = dimensions[1],
             n = dimensions[2],
             o = dimensions[3],
             p = dimensions[4];
    npy_intp in1_loop_stride = steps[0],
             in2_loop_stride = steps[1],
             out_loop_stride = steps[2],
             in1_m_stride = steps[3],
             in1_n_stride = steps[4],
             in2_o_stride = steps[5],
             in2_p_stride = steps[6];

    printf("In gufuncs_bug_different_dim function:\n\n");
    printf("loop_len: %d\n\n", loop_len);
    printf("in1 args\nm: %d, n: %d\n", m, n);
    printf("strides: %d (loop), %d (m), %d (n)\n\n", in1_loop_stride,
                                                     in1_m_stride,
                                                     in1_n_stride);
    printf("in2 args\no: %d, p: %d\n", o, p);
    printf("strides: %d (loop), %d (o), %d (p)\n\n", in2_loop_stride,
                                                     in2_o_stride,
                                                     in2_p_stride);
} /* gufuncs_bug_different_dim */

/*
    Expects two input arguments with shapes (m, m), (o, p),
    returns a scalar, shape ().
 */
static void
gufuncs_bug_same_dim(char **args,
                          npy_intp *dimensions,
                          npy_intp *steps,
                          void *data)
{
    char *in1 = args[0],
         *in2 = args[1],
         *out = args[2];
    npy_intp loop_len = dimensions[0],
             m = dimensions[1],
             o = dimensions[2],
             p = dimensions[3];
    npy_intp in1_loop_stride = steps[0],
             in2_loop_stride = steps[1],
             out_loop_stride = steps[2],
             in1_m1_stride = steps[3],
             in1_m2_stride = steps[4],
             in2_o_stride = steps[5],
             in2_p_stride = steps[6];

    printf("In gufuncs_bug_same_dim function:\n\n");
    printf("loop_len: %d\n\n", loop_len);
    printf("in1 args\nm: %d\n", m);
    printf("strides: %d (loop), %d (m1), %d (m2)\n\n", in1_loop_stride,
                                                     in1_m1_stride,
                                                     in1_m2_stride);
    printf("in2 args\no: %d, p: %d\n", o, p);
    printf("strides: %d (loop), %d (o), %d (p)\n\n", in2_loop_stride,
                                                     in2_o_stride,
                                                     in2_p_stride);
} /* gufuncs_bug_same_dim */

/*
 *****************************************************************************
 **                           GUFUNC REGISTRATION                           **
 *****************************************************************************
 */

static void *array_of_nulls[] = {
    (void *)NULL,
    (void *)NULL,
    (void *)NULL,
    (void *)NULL,

    (void *)NULL,
    (void *)NULL,
    (void *)NULL,
    (void *)NULL,

    (void *)NULL,
    (void *)NULL,
    (void *)NULL,
    (void *)NULL,

    (void *)NULL,
    (void *)NULL,
    (void *)NULL,
    (void *)NULL
};
 
 static char test_types[] = {
    NPY_DOUBLE, NPY_DOUBLE, NPY_DOUBLE
};

PyUFuncGenericFunction test_diff_funcs[] = {
    &gufuncs_bug_different_dim
};

PyUFuncGenericFunction test_same_funcs[] = {
    &gufuncs_bug_same_dim
};

typedef struct gufunc_descriptor_struct {
    char *name;
    char *signature;
    char *doc;
    int ntypes;
    int nin;
    int nout;
    PyUFuncGenericFunction *funcs;
    char *types;
} GUFUNC_DESCRIPTOR_t;

GUFUNC_DESCRIPTOR_t gufunc_descriptors[] = {
    /*
        There is a bug in the gufuncs machinery, and arguments with
        repeated dimensions don't get the right strides dispatched to
        the gufunc loop function. To work around this, different
        names are used for the dimensions of the look-up-table in
        the gufunc signatures below, and the gufunc loops assume that
        they will all be the same.
     */
    {
        "test_diff",
        "(m,n),(o,p)->()",
        "test with different core dimensions\n",
        1, 2, 1,
        test_diff_funcs,
        test_types
    },
    {
        "test_same",
        "(m,m),(o,p)->()",
        "test with same core dimensions\n",
        1, 2, 1,
        test_same_funcs,
        test_types
    }
}; /* End of gufunc_descriptors declaration */

static void
addUfuncs(PyObject *dictionary) {
    PyObject *f;
    int i;
    const int gufunc_count = sizeof(gufunc_descriptors) /
                             sizeof(gufunc_descriptors[0]);
    for (i=0; i < gufunc_count; i++) {
        GUFUNC_DESCRIPTOR_t* d = &gufunc_descriptors[i];
        f = PyUFunc_FromFuncAndDataAndSignature(d->funcs,
                                                array_of_nulls,
                                                d->types,
                                                d->ntypes,
                                                d->nin,
                                                d->nout,
                                                PyUFunc_None,
                                                d->name,
                                                d->doc,
                                                0,
                                                d->signature);
        PyDict_SetItemString(dictionary, d->name, f);
        Py_DECREF(f);
    }
}

/*
 *****************************************************************************
 **                          MODULE INITIALIZATION                          **
 *****************************************************************************
 */
 
 static PyMethodDef gufuncs_bug_methods[] = {
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

#if defined(NPY_PY3K)
static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        GUFUNCS_BUG_MODULE_NAME,
        NULL,
        -1,
        gufuncs_bug_methods,
        NULL,
        NULL,
        NULL,
        NULL
};
#endif

#if defined(NPY_PY3K)
#define RETVAL m
PyObject *PyInit_gufuncs_bug(void)
#else
#define RETVAL
PyMODINIT_FUNC
initgufuncs_bug(void)
#endif
{
    PyObject *m;
    PyObject *d;
    PyObject *version;

#if defined(NPY_PY3K)
    m = PyModule_Create(&moduledef);
#else
    m = Py_InitModule(GUFUNCS_BUG_MODULE_NAME, gufuncs_bug_methods);
#endif
    if (m == NULL)
        return RETVAL;

    import_array();
    import_ufunc();

    d = PyModule_GetDict(m);

    version = PyString_FromString(gufuncs_bug_version_string);
    PyDict_SetItemString(d, "__version__", version);
    Py_DECREF(version);

    /* Load the ufunc operators into the module's namespace */
    addUfuncs(d);

    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_RuntimeError,
                        "cannot load _gufuncs module.");
    }

    return RETVAL;
}
