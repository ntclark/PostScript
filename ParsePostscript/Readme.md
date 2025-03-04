# ParsePostscript

This is the GUI host used to embed the actual [PostScript interpreter](../PostScript) for development purposes, or, for that matter demonstration purposes.

It is a windows application that implements the appropriate COM interfaces used to host the PS Interpreter COM object

As such, it is an excellent example of my COM centric interaction techniques between a windows user interface application, and any given COM object
that understands the IOleObject type of interfaces.

Specifically, this project demonstrates to you how you would embed the main PostScript interpreter engine in some system **you** may be developing in order 
to seamlessly implement PostScript file ('.ps) interpretation for whatever reason you might find necessary.

