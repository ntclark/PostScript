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

#include "job.h"

    // Note the very important convention: upon entry to the parser functions, the pStart pointer
    // is positioned just past the delimiter that initiated the parsing.
    // For example, "<< dictKey dictElement >>" as input, upon entry the current string is
    // " dictKey dictElement >>"
    // Therefore, when calling the delimiter peek methods, use the value of "pNext" as the
    // starting point for continued parsing.

    void job::parseCollectionString(char *pszDelimiterBegin,char *pszDelimiterEnd,char *pStart,char **ppEnd) {

    // This function merely defines the beginning and end of the collection string,
    // it does not create any objects or manipulate the operand stack.

    char *p = pStart;
    char *pNext = p;
    long depth = 1L;

    boolean twoCharDelimiter = ! ( '\0' == pszDelimiterBegin[1] );

    do {

        ADVANCE_THRU_WHITE_SPACE(p)

        if ( '\0' == *p )
            break;

        if ( 0x0A == p[0] || 0x0D == p[0] ) {
            while ( 0x0A == *p || 0x0D == *p ) 
                p++;
            if ( 0 < inputLineNumberSize ) {
                sscanf(p,szLNFormat,&inputLineNumber);
                p += inputLineNumberSize;
            }
            continue;
        }

        char *pCollectionDelimiter = collectionDelimiterPeek(p,&pNext);

        if ( ! ( NULL == pCollectionDelimiter ) && pszDelimiterBegin[0] == *pCollectionDelimiter && ( ( ! twoCharDelimiter ) || pszDelimiterBegin[1] == *pCollectionDelimiter ) ) {
            depth++;
            p = pNext;
            continue;
        }

        if ( ! ( NULL == pCollectionDelimiter ) && pszDelimiterEnd[0] == *pCollectionDelimiter && ( ( ! twoCharDelimiter ) || pszDelimiterEnd[1] == *pCollectionDelimiter ) ) {
            depth--;
            if ( 0 == depth ) {
                *ppEnd = pNext;
                return;
            }
            p = pNext;
            continue;
        }

        char *pStringDelimiter = delimiterPeek(p,&pNext);

        if ( ! ( NULL == pStringDelimiter ) && STRING_DELIMITER_BEGIN[0] == *pStringDelimiter ) {
            parse(STRING_DELIMITER_BEGIN,STRING_DELIMITER_END,pNext,&pNext);
            p = pNext + 1;
            continue;
        }

        p++;

    } while ( ! ( '\0' == p ) );

    *ppEnd = NULL;

    return;
    }


    void job::parseProcedure(char *pStart,char **ppEnd) {

    parseCollectionString(PROC_DELIMITER_BEGIN,PROC_DELIMITER_END,pStart,ppEnd);

    object *pName = peekPrior();
    char *pszName = NULL;
    if ( ! ( NULL == pName ) && object::objectType::literal == pName -> ObjectType() )
        pszName = pName -> Name();

    procedure *pProcedure = new (CurrentObjectHeap()) procedure(this,pszName,pStart,*ppEnd);

    push(pProcedure);

    char *p = pStart;
    char *pNext = p;

    do {

        if ( '\0' == *p )
            break;

        ADVANCE_THRU_WHITE_SPACE(p)

        if ( 0x0A == p[0] || 0x0D == p[0] ) {
            while ( 0x0A == *p || 0x0D == *p ) 
                p++;
            if ( 0 < inputLineNumberSize ) {
                sscanf(p,szLNFormat,&inputLineNumber);
                p += inputLineNumberSize;
            }
            continue;
        }

        char *pCollectionDelimiter = collectionDelimiterPeek(p,&pNext);

        char *pDelimiter = NULL;

        if ( NULL == pCollectionDelimiter )
            pDelimiter = (char *)delimiterPeek(p,&pNext);

        if ( ! ( NULL == pDelimiter ) ) {
            (this ->* tokenParsers[std::hash<std::string>()((char *)pDelimiter)])(pNext,&pNext);
            if ( ! ( DSC_DELIMITER[0] == *pDelimiter ) && ! ( COMMENT_DELIMITER[0] == *pDelimiter ) )
                pProcedure -> insert(pop());
            p = pNext;
            continue;
        }

        if ( ! ( NULL == pCollectionDelimiter ) && PROC_DELIMITER_BEGIN[0] == *pCollectionDelimiter ) {
            (this ->* collectionParsers[std::hash<std::string>()((char *)pCollectionDelimiter)])(pNext,&pNext);
            procedure *pInnerProcedure = reinterpret_cast<procedure *>(pop());
            pInnerProcedure -> pContainingProcedure = pProcedure;
            pProcedure -> insert(pInnerProcedure);
            p = pNext;
            continue;
        }

        if ( ! ( NULL == pCollectionDelimiter ) && PROC_DELIMITER_END[0] == *pCollectionDelimiter ) {
            if ( ! ( NULL == ppEnd ) )
                *ppEnd = pNext;
            return;
        }

        object *po = NULL;

        if ( ! ( NULL == pCollectionDelimiter ) ) {
            //
            //NTC: 03-11-2026. Parsing the procedure is slightly different than parsing
            // a dictionary.
            // Consider { [ 1 2 3 ] } as the procedure. This results in "[" "1" "2" "3" "]" as the elements
            // of the procedure.
            // I think this is working okay, when the procedure executes, the array is created
            // properly.
            // However, with a dictionary, << /Array [ 1 2 3 ] >> needs to resolve the second
            // value to an array, not to 5 components of the array. In essence, with a dictionary,
            // all composite objects need to be defined as such, whereas in a procedure,
            // there may be a reason they should be resolved at the point of use
            po = new (CurrentObjectHeap()) object(this,pCollectionDelimiter);
            push(po);
            resolve();
            po = pop();
            p = pNext;
        } else 
            po = new (CurrentObjectHeap()) object(this,parseObject(p,&p));

        po -> HandyIdentifier((long)pProcedure -> entries.size());

        pProcedure -> insert(po);

    } while ( ! ( '\0' == p ) );

    if ( ! ( NULL == ppEnd ) )
        *ppEnd = NULL;

    return;
    }


    void job::parseDictionary(char *pStart,char **ppEnd) {

    parseCollectionString(DICTIONARY_DELIMITER_BEGIN,DICTIONARY_DELIMITER_END,pStart,ppEnd);

    object *pName = peekPrior();
    char *pszName = NULL;
    if ( ! ( NULL == pName ) && object::objectType::literal == pName -> ObjectType() )
        pszName = pName -> Name();

    dictionary *pDictionary = new (CurrentObjectHeap()) dictionary(this,pszName,pStart,*ppEnd);

    push(pDictionary);

    char *p = pStart;
    char *pNext = p;


/*

this does not work because it's not every other element in the list becomes a value, there may 
be a procedure within the definition - or in any case, something that executes to create the value.

Example << /key 7 0 R >> where R is a procedure that takes two operands and leaves 1 on the stack

*/

    object *pKey = NULL;

    do {

        if ( '\0' == *p )
            break;

        ADVANCE_THRU_WHITE_SPACE(p)

        if ( 0x0A == p[0] || 0x0D == p[0] ) {
            while ( 0x0A == *p || 0x0D == *p ) 
                p++;
            if ( 0 < inputLineNumberSize ) {
                sscanf(p,szLNFormat,&inputLineNumber);
                p += inputLineNumberSize;
            }
            continue;
        }

        char *pCollectionDelimiter = collectionDelimiterPeek(p,&pNext);
        char *pDelimiter = NULL;

        if ( NULL == pCollectionDelimiter )
            pDelimiter = (char *)delimiterPeek(p,&pNext);

        if ( ! ( NULL == pDelimiter ) ) {
            (this ->* tokenParsers[std::hash<std::string>()((char *)pDelimiter)])(pNext,&pNext);
            if ( DSC_DELIMITER[0] == *pDelimiter || COMMENT_DELIMITER[0] == *pDelimiter ) {
                p = pNext;
                continue;
            }
            if ( NULL == pKey )
                pKey = pop();
            else {
                pDictionary -> put(pKey -> Name(),pop());
                pKey = NULL;
            }
            p = pNext;
            continue;
        }

        if ( ! ( NULL == pCollectionDelimiter ) && DICTIONARY_DELIMITER_BEGIN[0] == *pCollectionDelimiter && DICTIONARY_DELIMITER_BEGIN[1] == *(pCollectionDelimiter + 1) ) {
            (this ->* collectionParsers[std::hash<std::string>()((char *)pCollectionDelimiter)])(pNext,&pNext);
            dictionary *pInnerDictionary = reinterpret_cast<dictionary *>(pop());
            pInnerDictionary -> pContainingDictionary = pDictionary;
            if ( NULL == pKey )
                pKey = pInnerDictionary;
            else {
                pDictionary -> put(pKey -> Name(),pInnerDictionary);
                pKey = NULL;
            }
            p = pNext;
            continue;
        }

        if ( ! ( NULL == pCollectionDelimiter ) && DICTIONARY_DELIMITER_END[0] == *pCollectionDelimiter && DICTIONARY_DELIMITER_END[1] == *(pCollectionDelimiter + 1)) {
            if ( ! ( NULL == ppEnd ) )
                *ppEnd = pNext;
            return;
        }

        object *po = NULL;

        if ( ! ( NULL == pCollectionDelimiter ) ) {
            // See the note  in parseProcedure, in this case, the collection
            // needs to be fully resolved before entry in the dictionary
            //
            (this ->* collectionParsers[std::hash<std::string>()((char *)pCollectionDelimiter)])(pNext,&pNext);
        } else {
            po = new (CurrentObjectHeap()) object(this,parseObject(p,&pNext));
            push(po);
            //resolve();
            executeObject();
        }

        po = pop();

        p = pNext;

        po -> HandyIdentifier((long)pDictionary -> entries.size());

        if ( NULL == pKey )
            pKey = po;
        else {
            pDictionary -> put(pKey -> Name(),po);
            pKey = NULL;
        }

    } while ( ! ( '\0' == p ) );

    if ( ! ( NULL == ppEnd ) )
        *ppEnd = NULL;

    return;
    }


    void job::parseArray(char *pStart,char **ppEnd) {

    parseCollectionString(ARRAY_DELIMITER_BEGIN,ARRAY_DELIMITER_END,pStart,ppEnd);

    object *pName = peekPrior();
    char *pszName = NULL;
    if ( ! ( NULL == pName ) && object::objectType::literal == pName -> ObjectType() )
        pszName = pName -> Name();

    array *pArray = new (CurrentObjectHeap()) array(this,pszName,pStart,*ppEnd);

    push(pArray);

    char *p = pStart;
    char *pNext = p;

    object *pKey = NULL;

    // This loop is substantially the same for every type of collection object.
    // I would like to consolodate it in the future.
    // Need some kind of lambda maybe to populate the specific collection

    do {

        if ( '\0' == *p )
            break;

        ADVANCE_THRU_WHITE_SPACE(p)

        if ( 0x0A == p[0] || 0x0D == p[0] ) {
            while ( 0x0A == *p || 0x0D == *p ) 
                p++;
            if ( 0 < inputLineNumberSize ) {
                sscanf(p,szLNFormat,&inputLineNumber);
                p += inputLineNumberSize;
            }
            continue;
        }

        char *pCollectionDelimiter = collectionDelimiterPeek(p,&pNext);
        char *pDelimiter = NULL;

        if ( NULL == pCollectionDelimiter )
            pDelimiter = (char *)delimiterPeek(p,&pNext);

        if ( ! ( NULL == pDelimiter ) ) {
            (this ->* tokenParsers[std::hash<std::string>()((char *)pDelimiter)])(pNext,&pNext);
            if ( DSC_DELIMITER[0] == *pDelimiter || COMMENT_DELIMITER[0] == *pDelimiter ) {
                p = pNext;
                continue;
            }
            pArray -> putElement(pArray -> size(),pop());
            p = pNext;
            continue;
        }

        if ( ! ( NULL == pCollectionDelimiter ) && ARRAY_DELIMITER_BEGIN[0] == *pCollectionDelimiter ) {
            (this ->* collectionParsers[std::hash<std::string>()((char *)pCollectionDelimiter)])(pNext,&pNext);
            array *pInnerArray = reinterpret_cast<array *>(pop());
            pArray -> putElement(pArray -> size(),pInnerArray);
            p = pNext;
            continue;
        }

        if ( ! ( NULL == pCollectionDelimiter ) && ARRAY_DELIMITER_END[0] == *pCollectionDelimiter ) {
            if ( ! ( NULL == ppEnd ) )
                *ppEnd = pNext;
            return;
        }

        object *po = NULL;

        if ( ! ( NULL == pCollectionDelimiter ) )
            (this ->* collectionParsers[std::hash<std::string>()((char *)pCollectionDelimiter)])(pNext,&pNext);
        else {
            po = new (CurrentObjectHeap()) object(this,parseObject(p,&pNext));
            push(po);
            resolve();
        }

        po = pop();

        p = pNext;

        po -> HandyIdentifier((long)pArray -> entries.size());

        pArray -> putElement(pArray -> size(),po);

    } while ( ! ( '\0' == p ) );

    if ( ! ( NULL == ppEnd ) )
        *ppEnd = NULL;

    return;
    }