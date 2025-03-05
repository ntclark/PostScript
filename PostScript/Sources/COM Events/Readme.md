# PostScript COM Event Interfaces

## COM is a truly beautiful thing

If this paraticular PostScript interpreter tool is going to be useful to you for your *specific* needs, then you'll need to provide some
mechanism to allow this tool the ability to let you know when those things happen.

You need to implement an event interface.

To be clear, *your* software "implements" an interface, that is you write the code that follows a precise definition in terms of calling convention, parameters, and order of functions.
*Then*, you provide a pointer **to** that interface to *this* object, and this object will call the appropriate functions in the interface with the specific parameters at the proper time.

You may already be familiar with all of that, though there will be a step by step later on.

## Limited functionality at this point

There is only one event interface defined in the system at this point as follows:

`
    IPostScriptEvents interface {
        HRESULT RenderChar(POINT *pPoint,char c);
        HRESULT RenderString(POINT *pPoint,char *pszString);
    }
`