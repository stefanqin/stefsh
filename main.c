/*stefsh main function*/
#include "main.h"

/*Shell functions*/
extern int stefshLoad(void);
extern void stefshLoop(void);
extern int stefshTerminate(void);

int main(void){

  stefshLoad();
  stefshLoop();
  stefshTerminate();

  return 0;
}
