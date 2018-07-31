# akeum-compiler-c
akeum (악음) is my attempt at reviving Choon, a Turing complete esoteric programming language made by Stephen Sykes back in 2002. More information about Choon is found on this [web archive](https://web.archive.org/web/20160316172205/http://www.stephensykes.com/choon/choon.html).

In the words of the original creator himself, he describes the language as:

> Choon's output is music - you can listen to it. And Choon gets away without having any conventional variable storage by
> being able to access any note that has been played on its output. One feature of musical performance is that once you have
> played a note then that's it, it's gone, you can't change it. And it's the same in Choon. Every value is a musical note,
> and every time a value is encountered in a Choon program it is played immediately on the output.

As of the latest commit, all of the Choon's instruction set has been implemented.

## Instruction set

WIP. For now, refer to this [web archive](https://web.archive.org/web/20160316172205/http://www.stephensykes.com/choon/choon.html).
Add `|G0-4` at the start of the program or `|G` at the start of each line if you want to test out Akeum with the same configuration as Choon.

## Additional features implemented

* Time Signature
* Clefs

## Planned to be implemented

* Thread Support (extension to the clefs implementation)
* Command Line Arguments (using key signatures)
* Many more to come
