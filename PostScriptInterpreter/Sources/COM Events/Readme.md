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

Remember that this is an interface provided by *you*. It can be written in almost any language, as long as the binding
between your system and this PostScript Interpreter COM object exists.

The typical usage of this interface is probably when you are looking at what text might be in the document printed. 
As each element of text is found in the PostScript language file, the system will pass that text and it's location (in units of points) in the document and you can do with it whatever you want.

## Note

My CursiVision product contains a PostScript print driver that can be configured to launch any process against the resulting PostScript language file. If that configured process is software that
you write - you can immediately start using PostScript parsing and the above event interface to build your document analyization system.

If you would like the installer for that Print driver, just ask at Nate@InVisioNateSW.com and I'll pass it along.


# Connecting up events

If you don't already know how, I'll describe here how you can integrate your events interface with the PostScript interpreter.

If you'd like to see an example, check out the [psDemo](../../../psDemo) project.

