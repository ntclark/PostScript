# PostScript COM Interfaces

## COM is a truly beautiful thing

I first used COM with the ATL (Active Template Library) in 1998 to embed a custom OpenGL graphics object into a project built with PowerBuilder.

That was very interesting and worked well. However, I never like to use anything that hides the details from me so in my own projects and pretty much everything thereafter,
I switched to Raw COM and never looked back.

Raw COM is in fact very simple. There are a huge number of interfaces that might be involved in any particular system using COM based on the precise needs.
However, with a basic boilerplate implementation to start with - one can use the debugger to find out what functionality the client, that is, 
the software entity using your object, be it your software, or some external system, is asking you to provide.

It is through that mechanism that I was able to determine how, for example, to create a visual COM object that might be embedded in something like a MS Word document. 
Because of the horrendous documentation for the requirements in such an exercise, one has to discover what interface is being asked for, by debugging, then look up 
the members of *that* interface. Such is a ramification of the bullshit practice of documenting everything with something like Doxygen. 
Lazy developers no longer feel the need to create comprehensive "how-to" documentation anymore and fall back on the totally inadequate 
"per method", or "per interace" documentation. A glaring example of the ineffectivenes of so called "automated" software systems
that end up doing more harm than good.

The good news is that I've figured this stuff out over the years, and have come up with boilerplate software that I can use over and over in different scenarios without any diffulty at all. 

That software is included in this repository, and you may find it useful in other projects. To see how it works you might inspect 
the [HostPostscript](../../../HostPostscript) project to see how this [PostScriptInterpreter](../../) is embedded into
the windows of the former project.
