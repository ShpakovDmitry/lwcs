/**
 * @file   lwcs.h
 * @brief  lightweight cooperative scheduler header file
 * @author ShpakovDmitry
 * @date   2020-12-25
 */

#ifndef LWCS_H
#define LWCS_H

#include <stdint.h>

typedef int (*lwcs_Task)(void); ///< Pointer to task function
typedef uint32_t lwcs_Time;     ///< Scheduler time
typedef int lwcs_Pid;           ///< Process ID (PID)

/**
 * @brief initialize sheduler
 * Should be called prior to any other lwcs method
 * @param jiffiesInMilliSec uint32_t Jiffies in millisecond
 * set for proper time scale
 * @param lwcs_Time (*)(void) getJiffies callback to get Jiffies
 * @return void
 */
void lwcs_initialize(uint32_t jiffiesInMilliSec,
                     lwcs_Time (* getJiffies)(void)
                     );


/**
 * @brief Add task to sheduler
 * @param task  lwcs_Task task callback
 * @param period lwcs_Time task run period in milliseconds
 * @return lwcs_Pid task PID number
 */
lwcs_Pid lwcs_addTask(lwcs_Task task, lwcs_Time period);

/**
 * @brief run task at given time
 * @param task lwcs_Task task callback
 * @param time lwcs_Time time at which run the task. Time relative to scheduler
 * @return pid lwcs_Pid returns task PID
 */
lwcs_Pid lwcs_runTaskAt(lwcs_Task task, lwcs_Time time);

/**
 * @brief delete task from sheduler
 * @param pid lwcs_Pid task PID
 * @return int returns 0 if succeded, -1 otherwise. This indicates that task
 * with such PID doesnot exists in sheduler task list
 */
int lwcs_deleteTask(lwcs_Pid pid);

/**
 * @brief change task run period by task PID
 * @param pid lwcs_Pid task PID
 * @return int returns 0 if succeded, -1 otherwise. This indicates that task
 * with such PID doesnot exists in sheduler task list
 */
int lwcs_changeTaskPeriod(lwcs_Pid pid, lwcs_Time period);

/**
 * @brief get sheduler current time
 * @return lwcs_Time sheduler current time
 */
lwcs_Time lwcs_getTime(void);

/**
 * @brief run sheduler
 */
void lwcs_run(void);

#endif  // LWCS_H
