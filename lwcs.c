/**
 * @file   lwcs.c
 * @brief  lightweight cooperative scheduler implementation file
 * @author ShpakovDmitry
 * @date   2020-12-25
 */

#include <stddef.h>
#include "lwcs.h"

/**
 * LWCS_MAX_TASKS is a amount of maximal tasks and is fixed at compile time
 * set to any number according to memory size available
 */
#define LWCS_MAX_TASKS 25

typedef struct {
    lwcs_Task task;
    lwcs_Pid pid;
    lwcs_Time period;
    lwcs_Time lastRun;
    lwcs_Time nextRun;
} lwcs_TaskDescriptor;

static const lwcs_TaskDescriptor lwcs_emptyTask = {
    .task = NULL,
    .pid = -1,
    .period = 0,
    .lastRun = 0,
    .nextRun = 0
};

static volatile lwcs_Time shedTime;
static lwcs_TaskDescriptor taskTable[LWCS_MAX_TASKS] = { 0 };
static uint32_t ticksInMilisec;
static void (*disableIRQ)(void);
static void (*enableIRQ)(void);

static lwcs_Pid addTaskGeneric(lwcs_TaskDescriptor taskDescriptor);
static int findFreeSpaceInTaskTable(void);
static int findPidInTaskTable(lwcs_Pid pid);

void lwcs_initialize(uint32_t jiffiesInMilliSec,
                     lwcs_Time currTime,
                     void (* disableTickInterrupt)(void),
                     void (* enableTickInterrupt)(void)
                     ) {
    ticksInMilisec = jiffiesInMilliSec;
    shedTime = currTime;
    disableIRQ = disableTickInterrupt;
    enableIRQ = enableTickInterrupt;
}

void lwcs_tickTime() {
    shedTime++;
}

lwcs_Pid lwcs_addTask(lwcs_Task task, lwcs_Time period) {
    lwcs_TaskDescriptor taskToAdd;
    taskToAdd = lwcs_emptyTask;
    taskToAdd.task = task;
    taskToAdd.period = period * ticksInMilisec;
    return addTaskGeneric(taskToAdd);

}

lwcs_Pid lwcs_runTaskAt(lwcs_Task task, lwcs_Time time) {
    lwcs_TaskDescriptor taskToAdd;
    taskToAdd = lwcs_emptyTask;
    taskToAdd.task = task;
    taskToAdd.nextRun = time;
    return addTaskGeneric(taskToAdd);
}

int lwcs_deleteTask(lwcs_Pid pid) {
   int i;
   i = findPidInTaskTable(pid);
   if ( i == -1 ) {
        return -1;
   }
   lwcs_TaskDescriptor* task = &taskTable[i];
   *task = lwcs_emptyTask;

   return 0;
}

int lwcs_changeTaskPeriod(lwcs_Pid pid, lwcs_Time period) {
    int i;
    i = findPidInTaskTable(pid);
    if ( i == -1 ) {
        return -1;
    }

    lwcs_TaskDescriptor* task = &taskTable[i];
    task->period = period;
    
    return 0;
}

lwcs_Time lwcs_getTime(void) {
    disableIRQ();
    lwcs_Time tmp = shedTime;
    enableIRQ();
    return shedTime;
}

static lwcs_Pid addTaskGeneric(lwcs_TaskDescriptor taskDescriptor) {
    int i;
    i = findFreeSpaceInTaskTable();
    if ( i == -1 ) {
        return -1;
    }

    lwcs_Pid pid;
    pid = (lwcs_Pid) i;    // pid is position in taskTable[]

    taskDescriptor.pid = pid;

    taskTable[i] = taskDescriptor;

    return pid;
}

static int findFreeSpaceInTaskTable(void) {
    int i;
    for (i = 0; i < LWCS_MAX_TASKS; i++) {
        if ( taskTable[i].task == NULL ) {
            break;
        }
    }

    return ( i == LWCS_MAX_TASKS ? -1 : i);
}

static int findPidInTaskTable(lwcs_Pid pid) {
    int i;
    for (i = 0; i < LWCS_MAX_TASKS; i++) {
        if ( taskTable[i].pid == pid ) {
            break;
        }
    }

    return ( i == LWCS_MAX_TASKS ? -1 : i);
}
