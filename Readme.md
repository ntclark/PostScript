<h1>Software quality is my passion</h1>

I became a PostScript guru back in 1985 or so, that is 40 years ago.

My specific dive into the language was to write a TeX like markup language in a DEC Vax environment.

I used the PL/I language, in which I was very proficient, to interpret plain text files, containing 
markup escape sequences defined by my system, and output PostScript language files sent to PostScript printers.

This was in the US Defense engineering industry, and my user base consisted of highly technical engineering and mathematics staff who gravitated 
to my system and took full advantage of the incredible flexibility and power of it.

Indeed, one colleague invented an entirely new algebraic notation under government contract, and in fact, it could <i>only</i> be realized 
utilizing my markup system.

Unlike that system in the 80's, the software hosted here-in is <b>not</b> a markup language, or in any way a "client" of PostScript.

This system is PostScript itself, from the ground up. It is a raw C++ implementation of the PostScript language! In other words, this software reads 
*.ps input files, and renders the contents WYSIWYG per the PostScript language definition.

<h2>WHY ?</h2>

Yes, there is already an open source solution for this, don't assume I'm not intametly aware of GhostScript. In fact, I know more than 
I want to about that software. 

In some ways, it was working with GhostScript that engendered my desire to re-visit my PostScript passion. This time, however, it is to create 
a replacement for GhostScript while at the same time forming the foundation upon which I might realize my much more important lifetime passion.

<h2>That lifetime passion ??</h2>

To finally and completely expose the absolute SHIT that ALL open source, and as far as I know, most if not ALL proprietary software truly is.

In my 43 years of deep technical software language; platform; techniques; that is, everything involved in the production of software experience - I have never
been able to fathom why on EARTH the software I have encountered is considered "acceptable" when it is SO sloppy, absolutely fraught with bugs, 
while extensibility and maintainability is as far away as the moon, and which is architected no better than I could have done as a 10 year old.

I have "rescued" systems over and over again, where every time, a rewrite from the ground up is the ONLY way to resuscitate the system. 
And, more times than not, I've had to do that on my own time, or at least without the approval of the muckity-mucks - 
because - those people, while in their incessant "design" meetings, have no clue how their own system even works - or, for that matter, 
how software development actually works, while at the same time, nothing can be done without some bullshit time estimate that <b>nobody</b> can
accurately predict and some sort of "task" created with feet to the fire on that estimate.

While all the time they buy into the bullshit best practices and engineering/software "processes" so they can pretend to be 
arriving at quality software. Which is way beyond their actual reach.

I worked for 8 years on the single most important system the US Federal Highway Administration uses to estimate it's bi-annual budgets.

That system had the most insane massive software bug that I, for years, tried to point out to the people, ostensibly accountable to the government, 
and was ignored over and over again. Why fix it if it's bringing in all the money right ? I tried to explain that bug to another individual 
who would host the system and described using the debugger to discover this bug, at which point he said, "What's the debugger" - after at least 
10 years as the person with the responsibility of the system.

This bug, impacting results in the trillions of dollars, yes, trillions with a T, first came to light when some moron manager - with a PhD in computer
science, no less, directed the "programmer peon" that "Everywhere ____ appears, make that a call to ____ instead".

That caused a bug to be in that system for years, either because "programmer peon" could not have thought it through, or didn't care, or the 
manager moron had no clue how software works ( a given ), or all three.

That real life example points out one of the reasons I have an absolute passion for software quality, that there can be severe ramifications
to a) inattention to every last detail in software development, and b) the real cost of un-informed, un-caring, un-educated (or over-educated) 
muckety mucks who can't possibly think beyond the top-level of system/software tools (i.e., can read any white paper and think they know something
when they actually know nothing because white papers say nothing).




