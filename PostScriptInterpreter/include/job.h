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
#pragma warning(disable: 6031)

#include <string>
#include <deque>
#include <stack>

#include "PostScriptInterpreter_i.h"
#include "Renderer_i.h"

#include "parsing.h"
#include "error.h"

#include "AdobeType1Fonts/AdobeType1Fonts.h"

#include "PostScript objects.h"

#include "Stacks/objectStack.h"
#include "Stacks/dictionaryStack.h"
#include "Stacks/psTransformsStack.h"

    class job {
    public:

        job(char *pszFileName,HWND hwndSurface,long inputLineNumberSize);
        job() : job(NULL,NULL,0) {}

        ~job();

        void *CurrentObjectHeap();

        long parseJob(bool useThread = true);

        long execute(char *pszBegin,char *pszEnd,char *pszFileName);
        long executeObject();
        void executeProcedure(procedure *);

        void parseProcedure(procedure *,char *pStart,char **ppEnd);
        void bindProcedure(procedure *pProcedure);
        void parseProcedureString(char *pStart,char **ppEnd);

        void RequestQuit() { executionStack.top() -> quitRequested = true; }

        void currentInputFlushSpace() {
            while ( ' ' == *(executionStack.top() -> pNext) )
                executionStack.top() -> pNext++;
        }
        char *currentInput() { 
            if ( executionStack.top() -> pNext == executionStack.top() -> pEnd )
                return NULL;
            return executionStack.top() -> pNext;
        }
        void setCurrentInput(char *pInput) { 
            if ( pInput < executionStack.top() -> pEnd )
                executionStack.top() -> pNext = pInput;
            else
                executionStack.top() -> pNext = executionStack.top() -> pEnd;
        }

        object* top() { 
            if ( 0 == pOperandStack -> size() )
                return NULL;
            return pOperandStack -> top();
        }
        object* pop() { return pOperandStack -> pop(); }
        object* peekPrior() { return pOperandStack -> peekPrior(); }
        void push(object *pObject) { pOperandStack -> push(pObject); }

        graphicsState *currentGS();

        dictionary *SystemDict() { return pSystemDict; }
        uint32_t ExecutionStackSize() { return (uint32_t)executionStack.size(); }
        char *CurrentSourceFileName() { return szPostScriptSourceFile; }

        bool seekDefinition();

#include "job_operators.h"

        static void *pHeap;
        static void *pCurrentHeap;
        static void *pNextHeap;
        static size_t currentlyAllocatedHeap;

   private:

        static DWORD dwNotificationCookie;

        void initializeHeap();
        void releaseHeap();

        void resolve();
        object *resolve(char *pszObjectName);

        dictionary *containingDictionary(object *pObj);

        long getPageCount(char *pszFileName);

        void parse(char *pszBeginDelimiter,char *pszEndDelimiter,char *pStart,char **ppEnd);
        void parseBinary(char *pszEndDelimiter,char *pStart,char **ppEnd);
        char *parseObject(char *pStart,char **pEnd);
        void parseDSC(char *pStart,char **ppEnd);
        void parseComment(char *pStart,char **ppEnd);
        void parseString(char *pStart,char **ppEnd);
        void parseHexString(char *pStart,char **ppEnd);
        void parseHex85String(char *pStart,char **ppEnd);
        void parseLiteralName(char *apStart,char **ppEnd);
        void parseResolveNowString(char *pStart,char **ppEnd);

        struct executionLevel {
            executionLevel(char *pBegin,char *pe,char *pszFileName) : 
                pStart(pBegin),pNext(pBegin),pEnd(pe) {
                if ( NULL == pszFileName )
                    strcpy(szCurrentFile,"<unspecified>");
                else
                    strncpy(szCurrentFile,pszFileName,MAX_PATH);
            }
            char *pStart{NULL};
            char *pNext{NULL};
            char *pEnd{NULL};
            boolean quitRequested{false};
            boolean fileClosed{false};
            char szCurrentFile[MAX_PATH]{0};
        };

        std::stack<executionLevel *> executionStack;

        static executionLevel *pRootExecutionLevel;

        std::map<size_t,void (__thiscall job::*)(char *pStart,char **ppEnd)> tokenProcedures;
        std::map<size_t,char *> antiDelimiters;
        std::map<size_t,name *,std::less<size_t>,containerAllocator<name *>> *pValidNames{NULL};

        std::list<comment *,containerAllocator<comment *>> *pComments{NULL};
        std::list<dscItem *,containerAllocator<dscItem *>> *pDSCItems{NULL};

        std::map<object::objectType,std::map<object::valueType,object *>> nameTypeMap;

        objectStack *pOperandStack{NULL};
        dictionaryStack *pDictionaryStack{NULL};

        std::list<resource *> resourceList;
        std::list<procedure *> procedureList;

        graphicsState *pGraphicsState{NULL};

        dictionary *pGlobalDict{NULL};
        dictionary *pSystemDict{NULL};
        dictionary *pUserDict{NULL};
        dictionary *pErrorDict{NULL};
        dictionary *pStatusDict{NULL};
        dictionary *pPdfDict{NULL};

        dictionary *pFontDirectory{NULL};

        dictionary *pGenericResourceImplementation{NULL};

        booleanObject *pTrueConstant{NULL};
        booleanObject *pFalseConstant{NULL};
        object *pNullConstant{NULL};
        object *pZeroConstant{NULL};
        object *pOneConstant{NULL};

        object *pFontTypeLiteral{NULL};
        object *pFontNameLiteral{NULL};
        object *pFontMatrixLiteral{NULL};
        object *pFontBoundingBoxLiteral{NULL};

        object *pEncodingArrayLiteral{NULL};
        object *pNotdefLiteral{NULL};
        object *pCharStringsLiteral{NULL};

        object *pGlyphDirectoryLiteral{NULL};
        object *pRestoreFontMatrixLiteral{NULL};
        object *pSfntsLiteral{NULL};
        object *pPageSizeLiteral{NULL};
        object *pBuildGlyphLiteral{NULL};
        object *pBuildCharLiteral{NULL};

        name *pStringType{NULL};
        name *pConstantStringType{NULL};
        name *pBinaryStringType{NULL};
        name *pArrayType{NULL};
        name *pPackedArrayType{NULL};
        name *pIntegerType{NULL};
        name *pRealType{NULL};
        name *pBooleanType{NULL};
        name *pDictType{NULL};
        name *pMarkType{NULL};
        name *pNullType{NULL};
        name *pSaveType{NULL};
        name *pFontType{NULL};
        name *pOperatorType{NULL};
        name *pNameType{NULL};
        name *pDSCItem{NULL};
        name *pComment{NULL};

        object *pLanguageLevel{NULL};

        array *pStandardEncoding{NULL};
        array *pISOLatin1Encoding{NULL};

        colorSpace *pDefaultColorSpace{NULL};

        char szPostScriptSourceFile[MAX_PATH];
        FILE *pPostScriptSourceFile{NULL};

        long saveCount{0L};
        long inputLineNumber{0L};
        long inputLineNumberSize{0L};
        long procedureDefinitionLevel{0L};
        long countPages{0L};

        char szLNFormat[16]{16*'\0'};

        char *collectionDelimiterPeek(char *,char **);
        char *delimiterPeek(char *,char **);

        bool isGlobalVM{false};
        bool hasExited{false};

        char *pStorage{NULL};
        char *pStorageEnd{NULL};

        HWND hwndSurface{NULL};
        HANDLE hsemIsInitialized{INVALID_HANDLE_VALUE};

        static unsigned int __stdcall executeInitialization(void *);
        static unsigned int __stdcall executeThread(void *);

        friend class graphicsState;
        friend class graphicsStateStack;
        friend class PostScriptInterpreter;
        friend class dictionary;
        friend class font;
        friend class name;
        friend class file;
   };
