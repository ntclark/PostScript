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

#include "Renderer.h"


    HRESULT Renderer::SaveState() {
    return pGraphicsStateManager -> Save();
    }


    HRESULT Renderer::RestoreState() {
    return pGraphicsStateManager -> Restore();
    }


    Renderer::GraphicsStateManager::~GraphicsStateManager() {
    clear();
    return;
    }


    HRESULT Renderer::GraphicsStateManager::Save() {
    parametersStack.push(new Renderer::GraphicParameters::values(*parametersStack.top()));
    CoCreateGuid(&parametersStack.top() -> valuesId);
    return S_OK;
    }


    HRESULT Renderer::GraphicsStateManager::Restore() {
    if ( 1 == parametersStack.size() ) {
        sprintf_s<1024>(Renderer::szErrorMessage,"Error: There was an underflow in the GraphicsState Save/Restore. More Restores were made than Saves");
        pParent -> pIConnectionPointContainer -> fire_ErrorNotification(Renderer::szLogMessage);
        return E_FAIL;
    }
    Renderer::GraphicParameters::values *pValues = parametersStack.top();
    parametersStack.pop();
    delete pValues;
    return S_OK;
    }