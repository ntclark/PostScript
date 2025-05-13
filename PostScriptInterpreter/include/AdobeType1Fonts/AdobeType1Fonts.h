#pragma once

#include <list>
#include <map>

#include "Renderer_i.h"

#include "AdobeType1Fonts/type1Glyph.h"

class job;
class font;
class dictionary;
class object;
class objectStack;

    static class AdobeType1Fonts {
    public:

        AdobeType1Fonts() {};

        static void Initialize(job *,IRenderer *,IGraphicElements *);
        static void Shutdown();

        static void executeEExec(class file *pFileObject);

        static void drawGlyph(font *pCurrentFont,uint16_t bGlyph,
                                XFORM *pPSXform,POINTF *pStartPoint,POINTF *pEndPoint);

        static type1Glyph *pType1Glyph;

    private:

        struct type1Token {
            enum tokenKind {
                isNumber = 0,
                isCommand = 1,
                isUnknown = 99
            };
            type1Token(AdobeType1Fonts::type1Token::tokenKind t,int v) {
                theKind = t;
                value = v;
            }
            type1Token(AdobeType1Fonts::type1Token::tokenKind t,char *p) {
                theKind = t;
                pszCommand = p;
            }
            enum tokenKind theKind{isUnknown};
            char *pszCommand{NULL};
            int value{0};
        };

        static job *pJob;
        static dictionary *pOperators;
        static IRenderer *pIRenderer;
        static IGraphicElements *pIGraphicElements;

        static uint32_t decryptType1Data(uint16_t initialKey,uint8_t *pbData,uint32_t cbData,uint8_t countRandomBytes,uint8_t **ppResults);
        static void decryptType1CharString(uint8_t *pbData,uint32_t cbData,uint8_t countRandomBytes,std::list<type1Token *> *pTokenList);

        static std::map<uint8_t,char *> type1CharStringCommands;
        static std::map<uint8_t,char *> type1CharStringSubCommands;

        static objectStack *pOperandStack;
        static object *pZeroValue;

        static void processTokens(std::list<type1Token *> *pTokens);

        void blend();
        void callgsubr();
        void callsubr();
        void closepath();
        void cntrmask();
        void endchar();
        void error();
        void escape();
        void hhcurveto();
        void hintmask();
        void hlineto();
        void hmoveto();
        void hsbw();
        void hstem();
        void hstemhm();
        void hvcurveto();
        void rcurveline();
        void operatorReturn();
        void rlinecurve();
        void rlineto();
        void rmoveto();
        void rrcurveto();
        void vhcurveto();
        void vlineto();
        void vmoveto();
        void vstem();
        void vstemhm();
        void vvcurveto();

        void dotsection();
        void vstem3();
        void hstem3();
        void and();
        void or();
        void not();
        void seac();
        void sbw();
        void store();
        void abs();
        void add();
        void sub();
        void div();
        void load();
        void neg();
        void eq();
        void callothersubr();
        void pop();
        void drop();
        void put();
        void get();
        void ifelse();
        void random();
        void mul();
        void sqrt();
        void dup();
        void exch();
        void index();
        void roll();
        void setcurrentpoint();
        void hflex();
        void flex();
        void hflex1();
        void flex1();

        void curveto(FLOAT dx1,FLOAT dy1,FLOAT dx2,FLOAT dy2,FLOAT dx3,FLOAT dy3);

        void invalidCommand();
        void type2Command();

    } adobeType1Fonts;
