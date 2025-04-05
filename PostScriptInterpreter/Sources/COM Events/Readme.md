# PostScript COM Event Interfaces

## COM is a truly beautiful thing

If this paraticular PostScript interpreter tool is going to be useful to you for your *specific* needs, then you'll need to provide some
mechanism to allow the tool the ability to let you know when certain things happen.

You need to implement an event interface.

To be clear, *your* software "implements" an interface. That is, you write the code that follows a precise definition in terms of 
calling convention, parameters, and order of functions. *Then*, you provide a pointer **to** that interface to the interpreter 
object, and the interpreter will call the appropriate functions in your interface with the specific parameters at the proper time.

You may already be familiar with all of that, though if it helps there will be a step by step later on.

## Limited functionality at this point

There is only one event interface defined in the system at this point as follows:

```
    IPostScriptInterpreterEvents interface {
        HRESULT RenderChar(POINT *pPoint,char c);
        HRESULT RenderString(POINT *pPoint,char *pszString);
    }
```

Remember that this is an interface provided by *you*. It can be written in almost any language, as long as the binding
between your system and this PostScript Interpreter COM object exists.

The typical usage of this interface is probably when you are looking at what text might be in a document. 
As each element of text is found in the PostScript language file, the system will pass that text and it's location 
(in units of points) in the document and you can do with it whatever you want.

Text from PostScript files may be represented in a couple of ways. Some text may be placed in the result a character at a time. 
In other words, the PostScript code contains a "moveto" command followed by the output of a single character, followed
by another "moveto" for the next character[^1], and so on. This would result in multiple calls to `RenderChar` in the events interface.

Other times an entire string, or parts of a string, might be placed in the document. In this case, a call to `RenderString` would occur.
The software in this repository has no control over how the print processor, or creator of the PostScript ('.ps) file chooses to represent the text.

It is also important to understand that some print jobs might use a Type3 font. This type of font does not have recognizable
"characters" that you would be able to actually "extract" because these "characters" are a series of PostScript compatible
graphics commands that actually draw the character when encountered.

And still another issue is that sometimes characters, really "glyphs", are given in a numeric form that the font mechanism might
lookup for the glyph definition, such as for what are called "composite glyphs" which are something like multiple characters 
overlayed at the same location. Consider umlaut characters, typical a lower case vowel with a horizontal colon above them.

Given these issues, one might consider creating an event interface that presented the client with a bitmap of the rendered
characters[^2]. Even more of a possibility would be to pass those bitmaps onto some OCR tool.

## Note

My CursiVision product contains a PostScript print driver that can be configured to launch any process against the resulting PostScript language file. 
If that configured process is software that you write - you can immediately start using PostScript parsing and the above event 
interface to build your document analyization system.

If you would like the installer for that Print driver, just ask at Nate@InVisioNateSW.com and I'll pass it along.

# Connecting up events

If you don't already know how, I'll describe here the steps you take to integrate your events interface with the PostScript interpreter.

If you'd like to see an example, check out the [psDemo](../../../psDemo) project, we'll use code from that example in this material.

### Ask if the intepreter supports events

After you get a pointer to any interface the intepreter provides, IPostScript, for exmaple, probably by calling CoCreateInstance, you then ask 
for an IConnectionPointContainer interface from that interface:

```
#include "PostScriptInterpreter_i.h"
#include "PostScriptInterpreter_i.c"

        // other stuff

    IPostScript *pIPostScript{NULL};

    HRESULT rc = CoCreateInstance(CLSID_PostScriptInterpreter,NULL,CLSCTX_ALL,
                    IID_IPostScriptInterpreter,reinterpret_cast<void **>(&pIPostScript));

    IConnectionPointContainer *pIConnectionPointContainer = NULL;

    rc = pIPostScript -> QueryInterface(IID_IConnectionPointContainer,
                    reinterpret_cast<void **>(&pIConnectionPointContainer));

```

You would have gotten an S_OK in the hr from the last statement.

### Okay then, now ask the connection point container if it supports this specific interface

So, now, with this last interface, you have the ability to ask the interpreter, "What outgoing (events or "source") interfaces
do you support anyway"? You must ask this for specific interfaces you want, you don't get a list back.

Note that I've used the '_i.h and '_i.c include files, there are many ways to do this but if you are going to 
actually be building the interpreter, you'll have these files availalable anyway. You can also use #importlib 
but I keep forgetting how to do that so won't be describing that strategy here.

So now, you need to ask the interpreter if it has a "connection point" for the specific interface of 
interest, that being IPostScriptInterpreterEvents. 

```
    IConnectionPoint *pIConnectionPoint = NULL;

    rc = pIConnectionPointContainer -> FindConnectionPoint(IID_IPostScriptInterpreterEvents,&pIConnectionPoint);

```

So, the interpreter will respond, "Of course I have that connection point", and place a pointer to it in the
pIConnectionPoint parameter.

### Since the source interface is supported, tell the interpreter you want to be a sink

All that is left to do now is let the interpreter know that you would like to receive calls on your implementation
at the approprite times. Therefore, you "Advise" the connection point that you are out there, waiting for 
just those notifications.

```
    DWORD dwCookie = 0L;
    IUnknown *pIUnknownSink = NULL;

    postScriptEvents.QueryInterface(IID_IUnknown,reinterpret_cast<void **>(&pIUnknownSink));

    rc = pIConnectionPoint -> Advise(pIUnknownSink,&dwCookie);
```

I usually pass a pointer to my object's IUnknown when I Advise a "source" object of my interest in being a "sink".

Mostly because I like the way it looks, and partially because my event interfaces may be what I call either
"tear off" interface, or in some cases NULL pointers that won't get populated until *during* MY QueryInterface, 
because I don't actually create them until and when they are actually going to be used. 
This supports that model, I don't create an instance of an events interface up front, only when I know they're being
used, which is obviously when some other entity asks for them.

When you are no longer interested in receiving the events, at system shutdown maybe ?, you should call
Unadvise on that IConnectionPointContainer you recieved. Passing the cookie you recieved tells the interpreter
exactly which pointer to the sink it should release.

```
    pIConnectionPoint -> Unadvise(dwCookie);

```

In fact, the interpreter won't release it's connection to the event interface until you unadvise your usage and
you definately don't want to get called on that interface if you've deleted it or are no longer prepared for it.

Finally, you should always release any other interface (example IConnectionPointContainer) when you're done 
with them.

### Another thing about events

It should be mentioned that your interface members are going to be called on the interpreter's thread, and there
is no guarantee at all what that thread is, you cannot dictate the threading of the intepreter. You can see what it is
using the debugger for example, but that *may* change on a case by case basis.

Also, you should know that the interpreter will wait for your return when it sends you an event. For this reason,
you should not do any lengthy computation then. You can create threads and synchronization objects if you want, but 
should avoid taking a lot of time, for example, don't communicate with the user and wait for an answer in 
an event handler.

[^1]: Concepts such as kerning and other text positioning or size have no relevance to this software. Those issues are taken care 
of upstream of any PostScript language file. That file contains all the information necessary to render the text in the
right place and size.

[^2]: Why not just another method in the already existing interface? Note that calls to an interface are only made if 
a connection to it has been provided to the PostScriptInterpreter and checking whether it has been is lightning fast.
However, recognizing the problem situations noted here is probably not at all efficient or possible, and so the client
will probably want one or the other. Of course, both interfaces could be provided as well.