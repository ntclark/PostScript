
#pragma once

#include <windows.h>
#include <float.h>

#define OBJECT_HEAP_SIZE   104857600

    class job;

    class object {
    public:

        enum objectType {
            atom = 0,
            procedure = 1,
            dictionary = 2,
            structureSpec = 3,
            directExecutable = 4,
            matrix = 5,
            array = 6,
            packedarray = 7,
            number = 8,
            logical = 9,
            mark = 10,
            null = 11,
            save = 12,
            pattern = 13,
            colorSpace = 14,
            font = 15,
            name = 16,
            literal = 17,
            resource = 18
        };

        enum valueType {
            invalidValueType = 99,
            unspecified = 100,
            container = 101,
            string = 102,
            character = 103,
            integer = 104,
            real = 105,
            radix = 106,
            executableProcedure = 107
        };

        static void initializeStorage();
        void *operator new(size_t size,void *pPtr);
        static void releaseStorage();

        object() {}

        object(job *,char c);
        object(job *,char *pszContents);
        object(job *,char *pszContents,objectType theType);
        object(job *,char *pszContents,objectType theType,valueType theValueType);

        object(job *,char *pStart,char *pEnd);
        object(job *,char *pStart,char *pEnd,objectType theType);
        object(job *,char *pStart,char *pEnd,objectType theType,valueType theValueType);

        object(job *,objectType theType);
        object(job *,objectType,valueType);
        object(job *,long value);
        object(job *,double value);

        virtual void put(long index,object *);
        virtual object *get(long index);
        virtual void execute();
        virtual char *ToString();

        virtual char *Name(char *pszNewName = NULL);

        virtual char *Contents(char *pszContents = NULL);

        bool IsSystemObject() { return isSystemObject; }

        char *TypeName();
        char *ValueTypeName();
        long IntValue(long value = -LONG_MAX);
        double DoubleValue(double value = -DBL_MAX);
        float FloatValue(float value = -FLT_MAX);
        double Value();

        void SetExecuteOnly(bool v) { isExecuteOnly = v; };
        bool IsExecuteOnly() { return isExecuteOnly; };

        enum objectType ObjectType() { return theType; };
        enum valueType ValueType( enum valueType vt = invalidValueType ) { if ( vt != invalidValueType ) theValueType = vt; return theValueType; };

        static void *pHeap;
        static void *pCurrentHeap;
        static void *pNextHeap;
        static size_t currentlyAllocatedHeap;

        static bool isCreatingSystemObjects;

    protected:

        job *pJob{NULL};
        bool isSystemObject{isCreatingSystemObjects};

        virtual ~object();

    private:

        void parseValue();

        object *pContainingDictionary{NULL};

        char *pszContents{NULL};
        char *pszName{NULL};

        union {
            long intValue{-LONG_MAX};
            double doubleValue;
        };

        objectType theType{null};
        valueType theValueType{unspecified};

        bool isExecuteOnly{false};

        friend class job;
        friend class dictionary;

    };
