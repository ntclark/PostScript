# PostScript

### This is the heart of this entire repository, it **is** the PostScript interpreter

#### Everything else was built to support the interpreter

However, the [Font Manager](../EnVisioNateSW_FontManager) and [Renderer](../EnVisioNateSW_Renderer) are both full featured COM objects that can be used for their respective purposes anywhere you need the functionality.

## A PostScript language interpreter

PostScript is the programming language that built Adobe. You probably know that originally (mid 80's ?) the language interpreter was generally executing in the firmware of laser printers. 
I became enamored with the language around that time because we needed rich and powerful (and flexible) document processing capabilities in my place of employment.
I obtained and quickly consumed the famous red and green PostScript reference manuals published by Adobe which fully described the language.
Using this understanding of the language, I wrote a document processing mark up language, like TeX, in PL/I in a VAX environment. That tool was very well
received and utilized extensively by engineering and mathematics staff.

As a "forth type" language, it was all new to me at the time, but I soon started hearing things like "begin 4 1 roll dup exch pop def end" in my dreams.
That's not a real statement but you get the idea.

If you've only heard of PostScript in the context of printers and never in the realm of windows applications, you may be wondering why anyone (me) needs to create a new interpreter for it.

### Myth busted

> PostScript is a dead language and is going away

This is absolutely <u>not true</u>.

First, the PostScript language is actually used a lot more than people may realize. Microsoft ships the pscript5 dll on Windows. That component makes it incredibly easy to create a
"PostScript compatible" print driver that can print from literally *anything* and produce a robust PostScript ('.ps) language output file. And those files, given tools downstream from the print driver, are the
raw materials to go on and create '.PDF (Portable Document Format) WYSIWYG soft copy documentation. These PS print drivers are literally everywhere and are a dime a dozen. However, they invariably 
also include or invoke one of those "downstream tools" to produce '.PDF from the generated PS source, then throw out the '.ps file.

To believe PostScript is going away is to believe PDF files are going away and anyone who says so is either un-informed, or is pulling your leg

However, creating PDF files is **only one** possibility enabled by PostScript sources.

Often I see talk of the need to analyze and/or otherwise scrape or pull data from '.PDF files. I see it all the time: "extract this or that 
from our PDF files" in proposals for contract work.

Well, why not think upstream just a bit, and rather than actually create a 'PDF file and *then* do your analysis on *that*, instead, 
let the PS interpreter spit out the data you're looking *while* the PostScript language is being parsed by *this* interpreter?

How can this particular repository help you do that ? Why, with COM (Component Object Model) Event interfaces naturally.

To learn more of this, please checkout the [information](./Sources/COM Events) about interfaces in this system.

## What about the PDF creation tools

The ultimate goal of this repository and it's future capabilities is in fact developing the tools to enable the creation of PDF artifacts from the PostScript source.

However, this is really a two step evolution. 

First, the idea expressed above to obtain information within '.ps files at the time they are parsed is a very real
and useful capability. I need this functionality myself in some of my other work and having that available and implemented here is in fact a viable product in it's own right.

Second, if you really want PDF generation right now, there are other Open Source options for you. 
One such option that everyone knows is GhostScript. There are others you can find via a google search

Another far better option in the future will be right here. Watch this space, my next task will be just that, implement the PDF creation part of my system.

My disdain for most Open Source is not a secret, if you're curious about that, peruse my thoughts on the [root page](../) readme.

In fact, my entire reason for writing this system is because I believe GhostScript is an absolute nightmare to understand.
I guess those shocked by that statement probably think I'm unfair and wonder if I had bothered to read the documentation or usage notes.
What those people may have assumed is that I'm talking about "usage" - I am not.

You see, *anytime* I'm going to use any software whatsoever in a system that will have my name associated it, I will always bring that software as a project sid by side into my
development environment and build and debug through it at the **very** deepest level. I insist in thoroughly understand everything that software
does and how it does it, and most importantly - what are, and how did they implement, the underlying algorithms, equations, or any aspect
of the solution domain for the system.

Put simply, I will **not** put my name on software if it includes software that I do not **completely** understand. I must also believe that it is completely free 
of bugs and has at least some modicum of cleanliness, readability, efficiency and ability for maintainability and extensibility.

Therefore, as it happens, I have rarely actually used Open Source in my software. I use open source to learn of it's strategy in satisfying the domain space, and I'll
usually inspect multiple approaches. Ultimately, I generally go back to inventing my own approach based on the definitive documentation if I can find that.

When I say GhostScript is "a nightmare to understand" - I refer directly to the horrible spaghetti code in the system, the incredibly bad structure of the
software itself, the antiquated arrangement of the sources, etc[^1]. Yes - I gave it weeks of my time to unravel - and ultimately just simply realized that, holy shit,
I just need to start this from scratch. The Adobe PostScript books are very detailed and precise, why not just start there and get it done right once and for all.

[^1]: ## >Yes, okay, it's decades old, I get that and maybe I'm unfair to the original authors. However, maybe if the system truly is the pride of those authors, why then, wouldn't they want to see it grow and mature and keep up with the times. *That* is what I would do and I think that *my* dedication and promise to the software that I bring into this world gives me at least some license to criticize those who abandon their own creations.

I'll have to say as well, if I thought GhostScript was bad - I was in for double shock when I tried to understand font technology/rendering with Freetype - AACH 
that was even worse.











