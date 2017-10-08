#include "intlist.h"
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#define ONE_MILLION 1000000.0
#define TO_SECONDS(T) (T).tv_sec + ((T).tv_usec)/ONE_MILLION



/* needs to be a global variable for my linked list so that dofree() has 
 * access to it 
 */
struct int_list list;

/**
 *  signal handler that cleans up memory
 **/
void dofree(int sig)
{
    /* reset the signal handler to the default handler for the signal being
     * handled. This is to prevent this function being recursively being 
     * called such as by a segmentation fault in freeing the list 
     */
    struct sigaction default_action;
    memset(&default_action, 0, sizeof(struct sigaction));
    default_action.sa_handler = SIG_DFL;
    sigaction(sig, &default_action, NULL);
    /* free the intlist */
    intlist_free(&list);
    /* all good, let's quit the program */
    exit(EXIT_SUCCESS);
}

int main(void)
{
    int count;
    struct sigaction act;

    /* initialise the signal handler data structure and register the signals
     * it needs to handle. I have done this so my program cleans up its own
     * memory on exit in all events. I know there is a potential recursive 
     * call to dofree() on segmentation fault so maybe I should remove 
     * SIGSEGV from the list
     */
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = dofree;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGABRT, &act, NULL);
    sigaction(SIGSEGV, &act, NULL);
    count = 0;
    /* we are going to repeatedly allocate and free the list and do various 
     * analyses on these runs to look at the properties of the memory 
     * management strategies
     */
    while(true)
    {
        /* keep track of how many times we have filled the list - let's look
         * at how the time for each iteration differs
         */
        struct rusage usage;
        struct timeval begin, end, duration;
        struct timeval alloc_begin, alloc_end, alloc_duration;
        struct timeval free_begin, free_end, free_duration;

        /* start the overall timer */
        getrusage(RUSAGE_SELF, &usage);
        begin = usage.ru_utime;

        printf("iteration number %d\n", ++count);

        /* initialise the int list */
        intlist_init(&list);

        /* start timing the memory allocation */

        getrusage(RUSAGE_SELF, &usage);
        alloc_begin = usage.ru_utime;
        intlist_fill(&list);

        /* end the timing of the memory allocation */
        getrusage(RUSAGE_SELF, &usage);
        alloc_end = usage.ru_utime;
        timersub(&alloc_end, &alloc_begin, &alloc_duration);
        printf("allocation took %f seconds.\n", TO_SECONDS(alloc_duration));

        /* begin the timing of the freeing of the memory */
        getrusage(RUSAGE_SELF, &usage);
        free_begin = usage.ru_utime;
        intlist_free(&list);

        /* finish the timing of the freeing of memory */
        getrusage(RUSAGE_SELF, &usage);
        free_end = usage.ru_utime;
        timersub(&free_end, &free_begin, &free_duration);
        printf("time to free: %f\n", TO_SECONDS(free_duration));

        /* end the timing of the overall algorithm */
        getrusage(RUSAGE_SELF, &usage);
        end = usage.ru_utime;
        timersub(&end, &begin, &duration);
        printf("This run took %f seconds.\n",  TO_SECONDS(duration));
    }
    return EXIT_SUCCESS;
}
