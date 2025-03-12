# PostScriptInterpreter

### This is the heart of this repository, it is the PostScript interpreter

#### Everything else was built to support the interpreter

Even so, the [Font Manager](../EnVisioNateSW_FontManager) and [Renderer](../EnVisioNateSW_Renderer) are both full featured COM 
objects that can be used for their respective purposes anywhere you need their functionality, completely independent of the interpreter.

## A PostScript language interpreter

PostScript is the programming language that built Adobe. You probably know that originally (mid 80's ?) the PS language interpreter was generally executing in the firmware of laser printers. 
I became enamored with the language around that time because we needed rich and powerful (and flexible) document processing capabilities in my place of employment.
I obtained and quickly consumed the famous red and green PostScript reference manuals published by Adobe which fully describe the language.
I wrote a document processing mark up language, like TeX, in PL/I in a VAX environment that would output PostScript compatible code to a file or printer.
That tool was very well received and utilized extensively by engineering and mathematics staff.

As a language similar to forth, it was all new to me at the time, but I soon started hearing things like "begin 4 1 roll dup exch pop def end" in my dreams.
That's a real snippet of PostScript.

If you've only heard of PostScript in the context of printers and never in the realm of windows applications, you may be wondering why anyone (me) needs to create a new interpreter for it.

### Myth busted

> PostScript is a dead language and is going away

That is absolutely **not true**.

First, the PostScript language is used a lot more than people may realize. Microsoft ships the pscript5 dll on Windows. That component makes it incredibly easy to create a
"PostScript compatible" print driver that can print from literally *anything* and produce a robust PostScript ('.ps) language output file. And those files, given tools downstream from the print driver, are the
raw materials used to go on and create '.PDF (Portable Document Format) WYSIWYG soft copy documentation. These PS print drivers are literally everywhere and are a dime a dozen. However, they invariably 
also include or invoke one of those "downstream tools" to produce '.PDF from the generated PS source, then throw out the '.ps file. 

What we are doing here is placing ourselves in the middle, between the Print Driver creating PS code, and the downstream tool used to create a '.PDF file from that. 
There are subtle and powerful reasons for doing that.

To believe PostScript is going away is to believe PDF files are going away and anyone who says so is either un-informed, or is pulling your leg

However, creating PDF files is **only one** possibility enabled by PostScript sources.

### PostScript rendering

The phase one aim of this project is to *display*, or "render" PostScript sources exactly as they would look if they *were* transformed to PDF.

In fact, you may not want PDF at all, maybe you just want to paint the document in some Windows application window. No need to go to the extra,
significantly complex, step of creating a PDF file, and then use some other tool, like pdfium, to display it. I've worked with pdfium hosting
PDF documents in a series of web pages using the MSHTML, or WebView tool. Though powerful, it is by no means trivial. 

With this PS Interpreter as an embeddable COM object it is very easy to render a Postscript printer language file ('.ps files) in any
windows application you may be developing. All you need is a device context to point the rendering to. And, *that* could be one from a 
physical printer for that matter.

For guidance on rendering PostScript in a Windows application, see the [HostPostscript](../HostPostscript) project in this repository.
That project uses the COM interfaces (IOleObject, etc) that are necessary to instantiate and control a visual COM object while *this* project
defines [interfaces](./Sources/COM%20Interfaces) that allow it to be a client hosted into a project like HostPostscript.

For that matter, these two projects, host and client, utilize the COM boilerplate code that I have used for more than a decade
to achieve the embedding of a visual component into a hosting windows application. This code is robust and almost completely plug and play
for future projects you may want to build[^1]. Check it out and use it in one of your own projects if desired. Like all my software,
though it is part of the projects in this repository - I have done as much as I can to enable you to see exactly HOW to extract it
and use it elsewhere. If source filters and folders like "COM Interfaces/being hosted" aren't useful, then I'll have to rethink
how to organize this stuff even better (but they are useful).

# Note:

I have developed a Print to PostScript/PDF print driver for my CursiVision product. That print driver, out of the box, can be configured to do **anything** with the 
resulting PostScript language file. If you have a need to develop a process that could be front-ended by printing a file (which can be automated by the way),
I have no problem giving you an installer for that print driver. Simply ask and you shall receive, send an e-mail to Nate@EnVisioNateSW.com.

As a real world example of this, consider configuring the print driver I mention to launch the [HostPostscript](../HostPostscript) executable from this repository.
If you do that, you can print any document whatsoever, and immediately render it to your screen.

For that matter, take the HostPostscript out of this repository - and modify it to be the rendering component of some system you're developing.

## Analyzing data in/from PDF files

Often I see talk of the need to analyze and/or otherwise scrape or pull data from '.PDF files. I see it all the time: "extract this or that 
from our PDF files", for example, in proposals for contract work.

Well, why not think upstream just a bit, and rather than actually create a 'PDF file and *then* do your analysis on *that*, instead, 
let the PS interpreter spit out the information you're looking *while* the PostScript language is being parsed by a smart, and more advanced, interpreter, such as this one?

How can this particular repository help you do that ? Why, with COM Event interfaces naturally.

To learn more of this, please checkout the information about [event interfaces](./Sources/COM%20Events) in this system.

## What about the PDF creation tools

The ultimate goals of this repository and it's future capabilities do entail developing the tools for the creation of PDF documents from the PostScript source.

However, this is really a two step evolution. 

First, the ability to simply render the document without necesssarily creating a PDF has been very important in my work and I rely on it heavilty.
Also, the idea expressed above to obtain information within '.ps files at the time they are parsed is also a useful capability.
I need this functionality myself in some of my other work and having that available and implemented here is in fact a viable product in it's own right.

Second, if you really want PDF generation right now, there are other Open Source options for you. 
One such option that everyone knows is GhostScript. There are others you can find via a google search

Another far better option in the future will be right here. Watch this space, my next task will be just that, implement the PDF creation part of my system.

My disdain for most Open Source is not a secret, if you're curious about that, peruse my thoughts on the [root page](../) readme.

In fact, my entire reason for writing this system is because I believe GhostScript is an absolute nightmare to understand.

I'm sure those shocked by that statement probably think I'm unfair and wonder if I had bothered to read the documentation or usage notes.
What those people may have assumed is that I'm talking about "usage" - I am not.

You see, *anytime* I'm going to use any software I didn't write in a system that will have my name associated with it, I will always bring that software as a project side by side into my
development environment and build and debug through it at the **very** deepest level. I insist in thoroughly understanding everything that software
does and how it does it, and most importantly - what are, and how did they implement, the underlying algorithms, equations, or any aspect
of the solution domain for the system.

You could say I trust no-one but myself in terms of assuring defect free, robust, and extensible software, and you'd be 90% correct.
Actually, I don't even trust myself - the only thing I trust is what I see executing in the CPU and what I see on the other side of executing in the CPU.
In otherwords, with everthing I develop I am in the debugger watching every single line execute and I am in there for way more time than I'm in the editor itself.
It is painfully obvious that the vast majority of Open Source Software developers, from the standpoint of lack of readability alone, have a tenth the 
dedication that I do for debugging, and that alone is enough to reject the use of that system in my software.

Put simply, I will **not** put my name on software if it includes parts that I do not **completely** understand. I must also believe that it is 100% free 
of bugs and has at least some modicum of cleanliness, readability, efficiency and ability for maintainability and extensibility. The term "black box" has
a use in something like electronics. In software, not so much. It's bad enough I have to trust Windows internals without seeing the source code but I 
can't bring myself to use something when I have the option to see what it's made of.

Therefore, as it happens, I have rarely actually used Open Source in my software. I use open source to learn of it's strategy in satisfying the domain space, and I'll
usually inspect multiple approaches. Ultimately, I generally go back to inventing my own approach based on any definitive documentation if I can find that.

When I say GhostScript is "a nightmare to understand" - I refer directly to the horrible spaghetti code in the system, the incredibly bad structure of the
software itself, the antiquated arrangement of the sources, etc[^2]. Yes - I gave it weeks of my time to unravel - and ultimately just simply realized that, holy shit,
I just need to start this from scratch. The Adobe PostScript books are very detailed and precise, why not just start there and get it done right once and for all.

I'll have to say as well, if I thought GhostScript was bad - I was in for double shock when I tried to understand font technology/rendering with Freetype - AACH 
that was even worse and I have nothing to show for, again, weeks of my time trying to follow it.

**I have very strong opinions about software quality, and Open Source in particular. As far as I know, closed source is just as bad, but all I can do is  start 
with open source**

**Having been writing software for 50 years, and being goood at it, and especially having produced high quality systems, I am taking it on myself to say these things
whether or not doing so makes me popular among my peers.**

<span style="font-size: 2Em">**What I'm saying needs to be said - and my goal is very much for the good of the state of software development**</span>

[^1]: **However, I encourage you to use your debugger and step through the code in all of these interfaces in order to really understand how it works. It is ultimately somewhat simple, though the quality of the MS documentation may leave you wondering as to that.**

[^2]: **Yes, okay, it's decades old, I get that and maybe I'm unfair to the original authors.*However*, maybe if the system truly is the pride of those authors, why then, wouldn't they want to see it grow and mature and keep up with the times. *That* is what I would do and I think that *my* dedication and promise to the software that I bring into this world gives me at least some license to criticize those who abandon their own creations.**











