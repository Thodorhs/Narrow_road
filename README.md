# Narrow road simulation
*cs345 course - Computer Science Department - University Of Crete*

# Brief explanation
    1.One road and one pavement.
    2.Red - blue walkers.
    3.Walkers direction east - west.
    5.Highest occuring color and direction is saved (dominant color - dominant direction).
    4.First: Dominant Color and direction goes first (everyone else goes on the pavement and waits).
    5.Second: The walkers of the same color and dominant direction go on the road.
    6.Third: Walkers of the next color and dominant direction go in the road.
    7.Fourth: The remaining walkers go in the road.

# Prerequisites
- Make
- gcc

# How to run
Run **./run.sh <amount of walkers - threads>** in the main directory.
