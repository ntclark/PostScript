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

    // IConnectionPointContainer

    struct _IConnectionPointContainer : public IConnectionPointContainer {
    public:

        _IConnectionPointContainer(Renderer *pp) : pParent(pp) {};
        ~_IConnectionPointContainer() {};

    STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
    STDMETHOD_ (ULONG, AddRef)();
    STDMETHOD_ (ULONG, Release)();

    STDMETHOD(FindConnectionPoint)(REFIID riid,IConnectionPoint **);
    STDMETHOD(EnumConnectionPoints)(IEnumConnectionPoints **);

    void fire_ErrorNotification(char *pszError);
    void fire_StatusNotification(char *pszStatus);
    void fire_LogNotification(char *pszLog);
    void fire_Clear();

    private:

        uint32_t refCount{1};
        Renderer *pParent{NULL};

    } *pIConnectionPointContainer{NULL};

    // IConnectionPoint

    struct _IConnectionPoint : IConnectionPoint {
    public:

        _IConnectionPoint(Renderer *pp);
        ~_IConnectionPoint();

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

        STDMETHOD (GetConnectionInterface)(IID *);
        STDMETHOD (GetConnectionPointContainer)(IConnectionPointContainer **ppCPC);
        STDMETHOD (Advise)(IUnknown *pUnk,DWORD *pdwCookie);
        STDMETHOD (Unadvise)(DWORD);
        STDMETHOD (EnumConnections)(IEnumConnections **ppEnum);

        int CountConnections() { return countLiveConnections; }

    private:

        int getSlot();
        int findSlot(DWORD dwCookie);

        Renderer *pParent{NULL};
        DWORD nextCookie{0L};
        int countConnections{0};
        int countLiveConnections{0};

        uint32_t refCount{1};
        CONNECTDATA *connections{NULL};

    } *pIConnectionPoint{NULL};

    // IEnumConnectionPoints

    struct _IEnumConnectionPoints : IEnumConnectionPoints {
    public:

        _IEnumConnectionPoints(Renderer *pp,_IConnectionPoint **cp,int connectionPointCount);
        ~_IEnumConnectionPoints();

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

        STDMETHOD (Next)(ULONG cConnections,IConnectionPoint **rgpcn,ULONG *pcFetched);
        STDMETHOD (Skip)(ULONG cConnections);
        STDMETHOD (Reset)();
        STDMETHOD (Clone)(IEnumConnectionPoints **);

    private:

    int cpCount{0};
    int enumeratorIndex{0};
    Renderer *pParent{NULL};
    _IConnectionPoint **connectionPoints{NULL};

    } *pIEnumConnectionPoints{NULL};

    // IEnumerateConnections

    struct _IEnumerateConnections : public IEnumConnections {
    public:

    _IEnumerateConnections(IUnknown* pParentUnknown,ULONG cConnections,CONNECTDATA* paConnections,ULONG initialIndex);
    ~_IEnumerateConnections();

        STDMETHODIMP QueryInterface(REFIID, void **);
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();
        STDMETHODIMP Next(ULONG, CONNECTDATA*, ULONG*);
        STDMETHODIMP Skip(ULONG);
        STDMETHODIMP Reset();
        STDMETHODIMP Clone(IEnumConnections**);

    private:

    ULONG refCount{0L};
    IUnknown *pParentUnknown{NULL};
    ULONG enumeratorIndex{0L};
    ULONG countConnections{0L};
    CONNECTDATA *connections{NULL};

    } *pIEnumerateConnections{NULL};

