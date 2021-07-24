# Sudoku

Just a simple Sudoku puzzle solver in c. 

There are two methods of working with puzzles. 

## Browser interface
Simply run the script in with node <root>/server.js in a shell
In the browser goto the link presented in the shell. 

You can compile the *C* code directly from the browser with either the compile button, or just press *c*. 

## Command Line 
Compile the *c* code. 
The generated sudoku program expects an input of 81 numbers [0-9] where the number 0 is simply a placeholder for an empty tile in the puzzle. All other characters are ignored. 
EG: echo "123456789123456789123456789123456789123456789123456789123456789123456789123456789" | ./sudoku  
would work just fine. 

## There are plenty of precreated puzzles in ./puzzles

## Whats the point?
Just to toy around with code. So why not? 
There were a couple ideas I had for interesting ways in conceptualizing a pipleline. At this point what I have created that I think are interesting is a simple function queue, a very crude GC system, and the idea of generalized Lists. Beyond that. I wanted to create the Sudoku puzzle solver in a way that would be done as a human would solve the puzzle. So all algorithms are the concepts of how a person would methodically solve a puzzle. This is not related directly to what we would do in a CS aspect. So there is a lot of code with regard to solving the puzzles using different human methods. 

For me, the actual puzzle was secondary to my entertaining the idea of using a function queue can be generalized to take a function and environment, and just run it. So the puzzle itself was just an excuse to implement that alone. 

The function queue was actually simpler to create than I realized though. So ¯\\_(ツ)_/¯

The solvers are done intentionally in an procedural manner as a human would solve the puzzles. Eventually this should allow me to classify the difficulty of a given puzzle. Regardless. I am able to add solvers as I see fit. 
