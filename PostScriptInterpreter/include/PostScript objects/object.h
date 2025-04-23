/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the “Software”), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

#pragma once

#include <windows.h>
#include <float.h>

#include "pointType.h"

#define OBJECT_HEAP_SIZE   2 * 104857600

#define POINT_TYPE_NAN std::numeric_limits<FLOAT>::quiet_NaN()
#define POINT_TYPE_NAN_POINT {POINT_TYPE_NAN,POINT_TYPE_NAN}

#define OBJECT_POINT_TYPE_VALUE FloatValue()
#define OBJECT_SET_POINT_TYPE_VALUE(v) FloatValue(v)

    class job;

    class object {
    public:

        enum objectType {
            atom = 0,
            procedure = 1,
            dictionaryObject = 2,
            structureSpec = 3,
            directExecutable = 4,
            objTypeMatrix = 5,
            objTypeArray = 6,
            packedarray = 7,
            number = 8,
            logical = 9,
            mark = 10,
            null = 11,
            objTypeSave = 12,
            pattern = 13,
            colorSpace = 14,
            font = 15,
            name = 16,
            literal = 17,
            resource = 18,
            file = 19,
            filter = 20,
            dictionaryEntryObject = 21,
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
            binaryString = 104,
            character = 105,
            integer = 106,
            real = 107,
            radix = 108,
            trueOrFalse = 109,
            arrayMark = 110,
            dictionaryMark = 111,
            procedureMark = 112,
            executableProcedure = 113,
            executableOperator = 114
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

        object() {}

        object(job *,char c);
        object(job *,char *pszContents);
        object(job *,char *pszContents,objectType theType,valueType theValueType,valueClassification vcf);
        object(job *,char *pszContents,objectType theType,valueType theValueType,valueClassification vcf,executableAttribute ea);

        object(job *,char *pStart,char *pEnd);
        object(job *,char *pStart,char *pEnd,objectType theType,valueType theValueType,valueClassification vcf);

        object(job *,objectType,valueType,valueClassification);
        object(job *,BYTE value);
        object(job *,unsigned short value);
        object(job *,long value);
        object(job *,double value);

        object(object &) = delete;
        object(object &&) = delete;

        void *operator new(size_t size,void *pPtr);
        void *allocate(size_t size);

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
        enum valueType ValueType( enum valueType vt = invalidValueType ) 
            { if ( vt != invalidValueType ) theValueType = vt; return theValueType; };

        object *ContainingDictionary() { return pContainingDictionary; }

        job *Job() { return pJob; }

    protected:

        job *pJob{NULL};

        objectType theObjectType{objectTypeUnspecified};
        valueType theValueType{valueTypeUnspecified};
        executableAttribute theExecutableAttribute{nonExecutable};
        accessAttribute theAccessAttribute{none};

    private:

        object(job *,char charVal,long longVal,double realVal,char *pStart,char *pEnd,
                 objectType,valueType,valueClassification,executableAttribute,accessAttribute);

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
