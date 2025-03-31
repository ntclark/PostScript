
#pragma once

#include <string>

#include "PostScriptInterpreter_i.h"

#include "Renderer_i.h"

#include "parsing.h"
#include "comment.h"
#include "error.h"

#include "PostScript objects.h"

#include "Stacks/objectStack.h"
#include "Stacks/dictionaryStack.h"
#include "Stacks/psTransformsStack.h"

    class job {
    public:

        job(char *pszFileName,HWND hwndSurface,IPostScriptTakeText *pIPostScriptTakeText);
        job() : job(NULL,NULL,NULL) {}
        ~job();

        void *CurrentObjectHeap();

        long parseJob(bool useThread = true);

        long execute(char *);
        long executeObject();
        long executeProcedure(procedure *);

        void parseProcedure(procedure *,char *pStart,char **ppEnd);
        void bindProcedure(procedure *pProcedure);
        void parseProcedureString(char *pStart,char **ppEnd);

        void RequestQuit() { quitRequested = true; }

        long currentInputFileOffset() { return (long)(currentInput() - pStart); }
        char *currentInput() { ADVANCE_THRU_WHITE_SPACE(pNext); return pNext; }
        void setCurrentInput(char *pInput) { pNext = pInput; }

        object* top() { 
            if ( 0 == operandStack.size() )
                return NULL;
            return operandStack.top(); 
        }
        object* pop() { return operandStack.pop(); }
        void push(object *pObject) { operandStack.push(pObject); }

        boolean deleteObject(object *pObject);
        void deleteNonContainedObject(object *pObj);

        void incrementCount() { countObjects++; }
        void decrementCount() { --countObjects; }

        graphicsState *currentGS();

#include "job_operators.h"

#include "job_pdfOperators.h"

        static void *pHeap;
        static void *pCurrentHeap;
        static void *pNextHeap;
        static size_t currentlyAllocatedHeap;

        static IRenderer *pIRenderer;

        static IGraphicElements *pIGraphicElements_External;
        static IGraphicParameters *pIGraphicParameters_External;

   private:

        //job();

        void resolve();
        object *resolve(char *pszObjectName);
        bool seekDefinition();

        dictionary *containingDictionary(object *pObj);

        long getPageCount(char *pszFileName);

        void parse(char *pszBeginDelimiter,char *pszEndDelimiter,char *pStart,char **ppEnd);
        void parseBinary(char *pszEndDelimiter,char *pStart,char **ppEnd);
        char *parseObject(char *pStart,char **pEnd);

        void parseFile(char *pszFileName);

        void parseStructureSpec(char *pStart,char **ppEnd);
        void parseComment(char *pStart,char **ppEnd);
        void parseString(char *pStart,char **ppEnd);
        void parseHexString(char *pStart,char **ppEnd);
        void parseHex85String(char *pStart,char **ppEnd);
        void parseLiteralName(char *apStart,char **ppEnd);

        HBITMAP parsePage(long pageNumber);

        char *pStart{NULL};
        char *pNext{NULL};

        std::map<size_t,void (__thiscall job::*)(char *pStart,char **ppEnd)> tokenProcedures;
        std::map<size_t,char *> antiDelimiters;
        std::map<size_t,name *> validNames;

        std::list<comment *> commentStack;
        std::map<size_t,object *> literalNames;
        std::list<structureSpec *> structureSpecs;

        std::map<object::objectType,std::map<object::valueType,object *>> nameTypeMap;

        objectStack operandStack;
        dictionaryStack dictionaryStack;

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

        object *pLanguageLevel{NULL};

        array *pStandardEncoding{NULL};
        array *pISOLatin1Encoding{NULL};

        colorSpace *pDefaultColorSpace{NULL};

        char szPostScriptSourceFile[MAX_PATH];
        FILE *pPostScriptSourceFile{NULL};

        long saveCount{0L};
        long inputLineNumber{0L};
        long procedureDefinitionLevel{0L};
        long countObjects{0L};
        long countPages{0L};

        char *collectionDelimiterPeek(char *,char **);
        char *delimiterPeek(char *,char **);
        char *token();

        bool isGlobalVM{false};
        bool hasExited{false};
        bool quitRequested{false};

        char *pStorage{NULL},*pEnd{NULL};

        HWND hwndSurface{NULL};
        HANDLE hsemIsInitialized{INVALID_HANDLE_VALUE};

        HBITMAP hbmSink{NULL};

        IPostScriptTakeText *pIPostScriptTakeText{NULL};

        static unsigned int __stdcall executeInitialization(void *);
        static unsigned int __stdcall executeThread(void *);

        friend class graphicsState;
        friend class graphicsStateStack;
        friend class PStoPDF;
        friend class dictionary;
        friend class font;
        friend class name;
        friend class file;
   };
