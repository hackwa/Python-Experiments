#include <Python.h>
#include <unistd.h>

PyObject *hdmiInstance;

int downloadBitstream(){
    PyObject *pName, *pModule, *pSubModule, *pFunc;
    PyObject *pArgs, *pValue;

    char *module = "pynq";
    char *subModule = "Bitstream";
    char *bitstream = "audiovideo.bit";
    char *downloadMethod = "download";

    pName = PyUnicode_DecodeFSDefault(module);
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        pSubModule = PyObject_GetAttrString(pModule, subModule);
        /* pSubModule is a new reference */
        if (pSubModule && PyCallable_Check(pSubModule)) {
            pArgs = PyTuple_New(1);
            pValue = PyUnicode_DecodeFSDefault(bitstream);
            PyTuple_SetItem(pArgs, 0, pValue);
            pValue = PyObject_CallObject(pSubModule, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL) {
                printf("Bitstream Download Successful \n");
                pFunc = PyObject_GetAttrString(pValue,downloadMethod);
                if (pFunc && PyCallable_Check(pFunc)) {
                    PyObject_CallObject(pFunc,NULL);
                    Py_DECREF(pValue);
            	     Py_DECREF(pFunc);
                }
            }
	    else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return 1;
             }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", subModule);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n",module);
        return 1;
    }

    return 0;
}


PyObject  *startHdmi(){
    PyObject *pName, *pModule, *pSubModule, *pFunc;
    PyObject *pArgs, *pValue;

    char *module = "pynq.drivers";
    char *subModule = "HDMI";
    char *direction = "in";
    char *startMethod = "start";

    pName = PyUnicode_DecodeFSDefault(module);
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);


    if (pModule != NULL) {
        pSubModule = PyObject_GetAttrString(pModule, subModule);
        /* pSubModule is a new reference */
        if (pSubModule && PyCallable_Check(pSubModule)) {
            pArgs = PyTuple_New(1);
            pValue = PyUnicode_DecodeFSDefault(direction);
            PyTuple_SetItem(pArgs, 0, pValue);
            pValue = PyObject_CallObject(pSubModule, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL) {
                printf("HDMI Object Created \n");
                pFunc = PyObject_GetAttrString(pValue,startMethod);
                if (pFunc && PyCallable_Check(pFunc)) {
             	    PyObject_CallObject(pFunc,NULL);
            	   // Py_DECREF(pFunc);
            	    return pValue;
                }
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return NULL;
            }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", subModule);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n",module);
        return NULL;
    }

    return NULL;
}

void getCurrentFrame(PyObject* hdmiInstance, Py_buffer *view)
{
    PyObject *pFunc, *pFrame, *frameInstance, *pAttr;
    char *frameMethod = "frame_raw";
    int zed;
    pFunc = PyObject_GetAttrString(hdmiInstance,frameMethod);
    if (pFunc && PyCallable_Check(pFunc)) {
        pAttr = PyObject_CallObject(pFunc,NULL);
            if(PyObject_CheckBuffer(pAttr)){
                printf("Buffer interface supported by frameobject\n");
            zed = PyObject_GetBuffer(pAttr, view,PyBUF_INDIRECT);
            if (zed == 0 ) {
                printf("Success! Buffer len: %d\n",view->len);
            }
            else{
                printf("Failure!\n");
            }
        }
    }
}

int
main(int argc, char *argv[])
{
    PyObject *hdmiInstance;
    unsigned char * data;
    Py_buffer view ;
    Py_Initialize();
    downloadBitstream();
    sleep(1);
    hdmiInstance = startHdmi();
    getCurrentFrame(hdmiInstance,&view);
    data = (unsigned char *) view.buf;
    for(int i=0;i<view.len;i++)
        {
            if(data[i])
            printf("%d\t",data[i]);
       }
    Py_Finalize();
    return 0;
}
