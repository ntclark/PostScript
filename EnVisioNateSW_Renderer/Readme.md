# The EnVisioNateSW Renderer

This component of the system affects the visual representation of everything else in the entire system.

In other words, nowhere else is there any code that calls the Windows GDI or Direct2D components to show entities on the screen (or any Device Context).

At some point, this component simply rendered text on behalf of the [Font Manager](../EnVisioNateSW_FontManager) however, it evolved to handle all lines, shapes, and images for all components of the system.

As a COM compnent, you can instantiate this from any software system you might want to generate graphics in. It has a simple COM interface and an elegant and flexible
implementation pattern that you could easily expand when your needs exceed those of the rest of this system.

In addition, you need not be an expert in the MS graphics components to utlize this component.

I pontificate elsewhere about commenting code. I've made the point that if you **are** going to comment code, do so in a manner similar to that 
indicated by [this file](./EnVisioNateSW_Renderer/COM Interfaces/IGraphicElements_Paths.cpp).
Those comments are taken verbatim from the PostScript reference manual. However, that does not mean that this component is 
geared solely toward rendering PostScript - it only means that the usage of this component is the same as that described in the PostScript manuals.
