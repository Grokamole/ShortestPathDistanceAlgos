# ShortestPathDistanceAlgos
Fully recursive DFS vs. BFS shortest path distance algorithms. 

Includes compilation options for:
    GNU g++ (using makefile)
	Visual Studio 2017 (using VS2017 solution)

Allows the user to select a text map file and get the shortest path using depth first and breadth first search options from a starting X,Y coordinate.

Map file data is as follows:
First line should be a pair of numbers separated with a comma (e.g., 8,4 for 8 columns and 4 rows).
Then the map structure should follow with 0's being clear, 1's being blocked spaces, and 2's being exits.

Look at the included map files for examples.

Note 1: 3.txt is a map file that may not include an exit.
Note 2: If the starting space is a blocked space, the algorithms will ignore that option.

Joseph Miller -- Copyright (c)2019