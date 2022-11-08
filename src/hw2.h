/*Thodorhs Pontzouktzidhs
csd4336@csd.uoc.gr*/

/**
 * @brief Prints road and pavement.
 * 
 */
void print_walkers();

/**
 * @brief Set the priorities of every walker based on his color and direction.
 * 
 */
void set_priorities();

/**
 * @brief Check if anyone is still on the road.
 * 
 * @return int 1 if someone is on the road, 0 if none is.
 */
int check_road();

/**
 * @brief check for the next minimum priority in the pavement.
 * 
 * @return int, the next minimum priority in the pavement.
 */
int check_for_priorities(int p);

/**
 * @brief Routine of every thread, simulates every person ( @param walker )
 * walking in the narrow road following the rules.
 * 
 * @param walker struct something. 
 * @return void* 
 */
void * simulation(void * walker);