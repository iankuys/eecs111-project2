#include "p2_threads.h"
#include "utils.h"

using namespace std;

extern pthread_cond_t  cond;
extern pthread_mutex_t mutex;

pthread_mutex_t waitingMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t enterMutex = PTHREAD_MUTEX_INITIALIZER;

bool overtime = false;
extern Restroom restroom = Restroom();
extern struct timeval t_global_start;
vector<Person> waitingQueue;

void *threadfunc(void *parm)
{
	int status;
	printf(" [Thread] Start\n");

	printf(" [Thread] Locks\n");
	status = pthread_mutex_lock(&mutex);

    printf(" [Thread] Blocked\n");
    status = pthread_cond_wait(&cond, &mutex);

	printf(" [Thread] Starts again.\n");
	for (int i=0; i<3; i++) {
		printf(" [Thread] Complete thread after (%d) seconds\n", (3-i));
		usleep(MSEC(1000));
	}

	printf(" [Thread] Unlocks\n");
	status = pthread_mutex_unlock(&mutex);
	printf(" [Thread] Complete\n");
}

void* maleThread(void* param) {
    Person person;
    person.set_gender(0);
    
    struct timeval currentTime, inlineTime;
    pthread_mutex_lock(&waitingMutex);
    restroom.add_person(person);
    waitingQueue.push_back(person);
    pthread_mutex_unlock(&waitingMutex);
    
    long waitingTime;
    bool entered = false;
    bool exited = false;

    gettimeofday(&inlineTime, NULL);
    while (true) {
        gettimeofday(&currentTime, NULL);
        waitingTime = get_elasped_time(inlineTime, currentTime);
        if (waitingTime > 30) {
            pthread_mutex_lock(&enterMutex);
            while (true) {
                pthread_mutex_lock(&waitingMutex);
                if (restroom.cleared_to_enter(person)) {
                    restroom.man_wants_to_enter(person);
                    entered = true;
                }
                pthread_mutex_unlock(&waitingMutex);
                if (entered) {
                    pthread_mutex_unlock(&enterMutex);
                    break;
                }
            }
        } else {
            pthread_mutex_lock(&enterMutex);
            pthread_mutex_lock(&waitingMutex);
            if (restroom.cleared_to_enter(person)) {
                restroom.man_wants_to_enter(person);
                entered = true;
            }
            pthread_mutex_unlock(&waitingMutex);
            pthread_mutex_unlock(&enterMutex);
        }
        if (entered) {
            break;
        }
        usleep(MSEC(0.5));
    }
    
    while (true) {
        pthread_mutex_lock(&enterMutex);
        pthread_mutex_lock(&waitingMutex);
        if (person.ready_to_leave()) {
            restroom.man_leaves(person);
            exited = true;
        }
        pthread_mutex_unlock(&waitingMutex);
        pthread_mutex_unlock(&enterMutex);
        if (exited) {
            break;
        }
        usleep(MSEC(0.5));
    }

    pthread_exit(0);
}

void* femaleThread(void* param) {
    Person person;
    person.set_gender(1);
    
    struct timeval currentTime, inlineTime;
    pthread_mutex_lock(&waitingMutex);
    restroom.add_person(person);
    waitingQueue.push_back(person);
    pthread_mutex_unlock(&waitingMutex);
    
    long waitingTime;
    bool entered = false;
    bool exited = false;

    gettimeofday(&inlineTime, NULL);
    while (true) {
        gettimeofday(&currentTime, NULL);
        waitingTime = get_elasped_time(inlineTime, currentTime);
        if (waitingTime > 30) {
            pthread_mutex_lock(&enterMutex);
            while (true) {
                pthread_mutex_lock(&waitingMutex);
                if (restroom.cleared_to_enter(person)) {
                    restroom.woman_wants_to_enter(person);
                    entered = true;
                }
                pthread_mutex_unlock(&waitingMutex);
                if (entered) {
                    pthread_mutex_unlock(&enterMutex);
                    break;
                }
            }
        } else {
            pthread_mutex_lock(&enterMutex);
            pthread_mutex_lock(&waitingMutex);
            if (restroom.cleared_to_enter(person)) {
                restroom.woman_wants_to_enter(person);
                entered = true;
            }
            pthread_mutex_unlock(&waitingMutex);
            pthread_mutex_unlock(&enterMutex);
        }
        if (entered) {
            break;
        }
        usleep(MSEC(0.5));
    }
    
    while (true) {
        pthread_mutex_lock(&enterMutex);
        pthread_mutex_lock(&waitingMutex);
        if (person.ready_to_leave()) {
            restroom.woman_leaves(person);
            exited = true;
        }
        pthread_mutex_unlock(&waitingMutex);
        pthread_mutex_unlock(&enterMutex);
        if (exited) {
            break;
        }
        usleep(MSEC(0.5));
    }

    pthread_exit(0);
}