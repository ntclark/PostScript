# PostScript COM Event Interfaces

## COM is a truly beautiful thing

If this paraticular PostScript interpreter tool is going to be useful to you for your *specific* needs, then you'll need to provide some
mechanism to allow the tool the ability to let you know when certain things happen.

You need to implement an event interface.

To be clear, *your* software "implements" an interface. That is, you write the code that follows a precise definition in terms of calling convention, parameters, and order of functions.
*Then*, you provide a pointer **to** that interface to *this* object, and this object will call the appropriate functions in the interface with the specific parameters at the proper time.

You may already be familiar with all of that, though if it helps there will be a step by step later on.

## Limited functionality at this point

There is only one event interface defined in the system at this point as follows:

```
    IPostScriptEvents interface {
        HRESULT RenderChar(POINT *pPoint,char c);
        HRESULT RenderString(POINT *pPoint,char *pszString);
    }
```

Remember that this is an interface provided by *you*. It can be written in almost any language, as long as the binding
between your system and this PostScript Interpreter COM object exists.

The typical usage of this interface is probably when you are looking at what text might be in a document. 
As each element of text is found in the PostScript language file, the system will pass that text and it's location 
(in units of points) in the document and you can do with it whatever you want.

Text from PostScript files may be printed in a couple of ways. Some text may be placed in the result a character at a time. 
In other words, the PostScript code contains a "moveto" command followed by the output of a single character, followed
by another "moveto" for the next character[^1], and so on. This would result in multiple calls to `RenderChar` in the events interface.

Other times an entire string, or parts of a string, might be placed in the document. In this case, a call to `RenderString` would occur.
The software in this repository has no control over how the print processor (typically pscript5.dll) chooses to print the text.

It is also important to understand that some print jobs might use a Type3 font. This type of font does not have recognizable
"characters" that you would be able to actually "extract" because these "characters" are a series of PostScript compatible
graphics commands that actually draw the character when encountered.

And still another issue is that sometimes characters, really "glyphs", are given in a numeric form that the font mechanism might
lookup for the glyph definition, such as for what are called "composite glyphs" which are something like multiple characters 
overlayed at the same location. Consider umlaut characters, typical a lower case vowel with a horizontal colon above them.

Given these issues, one might consider creating an event interface that presented the client with a bitmap of the rendered
characters[^2]. Even more of a possibility would be to pass those bitmaps onto some OCR tool.

## Note

My CursiVision product contains a PostScript print driver that can be configured to launch any process against the resulting PostScript language file. If that configured process is software that
you write - you can immediately start using PostScript parsing and the above event interface to build your document analyization system.

If you would like the installer for that Print driver, just ask at Nate@InVisioNateSW.com and I'll pass it along.

# Connecting up events

If you don't already know how, I'll describe here how you can integrate your events interface with the PostScript interpreter.

If you'd like to see an example, check out the [psDemo](../../../psDemo) project.












[^1]: Concepts such as kerning and other text positioning or size have no relevance to this software. Those issues are taken care 
of upstream of any PostScript language file. That file contains all the information necessary to render the text in the
right place and size.

[^2]: Why not just another method in the already existing interface? Note that calls to an interface are only made if 
a connection to it has been provided to the PostScriptInterpreter and checking whether it has been is lightning fast.
However, recognizing the problem situations noted here is probably not at all efficient or possible, and so the client
will probably want one or the other. Of course, both interfaces could be provided as well.