# PostScript

### This is the heart of this entire repository, it **is** the PostScript interpreter

#### Everything else was built to support the interpreter

However, the [Font Manager](../EnVisioNateSW_FontManager) and [Renderer](../EnVisioNateSW_Renderer) are both full featured COM objects that can be used for their respective purposes anywhere you need the functionality.

## A PostScript language interpreter

PostScript is the programming language that built Adobe. You probably know that originally (mid 80's ?) the language interpreter was generally executing in the firmware of laser printers. 
I became enamored with the language around that time because we needed rich and powerful (and flexible) document processing capabilities in my place of employment.
I obtained and quickly consumed the famous red and green PostScript reference manuals published by Adobe which fully described the language.
As a "forth type" language, it was all new to me at the time, but I soon started hearing things like "begin 4 1 roll dup exch def end" in my dreams.
That's not a real statement but you get the idea.

If you've only heard of PostScript in the context of printers and never in the realm of windows applications, you may be wondering why anyone (me) needs to create a new version of it.

## Myth busted

> PostScript is a dead language and is going away

This is absolutely not true.

First, the PostScript language is actually used a lot more than people may realize. Microsoft ships the pscript5 dll on Windows. That component makes it incredibly easy to create a
"PostScript compatible" print driver that can print from literally *anything* and produce a robust PostScript ('.ps) output file. And those files, given tools downstream from the print driver, are the
raw materials to go on and create '.PDF (Portable Document Format) WYSIWYG soft copy documentation. These PS print drivers are literally everywhere and are a dime a dozen. However, they invariably 
also include or invoke one of those "downstream tools" to produce '.PDF from the generated PS source, then throw out the '.ps file.

However, creating PDF files is **only** one possibility enabled by PostScript sources.

All the time I see talk of the need to analyze and/or otherwise scrape or pull data from '.PDF files. I see it all the time: "extract this or that 
from our PDF files" in proposals for contract work.

Well, why not think upstream just a bit, and rather than actually create a 'PDF file and *then* do your analysis on *that*, instead, 
let the PS interpreter spit out the data you're looking *while* the PostScript language is being parsed by *this* interpreter?





