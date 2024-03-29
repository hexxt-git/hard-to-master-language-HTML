# hard to master language (HTML)
is an interpreted programming language made in C. it offers simple instructions that *should be turing complete* so you could make anything in it, the whole thing was made in one night.

feel free to contribute to this with code examples, instructions or whatever you like.
## how to use
### building
the github repo comes with the built file ready but if you still want to build yourself. there are no dependencies, just use
```bash
gcc interpreter.c -o interpreter
```
### running
run the built file with the link to your .html file as an argument
```bash
./interpreter /link/to/file.html
```

## instruction set
- psh  int -> pushes a number to the top of the stack
- pop      -> removes the top number from the stack
- add      -> adds up   the top two numbers of the stack
- sub      -> subtracts the top two numbers of the stack
- mul      -> multiplies the top two numbers of the stack
- div      -> divides the top two numbers of the stack (returns divistion and modulo)
- ife      -> only does the next instruction if the top two elements are equal
- ifn      -> only does the next instruction if the top two elements are not equal
- ifg      -> only does the next instruction if the top element is greater than the second
- ifl      -> only does the next instruction if the top element is less than the second
- jmp  int -> jumps the program to a line
- jml  lbl -> jumps the program to a label
- lbl:     -> ends the previous label and begins a new one
- hlt      -> halts the label
- red      -> reads and int input
- prt      -> writes the top int of the stack to termainal
- wrt      -> writes the top int of the stack as character to termainal
- log  msg -> writes a message to termainal
- pss      -> does nothing


## instructions
html programs start at main label and end at mains hlt or the end of the file.

when a label is called the program will jump to the label and continue from there.

you must hlt out of a label otherwise the program will continue to the next label.

when you hlt out of a label the program will continue where it left off before jml was called.

## the stack
the stack is a list of integers that the program can push and pop from.
