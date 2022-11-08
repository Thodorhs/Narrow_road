/*Thodorhs Pontzouktzidhs
csd4336@csd.uoc.gr*/

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BLU "\x1B[34m"
#define RED "\033[1;31m"
#define WHT "\033[0m"

time_t t;
/**
 * semaphore,mutexes
 * intialization.
 */
sem_t phases[3];
pthread_mutex_t lock;
pthread_mutex_t lock1;
pthread_mutex_t lock2;

/*global hold of thread-walkers amount.*/
int people;

/*used to fill road in the begining*/
int currwest=1;
int currreast=0;

/*used in @set_priorities() to set @walker.priority of everyone*/
int bluecount=0;
int redcount=0;
int goingwest=0;
int goingeast=0;

/*walker struct: @arg of thread routine*/
struct something{
    int id;
    int index;
    enum{WEST,EAST} direction;
    enum{red,blu} color;
    enum{first,second,third,fourth} priority;
}typedef walkers;

/**
 * road,pave arrays init, 
 * DISCLAIMER: I need referance to memmory of each walker so i can mess with the from each thread.
 */
walkers** road;
walkers** pave;

void print_walkers(){
int j;
printf("|| ");
    for(j=0;j<people;j++){
        if(pave[j]!=NULL){
            if(pave[j]->color==red){
                printf(RED);
            }else{
                printf(BLU);
            }
            if (pave[j]->direction==EAST){
                printf("%d>",pave[j]->id);
            }else{
                printf("<%d",pave[j]->id);
            }
            printf(WHT);
            printf(" || ");
        }else{
            printf("-1");
            printf(" || ");
        }
    }
printf("\n");

printf("|| ");
    for(j=0;j<people;j++){
        if(road[j]!=NULL){
            if(road[j]->color==red){
                printf(RED);
            }else{
                printf(BLU);
            }
            if (road[j]->direction==EAST){
                printf("%d>",road[j]->id);
            }else{
                printf("<%d",road[j]->id);
            }
            printf(WHT);
            printf(" || ");
        }else{
            printf("-1");
            printf(" || ");
        }
    }
printf("\n==============================================================\n");
return;
}

void set_priorities(){
int j,bestc,bestd;
    if(bluecount>=redcount){
        bestc=1;
    }else{
        bestc=0;
    }
    if(goingeast>=goingwest){
        bestd=1;
    }else{
        bestd=0;
    }
    /*now that we found dominant color and direction lets set the priorities of each walker*/
    for(j=0;j<people;j++){
        if(bestc==road[j]->color&&bestd==road[j]->direction){
            road[j]->priority=first;
        }else if(bestc==road[j]->color&&bestd!=road[j]->direction){
            road[j]->priority=second;
        }else if(bestd==road[j]->direction&&bestc!=road[j]->color){
            road[j]->priority=third;
        }else if(bestd!=road[j]->direction&&bestc!=road[j]->color){
            road[j]->priority=fourth;
        }else{
            road[j]->priority=fourth;
        }
    }
return;
}

int check_road(){
int j,count=0;
    for(j=0;j<people;j++){
        if(road[j]==NULL){
            count++;
        }
    }
    if(count==people){
        return 1;
    }
return 0;
}

int check_for_next_priority(){
int j,min=0;
    for(j=0;j<people;j++){
        if(pave[j]){
            min=pave[j]->priority;
        }
    }
    for(j=0;j<people;j++){
        if(pave[j]&&min>pave[j]->priority){
            min=pave[j]->priority;
        }
    }
return min-1;
}

void * simulation(void * walker){
walkers *a=(walkers*) walker;
int i,temprior;
    /*if not first priority go to the pavement and wait*/
    if(a->priority!=first){
        pave[a->index]=a;
        road[a->index]=NULL;
        /*printf("id %d is waiting\n",a->id);*/
        sem_wait(&phases[a->priority-1]);/*who ever wakes up from here can walk so go in the road...*/
        /*printf("id %d started\n",a->id);*/
        pthread_mutex_lock(&lock1);
        road[a->index]=a;
        pave[a->index]=NULL;
        pthread_mutex_unlock(&lock1);
    }

    if(a->direction==WEST){
        while(a->index!=0){
            pthread_mutex_lock(&lock2);
            if(road[a->index-1]==NULL){
                /*printf("i lock %d priority %d\n",a->id,a->priority);*/
                road[a->index-1]=a;
                road[a->index]=NULL;
                a->index--;
                sleep(1);
                print_walkers();
                fflush(stdout);
            }
            pthread_mutex_unlock(&lock2);
            
        }
    }else{
        while(a->index!=people-1){
            pthread_mutex_lock(&lock2);
            if(road[a->index+1]==NULL){
                /*printf("i lock %d priority %d\n",a->id,a->priority);*/
                road[a->index+1]=a;
                road[a->index]=NULL;
                a->index++;
                sleep(1);
                print_walkers();
                fflush(stdout);
            }   
            pthread_mutex_unlock(&lock2);
            
        }
    }
    pthread_mutex_lock(&lock2);
    /*printf("im %d out\n",a->id);*/
    road[a->index]=NULL;
    if(a->priority!=fourth&&check_road()){ /*1.there is no point for the last priority to post 2.only the last thread on the road can wake up the next priority (check_road())*/
        /*if anyone of the next priority is in the pavement wake him up*/
        temprior=check_for_next_priority();
        /*printf("new priority %d\n",temprior);*/
            for(i=0;i<people;i++){
                sem_post(&phases[temprior]);
            }
    }
    pthread_mutex_unlock(&lock2);
free(a);
return NULL;
}

int main(int argc, char *argv[]){
int i,j;

pthread_t *ptr;
walkers* walker;

if(argc!=2){
    printf("Error: only one argument allowed\n");
    return 1;
}

people=atoi(argv[1]);
if(people>100){
    printf("Error: max value of argument is 100\n");
    return 1;
}

srandom((unsigned) time(&t));

sem_init(&phases[0], 0, 0);
sem_init(&phases[1], 0, 0);
sem_init(&phases[2], 0, 0);

if (pthread_mutex_init(&lock1, NULL) != 0) {
    printf("\n mutex init has failed\n");
    return 1;
}
if (pthread_mutex_init(&lock2, NULL) != 0) {
    printf("\n mutex init has failed\n");
    return 1;
}

road=(walkers**)calloc(people,sizeof(walkers*));
pave=(walkers**)calloc(people,sizeof(walkers*));
ptr=(pthread_t*)malloc(people*sizeof(pthread_t));

    for(i=0;i<people;i++){
        walker=malloc(sizeof(walkers));
        walker->id=i;
        
        if(random() % 2){
            bluecount++;
            walker->color=blu;
        }else{
            redcount++;
            walker->color=red;
        }

        walker->direction=random() % 2;

        if(walker->direction==EAST){
            goingeast++;
            walker->index=currreast;
            road[currreast]=walker;
            currreast++;
        }else{
            goingwest++;
            walker->index=people-currwest;
            road[people-currwest]=walker;
            currwest++;
        }     
    }

set_priorities();
print_walkers();
printf("\nred=%d blue=%d\n",redcount,bluecount);
printf("east=%d west=%d\n",goingeast,goingwest);

    for(i=0;i<people;i++){
        if(pthread_create(&ptr[i], NULL, simulation, road[i]) != 0){
            printf("Error making thread\n");
            return 1;
        }
    }

    for(i=0;i<people;i++){
        if (pthread_join(ptr[i],NULL) != 0 ){
            printf("Error joining thread\n");
            return 1;
        }
    }

sem_destroy(&phases[0]);
sem_destroy(&phases[1]);
sem_destroy(&phases[2]);
pthread_mutex_destroy(&lock1);
pthread_mutex_destroy(&lock2);
print_walkers();
free(road);
free(pave);
free(ptr);
free(walker);

return 0;
}