/*
** $Id: hwb_os.h $
** hwb_os - hw_bare system os module
*/

#ifndef _HWB_OS_H_
#define _HWB_OS_H_


extern int sys_init(void);
extern void sys_run(void);

typedef unsigned long clock_t;
typedef void (*Task)(void);

/* @return: task id(0~TASK_STACK_SIZE)
 *          -1: task full/error
 */
extern int sys_task_reg_timer(clock_t ms, Task foo);
extern int sys_task_reg_alarm(clock_t ms, Task foo);
extern int sys_task_reg_event(int evt, Task foo);
extern int sys_task_destory(int task_id);
extern int sys_task_remove(Task foo);

/* system event functions
 * evt: 1~31
 */
extern int sys_event_trigger(int evt);
extern int sys_event_clear(int evt);

extern clock_t sys_uptime(void);

/* Task Stack Size */
#ifdef HWB_OS_TASK_STACK_SIZE
    #define TASK_STACK_SIZE  HWB_OS_TASK_STACK_SIZE
#else
    #define TASK_STACK_SIZE  16
#endif

/* Task Ticks, unit ms */
#ifdef HWB_OS_TASK_STACK_SIZE
    #define _1TS    HWB_OS_1TS
#else
    #define _1TS    1
#endif

/* Reserved event-0 for systick */
#define EVENT_SYSTICKS   0


#endif /* _HWB_OS_H_ */
