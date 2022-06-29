#include <stdint.h>
#include "hw_bare.h"


typedef enum {
    TASK_NONE=0,
    TASK_ALARM,
    TASK_TIMER,
    TASK_EVENT,
} _TaskType;

typedef struct {
    Task foo;
    int type;
    clock_t argv;
    clock_t ticks;
} TaskStack;

volatile TaskStack _sys_task[TASK_STACK_SIZE];
volatile clock_t _sys_event = 0;
volatile clock_t _sys_uptime = 0;

static void Ticks_Config(int);
static void Ticks_Sleep(void);

static int Task_Register(int,clock_t,Task);
static int isEventSet(int evt);


int sys_init(void) {
    Ticks_Config(_1TS);
    for(int i=0; i<TASK_STACK_SIZE; i++) {
        sys_task_destory(i);
    }
    return 0;
}

int sys_task_reg_timer(clock_t ms, Task foo) {
    if(ms < _1TS) ms = _1TS;
    return Task_Register(TASK_TIMER, ms, foo);
}

int sys_task_reg_alarm(clock_t ms, Task foo) {
    if(ms < _1TS) ms = _1TS;
    return Task_Register(TASK_ALARM, ms, foo);
}

int sys_task_reg_event(int evt, Task foo) {
    return Task_Register(TASK_EVENT, evt, foo);
}

int sys_task_destory(int task_id) {
    if(task_id <0) return -1;
    if(task_id >= TASK_STACK_SIZE) return -1;
    _sys_task[task_id].type = TASK_NONE;
    _sys_task[task_id].foo = 0;
    _sys_task[task_id].argv = 0;
    _sys_task[task_id].ticks = 0;
    return 0;
}

int sys_task_remove(Task foo) {
    for(int i=0; i<TASK_STACK_SIZE; i++) {
        if(_sys_task[i].foo == foo) {
            _sys_task[i].type = TASK_NONE;
            _sys_task[i].foo = 0;
            _sys_task[i].argv = 0;
            _sys_task[i].ticks = 0;
            return 0;
        }
    }
    return -1;
}

void sys_run(void) {
    while(1) {
        int evt_systicks = 0;
        Ticks_Sleep();
        if(isEventSet(EVENT_SYSTICKS)) {
            evt_systicks = 1;
            sys_event_clear(EVENT_SYSTICKS);
            _sys_uptime += _1TS;
        }
        for(int i=0; i<TASK_STACK_SIZE; i++) {
            switch(_sys_task[i].type) {
            case TASK_ALARM:
                if(evt_systicks) {
                    if(_sys_task[i].ticks < _1TS) {
                        _sys_task[i].ticks = 0;
                    } else {
                        _sys_task[i].ticks -= _1TS;
                    }
                    if(_sys_task[i].ticks == 0) {
                        _sys_task[i].foo();
                        sys_task_destory(i);
                    }
                }
                break;
            case TASK_TIMER:
                if(evt_systicks) {
                    if(_sys_task[i].ticks < _1TS) {
                        _sys_task[i].ticks = 0;
                    } else {
                        _sys_task[i].ticks -= _1TS;
                    }
                    if(_sys_task[i].ticks == 0) {
                        _sys_task[i].foo();
                        _sys_task[i].ticks = _sys_task[i].argv;
                    }
                }
                break;
            case TASK_EVENT:
                if(isEventSet(_sys_task[i].argv)) {
                    _sys_task[i].foo();
                    sys_event_clear(_sys_task[i].argv);
                }
                break;
            }
        }
    }
}

int sys_event_trigger(int evt) {
    _sys_event |= (clock_t)(1<<evt);
    return 0;
}

int sys_event_clear(int evt) {
    _sys_event &= ~((clock_t)(1<<evt));
    return 0;
}

static int isEventSet(int evt) {
    return ((_sys_event&((clock_t)(1<<evt)))!=0);
}

static int Task_Register(int type, clock_t argv, Task foo) {
    for(int i=0; i<TASK_STACK_SIZE; i++) {
        if(_sys_task[i].type == TASK_NONE) {
            _sys_task[i].foo = foo;
            _sys_task[i].argv = argv;
            _sys_task[i].ticks = argv;
            _sys_task[i].type = type;
            return i;
        }
    }
    return -1;
}

clock_t sys_uptime(void) {
    return _sys_uptime;
}


#include "hc32lx.h"

/*
 * Configure Tick to generate an update interrupt each x ms
 */
static void Ticks_Config(int ts) {
#if defined(HWB_OS_USE_SYSTICK)
    SysTick_Config( _1TS*CORE_CLOCK_HZ/1000 );
#elif defined(HWB_OS_USE_LPTIM)
    M0P_CLOCK->PERI_CLKEN_f.LPTIM = 1;
    M0P_LPTIMER->CR_f.TR = 0;
    M0P_LPTIMER->CR_f.CT = 0;
    M0P_LPTIMER->CR_f.MD = 1;
    M0P_LPTIMER->CR_f.GATE = 0;
    M0P_LPTIMER->CR_f.TCK_SEL = 3; /* clock=RCL */
    M0P_LPTIMER->ICLR_f.TFC = 0;
    M0P_LPTIMER->CR_f.IE = 1;
    nvic_irq_enable(LPTIM_IRQn, HWB_IRQ_PRIO_DEFAULT);
    /* wait write flag */
    while(M0P_LPTIMER->CR_f.WT_FLAG == 0) nop();
    M0P_LPTIMER->ARR_f.ARR = 0xFFFF-(CORE_CLOCK_HZ*_1TS+500)/1000;
    M0P_LPTIMER->CR_f.TR = 1;
#else
    #error "Please defined the OS's ticks source"
#endif
}

static void Ticks_Sleep(void) {
#if defined(HWB_OS_USE_SYSTICK)
    wfi();
#elif defined(HWB_OS_USE_LPTIM)
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    wfi();
#endif
}

/*
 * Configure Tick IRQ callback function
 */
#if defined(HWB_OS_USE_LPTIM)
void __attribute__((weak)) LPTIM_IRQn_handler(void) {
    M0P_LPTIMER->ICLR_f.TFC = 0;
    sys_event_trigger(EVENT_SYSTICKS);
}
#elif defined(HWB_OS_USE_SYSTICK)
void __attribute__((weak)) SysTick_IRQn_handler(void) {
    sys_event_trigger(EVENT_SYSTICKS);
}
#endif
