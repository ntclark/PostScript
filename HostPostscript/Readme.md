# HostPostscript

>All of my repositories need the [Common](https://github.com/ntclark/Common) repository and there you will find clear 
build instructions for all of them.

This is a GUI host used to embed the actual [PostScript interpreter](../PostScriptInterpreter) for development or demonstration purposes.

If you want, this is something that you could modify to be the document rendering component of your existing or planned software system.

This latter concept is discussed [here](../PostScriptInterpreter/#note).

This project is a windows application that implements the appropriate COM interfaces used to host the PS Interpreter COM object

As such, it is an excellent example of my COM centric interaction techniques between a windows user interface application, and any given COM object
that understands the IOleObject type of interfaces.

Specifically, this project demonstrates to you how you would embed the main PostScript interpreter engine in some system **you** may be developing in order 
to seamlessly implement PostScript file ('.ps) interpretation or rendering for whatever reason you might find necessary.

To use this project, use the interface to navigate to a '.ps file, and then click "Execute".

Note that '.ps files are the project of PostScript print drivers, or the Print to PDF drivers. Though '.ps (postscript source) is probably produced by
a print to PDF driver, that driver might throw the file away.

I have developed a commercial product that contains a PostScript (print to PDF) print driver. I'm more than happy to share that print driver 
if you need one that preserves the '.ps file.

To read more about this, as well as to describe how you might use the sources in this particular project as a viable starting point for a system
you want to write, check out the link above, repeated [here](../PostScriptInterpreter/#note).
