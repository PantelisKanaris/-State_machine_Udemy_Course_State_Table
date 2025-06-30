#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
// button and buzzer pins
#define PIN_BUTTON1 2
#define PIN_BUTTON2 3
#define PIN_BUTTON3 4
#define PIN_BUZZER 12

#define BTN_PAD_VALUE_INC_TIME 4
#define BTN_PAD_VALUE_DEC_TIME 2
#define BTN_PAD_VALUE_ABRT 6
#define BTN_PAD_VALUE_SP 1
// lcd pins
#define PIN_LCD_RS 5
#define PIN_LCD_RW 6
#define PIN_LCD_EN 7
#define PIN_LCD_D4 8
#define PIN_LCD_D5 9
#define PIN_LCD_D6 10
#define PIN_LCD_D7 11

/// @brief This is the transmitions events/signals
typedef enum
{
    INC_TIME,
    DEC_TIME,
    TIME_TICK,
    START_PAUSE,
    ABRT,
    // INTERNAL ACTIVIY SIGNALS
    ENTRY,
    EXIT,
    MAX_SIGNALS

} signals_e;


/// @brief For generic  generated events
typedef struct event_t
{
    signals_e signal;
    /* data */
} event_t;

/// @brief For user generated events
typedef struct user_event_t
{
    event_t super;
    /* data */
} user_event_t;

typedef enum
{
    EVENT_HANDLED,
    EVENT_IGNORED,
    EVENT_TRANSITION,
} event_status_e;

/// @brief This is for the tick event it also holds the counter parameter.
typedef struct tick_event_t
{
    event_t super;
    uint8_t ss;
    /* data */
} tick_event_t;


//This will not be needed because we wont use these variabls to code the states
/// @brief This are all of the States
typedef enum
{
    IDLE,
    TIME_SET,
    COUNTDOWN,
    PAUSE,
    STAT,
    MAX_STATES
} states_e;


// Forward-declare struct so the compiler knows the name exists
struct timer_state_struct_t;


//this will be in the type inside the 2d table.
//this is a function pointer that will point to the event handler needed to be executed.
typedef event_status_e( *event_handler_function_pointer)(timer_state_struct_t *state, event_t *event);

//This will have instead of a state enum an activave_state function pointer that will point to  the state handler of the active state.
/// @brief This struct represent the 'global variables' these means that all of the states will have access to this variables.
typedef struct timer_state_struct_t
{
    uint32_t current_time;
    uint32_t elapsed_time;
    uint32_t productive_time;
    //this is a function pointer that will hold the active state function handler
   states_e current_state;
   event_handler_function_pointer * state_table;
    /* data */
} timer_state_struct_t;

// STATE MACHINE.cpp DECLARATIONS
void state_machine_init(timer_state_struct_t *state);
event_status_e state_machine_switch_approach(timer_state_struct_t *state, event_t *event);



//BUTTON DEBOUNCING STATE MACHINE ENUMS

typedef enum 
{
    PRESSED,
    DEPRESSED,
    BOUNCE
}button_state_e;




event_status_e Idle_State_Entry(timer_state_struct_t *state, event_t *event);
event_status_e Idle_State_Exit(timer_state_struct_t *state, event_t *event);
event_status_e Idle_State_Dec_Time(timer_state_struct_t *state, event_t *event);
event_status_e Idle_State_Inc_Time(timer_state_struct_t *state, event_t *event);
event_status_e Idle_State_Start_Pause(timer_state_struct_t *state, event_t *event);
event_status_e Idle_State_Time_Tick(timer_state_struct_t *state, event_t *event);
event_status_e Idle_State_Abrt(timer_state_struct_t *state, event_t *event);

event_status_e Time_Set_Entry(timer_state_struct_t *state, event_t *event);
event_status_e Time_Set_Exit(timer_state_struct_t *state, event_t *event);
event_status_e Time_Set_Dec_Time(timer_state_struct_t *state, event_t *event);
event_status_e Time_Set_Inc_Time(timer_state_struct_t *state, event_t *event);
event_status_e Time_Set_Start_Pause(timer_state_struct_t *state, event_t *event);
event_status_e Time_Set_Time_Tick(timer_state_struct_t *state, event_t *event);
event_status_e Time_Set_Abrt(timer_state_struct_t *state, event_t *event);

event_status_e Count_Down_Entry(timer_state_struct_t *state, event_t *event);
event_status_e Count_Down_Exit(timer_state_struct_t *state, event_t *event);
event_status_e Count_Down_Dec_Time(timer_state_struct_t *state, event_t *event);
event_status_e Count_Down_Inc_Time(timer_state_struct_t *state, event_t *event);
event_status_e Count_Down_Start_Pause(timer_state_struct_t *state, event_t *event);
event_status_e Count_Down_Time_Tick(timer_state_struct_t *state, event_t *event);
event_status_e Count_Down_Abrt(timer_state_struct_t *state, event_t *event);


event_status_e Pause_Entry(timer_state_struct_t *state, event_t *event);
event_status_e Pause_Exit(timer_state_struct_t *state, event_t *event);
event_status_e Pause_Dec_Time(timer_state_struct_t *state, event_t *event);
event_status_e Pause_Inc_Time(timer_state_struct_t *state, event_t *event);
event_status_e Pause_Start_Pause(timer_state_struct_t *state, event_t *event);
event_status_e Pause_Time_Tick(timer_state_struct_t *state, event_t *event);
event_status_e Pause_Abrt(timer_state_struct_t *state, event_t *event);

event_status_e Stat_Entry(timer_state_struct_t *state, event_t *event);
event_status_e Stat_Exit(timer_state_struct_t *state, event_t *event);
event_status_e Stat_Dec_Time(timer_state_struct_t *state, event_t *event);
event_status_e Stat_Inc_Time(timer_state_struct_t *state, event_t *event);
event_status_e Stat_Start_Pause(timer_state_struct_t *state, event_t *event);
event_status_e Stat_Time_Tick(timer_state_struct_t *state, event_t *event);
event_status_e Stat_Abrt(timer_state_struct_t *state, event_t *event);






#endif
