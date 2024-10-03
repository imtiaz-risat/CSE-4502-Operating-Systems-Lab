#include <lib/debug.h>
#include <lib/x86.h>
#include "import.h"

// Define the maximum number of containers/processes the system can support
// #define NUM_IDS 256

// Define the maximum number of children a single container can have
// #define MAX_CHILDREN 16

struct SContainer {
    int quota;      // maximum memory quota of the process
    int usage;      // the current memory usage of the process
    int parent;     // the id of the parent process
    int nchildren;  // the number of child processes
    int used;       // whether current container is used by a process
};

// mCertiKOS supports up to NUM_IDS processes
static struct SContainer CONTAINER[NUM_IDS];

/**
 * Initializes the container data for the root process (the one with index 0).
 * The root process is the one that gets spawned first by the kernel.
 */
void container_init(unsigned int mbi_addr)
{
    unsigned int real_quota;
    // TODO: define your local variables here.
    unsigned int available_quota = 0;
    unsigned int total_pages = get_nps();

    pmem_init(mbi_addr);
    real_quota = 0;

    /**
     * TODO: Compute the available quota and store it into the variable real_quota.
     * It should be the number of the unallocated pages with the normal permission
     * in the physical memory allocation table.
     */
    for(int i = 0; i < total_pages; i++) {
        if(at_is_norm(i) && at_is_allocated(i)==0){
            available_quota++;
        }
    }
    real_quota = available_quota;

    KERN_DEBUG("\nreal quota: %d\n\n", real_quota);

    // setting up the root container with a quota equal to the total number of available pages
    CONTAINER[0].quota = real_quota;    
    CONTAINER[0].usage = 0;
    CONTAINER[0].parent = 0;
    CONTAINER[0].nchildren = 0;
    CONTAINER[0].used = 1;
}

// Get the id of parent process of process # [id].
unsigned int container_get_parent(unsigned int id)
{
    // TODO
    return CONTAINER[id].parent;
}

// Get the number of children of process # [id].
unsigned int container_get_nchildren(unsigned int id)
{
    // TODO
    return CONTAINER[id].nchildren;
}

// Get the maximum memory quota of process # [id].
unsigned int container_get_quota(unsigned int id)
{
    // TODO
    return CONTAINER[id].quota;
}

// Get the current memory usage of process # [id].
unsigned int container_get_usage(unsigned int id)
{
    // TODO
   return CONTAINER[id].usage;
}

// Determines whether the process # [id] can consume an extra
// [n] pages of memory. If so, returns 1, otherwise, returns 0.
unsigned int container_can_consume(unsigned int id, unsigned int n)
{
    // TODO
    // Checks if a container can consume additional resources (pages) without exceeding its quota
    if(CONTAINER[id].usage + n <= CONTAINER[id].qouta){
        return 1;
    }else{
        return 0;
    }
}

/**
 * Dedicates [quota] pages of memory for a new child process.
 * You can assume it is safe to allocate [quota] pages
 * (the check is already done outside before calling this function).
 * Returns the container index for the new child process.
 */
unsigned int container_split(unsigned int id, unsigned int quota)
{
    unsigned int child, nc;

    nc = CONTAINER[id].nchildren;
    child = id * MAX_CHILDREN + 1 + nc;  // container index for the child process

    if (NUM_IDS <= child) {
        return NUM_IDS;
    }

    /**
     * TODO: Update the container structure of both parent and child process appropriately.
     */

    return child;
}

/**
 * Allocates one more page for process # [id], given that this will not exceed the quota.
 * The container structure should be updated accordingly after the allocation.
 * Returns the page index of the allocated page, or 0 in the case of failure.
 */
unsigned int container_alloc(unsigned int id)
{
    /*
     * TODO: Implement the function here.
     */
    if (CONTAINER[id].usage < CONTAINER[id].quota) {
        unsigned int page_index = pmem_alloc();

        if (page_index != 0) {
            CONTAINER[id].usage++;
            return page_index;
        }
    }
    return 0;


    return 0;
}

// Frees the physical page and reduces the usage by 1.
void container_free(unsigned int id, unsigned int page_index)
{
    // TODO
    if (at_is_allocated(page_index)) {
        pmem_free(page_index);
        CONTAINER[id].usage--;
    }
}
