#ifndef __MYSEM_H__
#define __MYSEM_H__

/* Create a semaphore with init_num as the initial value */
void mysem_create(volatile int *sem_id, int init_num);

/* Decrease the semaphore value by 1
   If the value is already 0 the calling thread will be blocked */
void mysem_down(int sem_id);

/* Increase the semaphore value by 1
   If the semaphore is already 1 an error message will be printed in
   the standard output an the function will return.
   The function gets as argument the line that is was called for debugging purposes */
int mysem_up(int sem_id, const int line);

/* Destroy the semaphore */
void mysem_destroy(int sem_id);

/* Helper function for assignment2 */
int is_sleeping(int sem_id);

#endif
