**lwcs** is as **L**ightweight **C**ooperative **S**cheduler for short.

##### Pros
* lightweight
* platform independent
* easy to use

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
