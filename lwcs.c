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

static lwcs_TaskDescriptor taskTable[LWCS_MAX_TASKS] = { 0 };
static uint32_t ticksInMilisec;
static lwcs_Time (*lwcs_getJiffies)(void);

static lwcs_Pid addTaskGeneric(lwcs_TaskDescriptor taskDescriptor);
static int findFreeSpaceInTaskTable(void);
static int findPidInTaskTable(lwcs_Pid pid);

void lwcs_initialize(uint32_t jiffiesInMilliSec,
                     lwcs_Time (* getJiffies)(void)
                     ) {
    ticksInMilisec = jiffiesInMilliSec;
    lwcs_getJiffies = getJiffies;
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
    taskToAdd.nextRun = time * ticksInMilisec;
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

void lwcs_run(void) {
    lwcs_Time lastShedTime = 0;
    lwcs_Time currentShedTime = 0;
    while (1) {
        currentShedTime = lwcs_getJiffies();
        currentShedTime *= ticksInMilisec;
        if ( currentShedTime == lastShedTime ) {
            continue;
        }
        lastShedTime = currentShedTime;
        for (uint8_t i = 0; i < LWCS_MAX_TASKS; i++) {
            lwcs_TaskDescriptor* task = &taskTable[i];
            if (task->task == NULL) {
                continue;
            }

            if ( lastShedTime >= task->nextRun ) {
                if ( task->task() == -1 ) {
                    lwcs_deleteTask(task->pid);
                    continue;
                }
                task->lastRun = lastShedTime;
                task->nextRun = lastShedTime + task->period;
            }
        }
    }
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
