**lwcs** is as **L**ight**w**eight **C**ooperative **S**cheduler for short.

##### Pros
* lightweight
* platform independent
* easy to use
* no dynamic memory allocation

##### Cons
* non-preemtive
* non Real-Time
* easy to break up things

**lwcs** is a portable, platform independent **non-preemtive** multitasking 
process sheduler. **Cooperative** means that no context switching is used, but
instead, each process **must** voluntary yield control one to another and work
in cooperation with each other. Process workflow is controlled by shceduler busy
loop. This *feature* gives the biggest problem of cooperative sheduler - each
process should not be blocking and do not use busy waiting on it own,
state-machines could be used instead, if action on different events is required
inside the process.

##### Usage example
Simply put `lwcs.h` and `lwcs.c` files in your working directory.

!!! **Make sure every task is simple fast and non-blocking.**

```c
#include <led.h>
#include "lwcs.h"

static const uint32_t jiffiesInMilliSec = 1;
extern lwcs_Time getJiffies(void);

static const lwcs_Time BLINK_RED_LED_PERIOD = 500;
static const lwcs_Time BLINK_GREEN_LED_PERIOD = 900;

int blinkLedForever(void);
int blinkLedOnce(void);

void main(void) {
    lwcs_initialize(jiffiesInMilliSec, &getJiffies);

    lwcs_addTask(&blinkRedLedForever, BLINK_RED_LED_PERIOD);
    lwcs_addTask(&blinkGreenLedOnce, BLINK_GREEN_LED_PERIOD);

    lwcs_run();
}

int blinkLedForever() {
    LED_invert(LED_RED);

    // if task returns 0 then task is preserved in scheduler run list
    return 0;
}

int blinkLedOnce() {
    LED_invert(LED_GREEN);

    // if task returns -1 then task is removed from scheduler run list
    return -1;
}
```
