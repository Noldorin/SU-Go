LET’S GO – A Computer AI Project by Dalena Le and Willie Gross 

Website: https://sugoproject.weebly.com 

Intro: 
----------------------
This Student Project uses a simplified version of the classic game Go to experiment with the following concepts: 

(1) Artificially Intelligent Agents

(2) Algorithmically Computed Performance Metrics 

(3) Shallow “Try & Backtrack” Patterns for determining AI choices 


The Basics: 
----------------------
The Driver for this program uses a very simple command line interface and text representation of the gameboard to allow the user and AI Agent to take turns placing stones until the user decides to quit. On their turn user can place a stone on any empty space by entering the (x,y) coordinates of that space on the board. The AI will then automatically run through its algorithmic decision process and place a stone in the place it determines is best based on its performance metric.  

An overview of how the AI Agent makes its choices is outlined on the website’s blog.  

There is no automatic score computation as this is a rather difficult and complicated problem to solve programmatically and accurately representing/automating the game of Go was not the purpose of this project. The user must manually score the game at the end (before deciding to quit) the same way they would when playing against a human opponent. 

More Details: 
----------------------
-Influence and AI Decision Making 

The AI Agent bases its decisions on a single performance metric called “Influence.” Each stone placed on the board exerts influence on the empty space surrounding it, exerting stronger influence on closer spaces, and less on spaces that are father away. Generally speaking, a color is considered by the Agent to “own” an empty space if that color has more stones closer to that space than the opposing color. This idea of influence may be somewhat blunt, but it strongly correlates with the primary goal of Go: to control and surround more of the board than your opponent. 

On each of its turns the Agent will “Try” to place a stone in an empty space, then calculate and store what its new total influence would be if it went there. It then backtracks and repeats this process for every empty space. It then picks the move from the list of moves it tried that yielded the highest total influence and chooses that as it’s move for that turn (picking pseudo-randomly in the event of a tie).

 In a game as complicated and nuanced as Go, no single performance metric will be enough to create an Agent that is thoroughly competent at playing it. However, the purpose of this project was not to create an AI that was good at Go, but simply to use the game as a medium for exploring a handful of fundamental AI Programming concepts. Therefore this blunt method of measuring performance is more than adequate for our purposes. 
 

Notes and Caveats: 
----------------------
There were a number of things we discovered and considered in the process of making this project, but did not have the time to implement, fix, or explore. They are listed here for your consideration. 


PERFORMANCE 
----------------------
CalculateInfluence() and the way Influence is stored and updated is extremely inefficient in the way it’s implemented. As a result, running the program for larger boards becomes very slow very fast. This was due to us simply brute-forcing the algorithm to get something up and running as soon as we could. We have since come up with a straightforward way to make the algorithm far faster, but do not have the time to implement and debug it and get it up to speed, but here it is to show we thought of it: 
 
Instead of recalculating the influence exerted by every stone on the board every turn and having separate influence values for black and white for each space, we would have a single influence value. 
This value would get added to by black influence, and subtracted from by white influence, if it was positive, it would be owned by black and vice versa.  

This method of storing influence meta-data would allow us to only compute the exerted influence of the newly place stone each turn, updating the previous influence Value for each space it influenced, instead of resetting all influence values to zero and then re-calling ExertInfluence() for both the new stone and all pre-existing stones. This would lower the complexity of CalculateInfluence by several orders of magnitude 


CAPTURING GROUPS:
----------------------
Unlike “real” Go, this game does now allow the capturing of groups of stones. This was an early oversight when we were rushing to get a prototype up and running as it is a relatively complex calculation. We have since determined a method for removing the, but similar to the performance optimization do not have enough time to reliably implement and debug it. However, we are confident in it and have listed it below: 
 
Each stone would have a new attribute called “Visited” which stores a bool indicated if that space has been checked for stone removal this turn, and RemoveCapturedStone would hold a list of stones to be removed. 

Each turn when a new stone was placed, RemoveCapturedStone would be called for each stone on the board. However instead of simply checking each stone’s immediate neighbors to see if it is surrounded by enemy stones and removing it if needed, it would perform the following recursive algorithm 

Check if it is fully surrounded by some combination of board edges and stones of either color AND at check if at least one of its immediate neighbors is a stone of the opposing color; only the very first call will check for this second condition since stones in the middle of a captured group might be completely surrounded by empty stone. If so it marks itself as visited, adds itself to a “removal”, and then recurs on each of its neighbors that contain a stone of its color. 

This recursion will perform the same checks on all contiguous neighbors of the same color until one of two things happen: 

It runs out of unvisited contiguous neighbors of the same color to iterate on without encountering any empty spaces. In which case it will terminate and remove all stone in the Removal List. 

It encounters an empty space. If this happens it means the stone/group of stones is not fully surrounded by the enemy and there-for is not captured. Whichever recursive call first encounters an empty neighbor will mark itself as visited, EMPTY the RemovalList and then LOCK the removal list to stop any subsequently visited stone in this group from adding themselves to the removal list. 

TIE BREAKERS & OTHER AGENT IMPROVEMENTS: 
-------------------------------------------
As mentioned above, building an AI that was great at playing Go was not the primary purpose of this project, as any nuanced metrics geared towards making it better at specific aspects of the game would take an enormous amount of playtesting and tweaking which wasn’t within the scope of a 10 week project. 

That being said we certainly had a few ideas of things that could have made the AI more challenging to play against, and one big category of improvements we defined was determining what to do in the case of ties between the influence scores of multiple possible moves the Agent could make each turn. Here are some possible tie-breakers we could have explored given more time, each alone, or perhaps combined together and weighted. 
 
When faced with a tie: 

(1) Choose the space closest to existing friendly stones

(2) Choose the space closest to existing enemy stones 

(3) Choose the space farthest from the board edge 

(4) Recur one level deeper, running the Try & Backtrack algorithm for the board-states created by each of the tied potential moves. 

(5) Repeat #4 a set number of times or until there is no tie, whichever comes first. 
