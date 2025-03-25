
# Software quality is my passion

>### Before I get into that, you may be looking for build instructions
>All of my repositories need the [Common](https://github.com/ntclark/Common) repository and there you will find clear 
build instructions for all of them.

I became a PostScript guru back in 1985 or so, that is 40 years ago. My specific dive into the language was to 
write a T<sub>E</sub>X like markup language in a DEC Vax environment.

I used the PL/I language to read plain text files containing markup escape sequences defined by my system, 
and output PostScript language files sent to PostScript printers.

This was in the US Defense engineering industry where my user base consisted of highly technical engineering and
mathematics professionals who gravitated to my system and took full advantage of the incredible flexibility and power of it.

Indeed, one colleague invented an entirely new algebraic notation under government contract, and in fact, it could have <i>only</i> been realized 
using my markup system.

Unlike that system in the 80's, the software hosted herein is <b>not</b> a markup language, or in any way a "client", or user of the PostScript language.

This system **is** PostScript itself, from the ground up. It is a raw C++ implementation of a PostScript language interpreter! 
In other words, this software reads *.ps input files, and renders the contents WYSIWYG per the PostScript language definition.

# WHY ?

Yes, there is already an open source solution for this, please know that I am intimately aware of GhostScript. In fact, I know more than 
I want to know about that software and spent way too much time trying to gain that knowledge.

In some ways, it was working with GhostScript that engendered my desire to re-visit my PostScript passion. This time, however, it is to create 
a replacement for GhostScript while at the same time forming the foundation upon which I might realize my much more important lifetime passion[^1].

[^1]: Okay, yes, I benefited from GhostScript and from the efforts of those people who put it together. I **did** use it in my own software 
for electronic signature capture. However, like ALL Open Source Software, the amount of effort you're probably going to spend getting what 
you want out of it is, in the fullness of time, way more than if you had just started from scratch. In any case, the only value I've *ever* 
found with Open Source software is learning "how" something works by example, even if it is badly written and incredibly hard to follow.
Both sloppy disorganized software **and** horribly bad or overcomplicated documentation on how an algorithm should work contribute
mightily to the high cost of software development. *Please* don't say "re-inventing the wheel", that will make me puke.

## My lifetime passion

>To completely expose the absolute **shit** that ALL open source, and, as far as I know, most if not ALL proprietary software truly is.

In my 43 years of deep technical software language; platform; techniques; that is, everything involved in the production of software; experience - 
I have never been able to fathom why on EARTH the software implementations I have encountered are considered "acceptable" when 
they (**all of them**) are SO sloppy, absolutely fraught with bugs, 
while extensibility and maintainability are as far away as the moon, and which is architected no better than I could have done as a 10 year old.

I have "rescued" systems over and over again, where every time, a rewrite from the ground up is the ONLY way to resuscitate it. 
And, more times than not, I've had to do that on my own time, or at least without the approval of the muckity-mucks - 
because - those people, who can't get out of their incessant "design" meetings, have no clue how their own systems even work - or, for that matter, 
how software development itself actually works. In their world, nothing can be done without some bullshit time estimate that **nobody** can
accurately predict that backs some sort of "task" created with "feet to the fire" on that estimate.

Every time the muckity-mucks buy into the bullshit best practices and engineering/software "processes" so they can pretend to be 
arriving at quality software. Which is way beyond their actual reach.

### Hear me O-muckety-mucks

If they had the balls to really go into their system at the code and architecture level and actually **see** what's ultimately
being produced - to actually USE a debugger - then maybe they'd realize what I'm trying to say and then I'd be happy that perhaps I've 
made some progress in my passion - that of finally doing something to make the actual writing of software 
(don't get me going on the use of the term "programming") the disciplined engineering endeavor that it should 
have been from the beginning, but currently is not.

Then, and perhaps only then - *maybe* some of those high-fallutin' software processes might actually work, at least they would have a 
foundation to actually stand upon. Until then, however, everyone is literally barking up the wrong tree, and simply dancing 
around the problem looking like a bunch of fools.

### To whit...

I worked for 8 years on the single most important system the US Federal Highway Administration uses to estimate it's bi-annual budgets. A system
that impacts the state of every single federal roadway in the whole of the United States.

That system had the most insane massive software bug which I, for years, tried to point out to those ostensibly accountable to the government. 
I was ignored over and over again for years, "Why fix it if it's bringing in all this contract money right?" I tried to explain that bug to 
the one individual who hosted and ran the system for the government and described how I used the debugger to discover that bug.
At which point he said to me, "What's the debugger?" - this after being, for at least 10 years, the person with the ultimate 
responsibility of the system.

This bug, impacting results in the trillions of dollars, yes, trillions with a T, first came to life when a moron manager - with a PhD in computer
science, no less, directed the "programmer peon" that "Everywhere the variable ____ appears, make that a call to ____ instead".

That caused a bug to be in that system for years, either because "programmer peon" did not have the technical acumen (typical) to think it 
through, or didn't care, or the manager moron had no clue how software actually works ( which was true in this case ), or all three. 
When I came upon the system I found this bug within three days.

The commonality of situations like the above is a major factor in the growth of my absolute passion for software quality.
Over the years I have found that there are severe ramifications of poorly built software due
to a) inattention to every last detail in software development, and b) the real cost of un-informed, un-caring, un-educated (or over-educated) 
muckety mucks who can't possibly think beyond the top-level of system/software tools. 
These are the people who can read any white paper and think they know everything when they actually know nothing because white papers say nothing.

With all of my being, I truly believe that the promise of software technology would be much further ahead if only the industry 
had demanded precision and cleanliness akin to that of, say, engineering drafting and architectural blueprinting at the getgo.

# How it should be

This particular project is in fact intended to be the foundation for my book. 

This book is 40 years in the making. As I've mentioned, I've come across many systems, proprietary **and** Open Source, in which I am literally 
amazed that they work at all, while immediately I can see exactly why 1) they are SO long in development, and 2) it is SO difficult to mold 
them into new and powerful uses.

My book will outline, using side-by-side comparison, the incredible positive differences that come about thru pristinely written 
and to the point software that is free from distracting and obnoxious constructs that literally crowd out the true meaning.
I cannot wait to publish this book, but in the meantime, my repositories demonstrate what I consider to be the prologue to it.

In as much as I may come across as a jerk in my descriptions here and in the discussions about other peoples' work I will 
say that I truly hope the reader will understand that my goal is pure - that of truly improving the state of the software 
development industry by highlighting those things that have prevented that improvement for about 75 years !!

This is not a work of fiction, and no names have been changed to protect the innocent. 
If a name is mentioned, that individual is probably a part of the problem, and is **not** innocent.
I offer no apology for my forthrightness, bad language, and phrases possibly offensive to some. What I believe and say needs to
be said and if I'm able to do nothing else in my career I will take on the responsibility of getting this said.

## general organization

I remember in the 80's when file names got the ability to be of length greater than 6. Or for example, variables in FORTRAN 
(yes FORTRAN) could be greater than 6 letters. WOW ! I went absolutely bananas over this new and exciting realm of possibility!

And here we are 4 decades later - and **STILL** most developers do not leverage this !! Well sure, it's great people figured out 
how to put like functionality grouped in a file by file basis - but why then don't they indicate what that grouping is via file name !?!? 
To say nothing of the ability to group even higher levels using folders, and *that* is rarely done.
I mean WTF ? Is it THAT hard to organize your sources ? Maybe people actually put all the source files in one place because 
they can't figure out how the environment points to the headers ?

And, what's with prepending something on file names to indicate, I don't know, where it came from ? Something ? Are you kidding me ? 
Is there a point to using up space in every file name in the sources for any reason ? 
Especially when it's apparently some kind of problem to actually name a file for it's relevance ? 

In the overall culture of Open Source, I see that 1) the growth and enhancement of tools *cannot* be leveraged by the community, 
and 2) (among others) structure and organization are stagnant. Why ? 

- People have the sources local and changes in the repository can't be immediately seen by them
    - Too effing bad. The masses need to learn how to use diff engines - up until 
that time whenever repository technology gets better
        - Oops, the repository technology is **not going** to get better. It is already too difficult and error prone. The masses will have to take 
responsibility for proper merging on their own, and at the same time one and only one owner of the repository must be responsible for it. 
Where are you original authors ? You gave birth to something, then loose all interest in it
- The systems are badly structured and/or architected.
    - In the past systems were built without dynamically loaded component parts ('.dlls, etc) so there are usually way more sources 
a contributor may have to deal with even when only interested in a specific area. 
Well, probably because there **AREN'T** any specific area(s)
    - It is way past time the big Open Source systems need to be re-architected in a substantial way.It is the job of a single individual 
who needs to take the reins and get this done. It *should* be the original author, and I mean the ONE author. I should not have to say this
but it's true, most "original authors" don't really give a shit about their child, if they did, they'd have already refactored/rearchitected 
it - kept it up with the times.
    - Also, don't give me any shit about these system being too big for one person, that's only because they are so sloppy, disorganized, and
poorly architected so that any one person would have a hard time getting their head around it. Too bad, it needs to be done, and by one person.
In a non-architected, total chaos system, multi-person teams are bullshit until such time as the system is properly architected. 
- Any of a plethora of technology aspects that are out pacing the original sources
    - Adapt the sources and do it now
- The insistence on multi-platform operability is a pipe dream
    - If some target platform poses significant impacts on the sources - someone fix the effing target platform.
        - The needs of the many outweigh the needs of the few

The fact that contemporary technologies weren't implemented in the past SHOULD NEVER hold an Open Source system back. 
These systems need to "Bite the Bullet" and get themselves updated already - those who are too lazy and/or insist on 
no changes because they are inconvenienced need to Get Over It or Get Out Of The Way.

My book will go into these and many more details about how software needs to be written, here are some, but by no means all, topics:

- Learn and understand at the deepest level of detail for the technology and keep up to date, and go back and improve when you realize you did it wrong the first time
    - Be incredibly knowledgeable about the technology to the point you've forgotten more than your co-workers will ever know.
However, don't talk about Object Orientation at the Christmas party.
- Do continuous refactoring all the time. If you look at your code in the future and you don't immediatly understand it, rewrite it until you do
- Do not comment code.
    - If you DO comment code, keep "We" out of it. There is no "We", it is "the system" **not** "We"
    - If your code has a comment - the correctness of the comment is <u>FAR MORE IMPORTANT</u> THAN THE CORRECTNESS OF THE CODE ITSELF. You **MUST** update 
any comment if even the **SLIGHTEST** change is made to code
        - The corallary to that is: DO NOT READ OR TRUST COMMENTS. They are **NOT** reality.
There is only **ONE** reality in software, that is how the instructions flow thru the CPU - there is <u>NO OTHER TRUTH</u>,
never trust comments, only trust code by DEBUGGING it or thorough understanding. If the comment is wrong DELETE IT WITHOUT QUESTION
- DO NOT COMMENT CODE. If you don't understand why, go to the above list item and read it slowly until you do.(see below)
- Keep the fucking tabs out of software. You can "say" that "our group uses x-spaces for tabs" until you're blue in the face. 
However, there is no more glaring proof that you could not care less about the visual appearance/readability of the sources.
I will be damned if I'll be changing my tab settings just to look at your code. Using "insert spaces" for tabs at least makes it look consistent regarding indents
- Don't even get me started on indenting 
- Also keep the fucking prettiness out of formatting. If I see another set of parameter declarations or variable names "lined up" I am going to absolutely puke.
I don't care about your artsy trite constructs with hieroglyphics invented out of ascii characters. If you put that crap in a book I want to read, I'd throw it in the
trash and demand my money back. It is **not** impressive and it is **not** cool.
- Never prepend variable names with some bullshit thing like "m_" who in the hell thought of that ??.
If you're class needs to have "reminders" about what's in it, maybe you should go back and revisit your design and/or actually try to understand your own software.
- Lot's more

### On the other hand about commenting

Take a look at sources in this repository that handle the PostScript operators. Currently there are two: 
[operatorsAtoL.cpp](./PostScriptInterpreter/Sources/operators/operators_AtoL.cpp), and [operatorsMtoZ.cpp](./PostScriptInterpreter/Sources/operators/operators_MtoZ.cpp).

First note that it is pretty obvious 1) what's in these files, and 2) where I need to go to find an operator given the first letter of it's name.

Next, note the methods therein that implement each of the operators. In every single one, I have scraped out the **actual** description (definition) of 
the operator from the PostScript reference book **as** the comments in the source for every operator.

**THAT** is commenting software. It is about precision in the description of what the software is doing from the absolute (and only) 
TRULY correct **and complete** reference. 
This is also a precise example of what I mean that the correctness of comments MUST be considered more important than the code itself, 
and because of that, it MUST come from a guaranteed truset source - NEVER hand written













