#ifndef __OPERATIONS_H__
#define __OPERATIONS_H__

/* 
 * volatile variables to be used by threads
 * as shared memory during execution time
 */
extern volatile char *pipe_array;
extern volatile int read_pos, write_pos, pipe_size;

void pipe_init(int size);
void pipe_write(char c);
void pipe_writeDone();
int pipe_read(char *c);
void pipe_destroy();

#endif