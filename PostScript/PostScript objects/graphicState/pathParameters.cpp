
#include "job.h"

#include "pathParameters.h"

#include <d2d1.h>
#include <d2d1helper.h>
#include <wincodec.h>

    ID2D1Factory *pathParameters::pID2D1Factory = NULL;
    ID2D1SolidColorBrush *pathParameters::pID2D1SolidColorBrush = NULL;
    ID2D1DCRenderTarget *pathParameters::pID2D1DCRenderTarget = NULL;

    pathParameters::pathParameters() {
    return;
    }


    pathParameters::pathParameters(pathParameters &rhs) {
    pToUserSpace = new (pPStoPDF -> currentJob() -> CurrentObjectHeap()) matrix(pPStoPDF -> currentJob());
    pToUserSpace -> a(rhs.pToUserSpace -> a());
    pToUserSpace -> b(rhs.pToUserSpace -> b());
    pToUserSpace -> c(rhs.pToUserSpace -> c());
    pToUserSpace -> d(rhs.pToUserSpace -> d());
    pToUserSpace -> tx(rhs.pToUserSpace -> tx());
    pToUserSpace -> ty(rhs.pToUserSpace -> ty());
    }


    pathParameters::~pathParameters() {
    }


    void pathParameters::initialize() {
    closepath();
    pToUserSpace -> identity();
    return;
    }