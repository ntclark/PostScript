
#pragma once

#include <windows.h>
#include <float.h>

#define OBJECT_HEAP_SIZE   104857600

typedef double POINT_TYPE;

#define OBJECT_POINT_TYPE_VALUE DoubleValue()
#define OBJECT_SET_POINT_TYPE_VALUE(v) DoubleValue(v)

#define PICA_FACTOR 128

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
            resource = 18,
            file = 19,
            filter = 20,
            objectTypeUnspecified = 99
        };

        enum valueClassification {
            simple = 50,
            composite = 51
        };

        enum valueType {
            invalidValueType = 99,
            valueTypeUnspecified = 100,
            container = 101,
            string = 102,
            constantString = 103,
            hexString = 104,
            binaryString = 105,
            character = 106,
            integer = 107,
            real = 108,
            radix = 109,
            trueOrFalse = 110,
            arrayMark = 111,
            dictionaryMark = 112,
            procedureMark = 113,
            executableProcedure = 114,
            executableOperator = 115
        };

        enum executableAttribute {
            nonExecutable = 201,
            executable = 202
        };

        enum accessAttribute {
            unlimited = 301,
            readOnly = 302,
            executeOnly = 303,
            none = 304
        };

        static void initializeStorage();
        void *operator new(size_t size,void *pPtr);
        static void releaseStorage();

        object() {}

        object(job *,char c);
        object(job *,char *pszContents);
        object(job *,char *pszContents,objectType theType,valueType theValueType,valueClassification vcf);
        object(job *,char *pszContents,objectType theType,valueType theValueType,valueClassification vcf,executableAttribute ea);

        object(job *,char *pStart,char *pEnd);
        object(job *,char *pStart,char *pEnd,objectType theType,valueType theValueType,valueClassification vcf);

        object(job *,objectType,valueType,valueClassification);
        object(job *,BYTE value);
        object(job *,long value);
        object(job *,double value);

        virtual void put(long index,BYTE v);
        virtual BYTE get(long index);

        virtual void putElement(long index,object *pElement);
        virtual object *getElement(long index);

        virtual void execute();
        virtual char *ToString();

        virtual char *Name(char *pszNewName = NULL);

        virtual char *Contents(char *pszContents = NULL);

        char *TypeName();
        char *ValueTypeName();
        long IntValue(long value = -LONG_MAX);
        double DoubleValue(double value = -DBL_MAX);
        POINT_TYPE PointValue();

        float FloatValue(float value = -FLT_MAX);
        double Value();

        void SetExecuteOnly(bool v) { isExecuteOnly = v; };
        bool IsExecuteOnly() { return isExecuteOnly; };

        enum objectType ObjectType() { return theObjectType; };
        enum valueType ValueType( enum valueType vt = invalidValueType ) { if ( vt != invalidValueType ) theValueType = vt; return theValueType; };

        object *ContainingDictionary() { return pContainingDictionary; }

        static void *pHeap;
        static void *pCurrentHeap;
        static void *pNextHeap;
        static size_t currentlyAllocatedHeap;

    protected:

        job *pJob{NULL};

        objectType theObjectType{objectTypeUnspecified};
        valueType theValueType{valueTypeUnspecified};
        executableAttribute theExecutableAttribute{nonExecutable};
        accessAttribute theAccessAttribute{none};

        virtual ~object();

    private:

        object(job *,char charVal,long longVal,double realVal,char *pStart,char *pEnd,objectType,valueType,valueClassification,executableAttribute,accessAttribute);

        void parseValue(objectType,valueType);

        object *pContainingDictionary{NULL};

        char *pszContents{NULL};
        char *pszName{NULL};

        union {
            long intValue{-LONG_MAX};
            double doubleValue;
        };

        bool isExecuteOnly{false};

        friend class job;
        friend class dictionary;

    };
