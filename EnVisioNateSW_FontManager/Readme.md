# The EnVisioNateSW Font management and rendering system

A far more flexible, usable, simpler, and cleaner font rendering system than freetype which is intended
to demonstrate how a system **can** be constructed such that it is accessible and easy to see and follow how something
so incredibly complex as font technology works.

I tried heroically to follow freetype with a deep dive using samples, experiments, and always the debugger
but spun my wheels so much I just simply gave up. It turns out to be the equivalent of the Marianas trench
in terms of any "deep dive" and it is simply **not** worth that amount of my time just to see how 
the technology works.

Like most projects in this repository, this is a COM object that you can use in any context for rendering text.

Note at this point it is for Type42 fonts. OpenType ? TrueType ? I don't remember seeing any documented difference
between the two.

It also turns out that "rasterization" itself is completely absent from this implementation. There is 
no need to worry about it because the MS Direct2D components do that for you pretty well, thank you very much[^1].

[^1]:Yes, this is **not** multiplatform, and I really don't care. Maybe linux has some sort of implementation 
for Direct2D and if so, great, maybe someday I'll try it.
But, as I've said before "The needs of the many outweigh the needs of the few" - Spock.

In fact, rendering itself actually does <u>not</u> occur using this component. Instead, it uses yet another component
of this system (as a COM object) to actually [render the graphics](../EnVisioNateSW_Renderer/Readme.md).
That component is not specific to font rendering at all, it is a powerful graphics engine in it's own right.
Font rendering in this context is just drawing and filling shapes that are defined by *this* component, the "Font Manager".

Note also that just about everything done here (if not everything) is built using the [online documentation](https://learn.microsoft.com/en-us/typography/opentype/spec/)
published by MicroSoft. Most everything needed to build more capability, if necessary, would be described therein.

The project contains my simple COM object boilerplate. For a complex (hostable) COM object, see the PostScriptGen2 project as the object, and the ParsePostcript project as a sample host




