# The EnVisioNateSW Font Management System

As a far more flexible, usable, simpler, and cleaner font rendering system than freetype, this component is intended
to demonstrate how software **can** be constructed such that it is accessible and easy to understand and to follow how something
so incredibly complex as font technology works.

I tried heroically to follow freetype with a deep dive using samples, experiments, and always the debugger
but spun my wheels so much I just simply gave up. It turns out to be the equivalent of the Marianas trench
in terms of any "deep dive" and it is simply **not** worth that amount of my time just to see how 
the technology works.

Like most projects in this repository, this is a COM object that you can use in any context for rendering text.

Note at this point it is for Type42 fonts. OpenType ? TrueType ? I don't remember what any documented difference
between the two is, so maybe I've got that wrong, doesn't matter to me.

It also turned out that "rasterization" itself is not only **not** a part of this implementation, with all of the concepts
I needed to work on in this entire repository, rasteration was the one thing I struggled with the most.

My goal with freetype was specifically to learn how to implement rasterization. By the time I approached 
this task, I fully understood how to tear apart a font file, or the font file structure from MS calls regarding
installed fonts. That is, to obtain the simple and composite glyph outlines, spacing and other geometries.

So then I needed to rasterize those things, I didn't need to see how the glyph shapes were defined, only to
see how to render them properly.

Because the Microsoft Documentation never issued the statement "We can rasterize paths really easy with plain old Direct 2D", I'm off 
on this wild goose chase trying to figure out rasterization with freetype.

As I mentioned above, I flat out gave up on freetype for this part of my learning. For one thing, they actually merge the
rasterization and glyph outline definitions together. Not even a single thought to software integration[^1]. 
I probably can't describe how difficult that made analyzing the one thing from the other without getting in a 
tizzy, so I won't go there.

In fact, rendering itself (to say nothing of rasterization) also does **not** occur using this component. 
Instead, this component uses another in this system (as a COM object) to actually [render the graphics](../EnVisioNateSW_Renderer).

That component is not specific to font rendering at all, it is a powerful graphics engine in it's own right.
Font rendering in this context is just drawing and filling shapes that are defined by *this* component, the "Font Manager".

Note also that just about everything done here (if not everything) is built using the [online documentation](https://learn.microsoft.com/en-us/typography/opentype/spec/)
published by MicroSoft. Most everything needed to build more capability, if necessary, would be described therein.

The project contains my simple COM object boilerplate. For a complex (hostable) COM object, see the PostScript project as the object, and the HostPostcript project as a sample host

## Usage example

I have create a small sample windows application in this repository to host the Font Manager component so you can see
how easy it would be to put this component to use in your own work.

You can check out the sample at: [Font Manager Example](./fontManagerDemo).

There is no requirement in this example for any Open Source system whatsoever (other than what is in this repository).
Therefore, this example, indeed like almost anything I have ever developed, relies only on itself, the Windows API, 
and sometimes other like systems developed and available from me.

[^1]: It's ironic, most developers don't get the subtle power of actually pulling pieces of a system apart 
from each other to make them fantastically more amenable to real integration and extensibility.