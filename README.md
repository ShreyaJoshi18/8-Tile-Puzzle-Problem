# 8-Tile-Puzzle-Problem


## Overview

8-Tile Puzzle is a project that will solve the very famous 8 -Tile Puzzle Problem using Heuristic and Non-Heuristic approaches and analyze the Distinction of Complexities among them. 


## What is 8 Puzzle??

8 puzzle problem uses a 3 by 3 grid board with eight inlaid square sliding pieces marked with values 1 through 8 along with a blank tile. The blank tile can move one step at a time. The aim of this puzzle is to arrange the numbers in the grid according to the desired output with the help of legal moves.

8-puzzle problem is the largest possible N-puzzle problem that could be solved completely. The general extension of this 8-puzzle is the N-puzzle which is an np-hard problem.

 ![puzzle8init](https://user-images.githubusercontent.com/49505029/128621431-80e50009-01ea-4cf6-8bb3-c10a97e76741.jpg)


## Methodology

In general, a typical 8-puzzle problem may take about 20 steps to solve, but it highly depends on the initial state of the problem. The movement of blank tile depends on its position: four possible moves if it is in middle, two possible moves if it is in corner and three along the edge, which makes the branching factor approximately 3.

●	The Algorithms which we have used are: BFS (non-heuristic), A*(heuristic) and Branch and Bound (heuristic). It is to show the comparison between heuristic and non-heuristic approaches to solve the problem.

●	BFS algorithm traverse a tree in a breadth wise motion and uses queue to remember to get the next vertex to start a search. This algorithm selects a single node (initial or source point) and then visits all the nodes adjacent to the selected node. Once the algorithm visits and marks the starting node, then it moves towards the nearest unvisited nodes and analyses them.

●	A* Search Algorithm at each step picks the node according to a value f, which is a parameter equal to the sum of two other parameters g and h. At each step, it picks the node having the lowest f and process that node. Here g is the movement cost to move from the starting point to a given square on the grid, following the path generated to get there and h is the estimated movement cost to move from that given square on the grid to the final state.

●	Branch and Bound heuristic algorithm depends on upper bound and lower bound regions of search space. There are three types of nodes:
  **<p align="center">
                1.	Live node: It is a node that has been generated but whose children have not yet	been	generated. </p>**
          
   **<p align="center"> 
                2. Node: It is a live node whose children are currently being explored. In other words, an E-node is a node currently being expanded.</p>**
    
  **<p align="center">
                3.	Dead node: It is a generated node, that is not to be expanded or explored any further. All children of a dead node have already been expanded.</p>**

●	Each node X in the search tree is associated with a cost. The cost function is useful for determining the next E-node. The next E-node is the one with the least cost. The cost function is defined as:
                                                                  **c(x) = f(x) + h(x)**,
	where f(x) is the length of the path from root to x (the number of moves so far) and h(x) is the number of non-blank tiles not in their goal position (the number of misplaced tiles). There are at least h(x) moves to transform state x to a goal state.



## Results and Analysis

![2021-08-08](https://user-images.githubusercontent.com/49505029/128621260-cd3ff2ff-6010-45bf-bf21-1f5e4a6ea538.png)
