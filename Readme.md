# Software quality is my passion

I became a PostScript guru back in 1985 or so, that is 40 years ago. My specific dive into the language was to write a TeX like markup language in a DEC Vax environment.

I used the PL/I language, in which I was very proficient, to read plain text files, containing 
markup escape sequences defined by my system, and output PostScript language files sent to PostScript printers.

This was in the US Defense engineering industry, and my user base consisted of highly technical engineering and mathematics staff who gravitated 
to my system and took full advantage of the incredible flexibility and power of it.

Indeed, one colleague invented an entirely new algebraic notation under government contract, and in fact, it could <i>only</i> be realized 
utilizing my markup system.

Unlike that system in the 80's, the software hosted herein is <b>not</b> a markup language, or in any way a "client" of PostScript.

This system is PostScript itself, from the ground up. It is a raw C++ implementation of the PostScript language! In other words, this software reads 
*.ps input files, and renders the contents WYSIWYG per the PostScript language definition.

# WHY ?

Yes, there is already an open source solution for this, please know that I am intimatly aware of GhostScript. In fact, I know more than 
I want to about that software. 

In some ways, it was working with GhostScript that engendered my desire to re-visit my PostScript passion. This time, however, it is to create 
a replacement for GhostScript while at the same time forming the foundation upon which I might realize my much more important lifetime passion[^1].

[^1]: Okay, yes, I benefited from GhostScript and from the efforts of those people who put it together. I **did** use it in my own software 
for electronic signature capture. However, like ALL Open Source Software, the amount of effort you're probably going to spend getting what 
you want out of it is, in the fullness of time, way more than if you had just started from scratch. In any case, the only value I've *ever* 
found with OS software is learning "how" something works by example, even if it is badly written and incredibly hard to follow.
Both sloppy disorganized software **and** horribly bad or overcomplicated documentation on how an algorithm should work contribute
mightily to the high cost of software development. *Please* don't say "re-inventing the wheel", that will make me puke.

## My lifetime passion

<p style="text-align:center;width: 256px">To completely expose the absolute SHIT that ALL open source, and as far as I know, most if not ALL proprietary software truly is.</p>

In my 43 years of deep technical software language; platform; techniques; that is, everything involved in the production of software; experience - I have never
been able to fathom why on EARTH the software implementations I have encountered are considered "acceptable" when they (<b>all of them</b>) are SO sloppy, absolutely fraught with bugs, 
while extensibility and maintainability are as far away as the moon, and which is architected no better than I could have done as a 10 year old.

I have "rescued" systems over and over again, where every time, a rewrite from the ground up is the ONLY way to resuscitate it. 
And, more times than not, I've had to do that on my own time, or at least without the approval of the muckity-mucks - 
because - those people, while in their incessant "design" meetings, have no clue how their own system even works - or, for that matter, 
how software development actually works, while at the same time, nothing can be done without some bullshit time estimate that <b>nobody</b> can
accurately predict and some sort of "task" created with "feet to the fire" on that estimate.

Every time the muckity-mucks buy into the bullshit best practices and engineering/software "processes" so they can pretend to be 
arriving at quality software. Which is way beyond their actual reach.

### Hear me O-muckety-mucks

If they had the balls to really go into their system at the code and architecture level and actually <b>see</b> what's ultimately
being produced - to actually USE a debugger - then maybe they'd realize what I'm trying to say and then I'd be happy that maybe I can make some progress in my passion - 
that of finally doing something to make the actual writing of software (don't get me going on the use of the term "programmer") the disciplined
engineering endeavor that it should have been from the beginning, but currently is not.

Then, and perhaps only then - <i>maybe</i> all those high-fallutin' software processes might actually work, at least they might have a 
foundation to actually stand upon. Until then, however, everyone is literally barking up the wrong tree, and simply dancing around the problem looking like a bunch of fools.

### To whit...

I worked for 8 years on the single most important system the US Federal Highway Administration uses to estimate it's bi-annual budgets.

That system had the most insane massive software bug that I, for years, tried to point out to the people ostensibly accountable to the government, 
and was ignored over and over again. Why fix it if it's bringing in all this money right? I tried to explain that bug to another individual 
who hosted the system and described using the debugger to discover this bug, at which point he said, "What's the debugger" - this after being, for at least 
10 years, the person with the ultimate responsibility of the system.

This bug, impacting results in the trillions of dollars, yes, trillions with a T, first came to light when some moron manager - with a PhD in computer
science, no less, directed the "programmer peon" that "Everywhere the variable ____ appears, make that a call to ____ instead".

That caused a bug to be in that system for years, either because "programmer peon" could not have thought it through before implementing it, or didn't care, or the 
manager moron had no clue how software works ( a given ), or all three. I came upon the system professionally and found this bug within three days.

The above real life example drives one of the reasons I have an absolute passion for software quality, that there can be severe ramifications
to a) inattention to every last detail in software development, and b) the real cost of un-informed, un-caring, un-educated (or over-educated) 
muckety mucks who can't possibly think beyond the top-level of system/software tools (i.e., can read any white paper and think they know something
when they actually know nothing because white papers say nothing).

With all of my being, I truly believe that the promise of software technology would be much further ahead if only the industry had demanded precision and cleanliness 
akin to that of, say, engineering drafting and blueprints at the getgo.

# How it should be

This particular project is in fact intended to be the foundation for my book. 

In fact, the book is 40 years in the making. As I've mentioned, I've come across many systems, proprietary ***and** Open Source, in which I am literally 
amazed that they work at all, while immediately understand exctly why 1) they are SO long in development, and 2) it is SO difficult to bend into new and powerful uses.

My book will outline, side-by-side the incredible positive differences that come about thru pristinely written and to the point code that is free from distracting and 
obnoxious constructs that literally crowd out the true meaning.I cannot wait to publish this book, but in the meantime, this site contains what might be considered
the prologue to it.

In as much as I may come across as a jerk in my descriptions here and in the discussions about other peoples' work I will say that I truly hope the reader will 
understand that my goal is pure - that of truly improving the state of the software development industry by highlighting those things that have prevented that
improvement for probably about 75 years !!

This is not a work of fiction, and no names have been changed to protect the innocent. If a name is mentioned, that individual is probably a part of the problem, and is **not** innocent - sorry.

## general organization

I remember in the 80's when file names got the permission to be of length greater than 6. Or for example, variables in FORTRAN (yes FORTRAN) could be greater than 6 letters. 
WOW ! I went absolutely bananas over this new and exciting realm of possibility!

And here we are 4 decades later - and **STILL** most developers do not leverage this !! Well sure, it's great people figured out how to put like functionality grouped
in file by file basis - but then don't indicate what that grouping is via file name !?!? To say nothing of the ability to group even higher levels using folders, and *that* is rarely done.
I mean WTF ? Is it THAT hard to organize your sources ? Maybe people actually put all the source files in one place because they can't figure out how the environment points to the headers ?

And, what's with prepending something on file names to indicate, I don't know, where it came from ? Something ? Are you kidding me ? Is there a point to using up space in every file name in the sources for 
any reason ? Especially when it's apparently some kind of problem to actually name a file for it's relevance ? 

Don't get me started on that incredibly lazy practice of putting "main" at the bottom of a file, just so you don't have to create a header for all the artifacts main might use! That, dear reader, 
is a glaring indicator of not only a lazy individual, but also one who could not care less about understanding structure and organization. I'm going to bet you that this person will repeat 
a declaration in some other source file when they finally want to expand; then , there we go, the system is falling apart even when only two source files comprise it.

Yes, a hundred times yes, it is best to follow the common sense strategy for **EVERY** situation, 1 file in the system, or 1000 files. There is no excuse for shortcuts, do it right every time and every place.

My book will go into these and many more details about how software needs to be written, here are some topics:

- Learn and understand at the deepest level of detail the technology and keep up to date, and go back and improve when you realize you did it wrong the first time
    - Be incredibly knowledgeable about the technology to the point you've forgotten more than your co-workers will ever know. However, don't talk about Object Orientation at the Christmas party
- Do continuous refactoring all the time. If you look at your code in the future and you don't immediatly understand it, rewrite it until you do
- Do not comment code.
    - If you DO comment code, keep "We" out of it. There is no "We", it is "the system" **not** "We".
    - If your code has comments - the correctness of the comment is FAR MORE IMPORTANT THAN THE CORRECTNESS OF THE CODE ITSELF. You **MUST** update any comment if even the **SLIGHTEST** change is made to code
        - DO NOT READ OR TRUST COMMENTS. They are **NOT** reality. There is only **ONE** reality in software, that is how the instructions flow thru the CPU - there is NO OTHER TRUTH, never trust comments, only trust code by DEBUGGING it or thorough understanding. If the comment is wrong DELETE IT WITHOUT QUESTION
-- DO NOT COMMENT CODE (see below)








