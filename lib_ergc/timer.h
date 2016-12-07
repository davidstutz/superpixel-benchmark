#ifndef __TIMER_H_
#define __TIMER_H_
#include <sys/time.h>

struct timeval tbegin, tend;

void tic() {
  gettimeofday(&tbegin,NULL);
}

double toc() {
  gettimeofday(&tend,NULL);
  return(((double)(1000*(tend.tv_sec-tbegin.tv_sec)+((tend.tv_usec-tbegin.tv_usec)/1000)))/1000.);
}




#endif
