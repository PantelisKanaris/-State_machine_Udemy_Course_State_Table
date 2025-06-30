#include "main.h"
#include "lcd.h"

event_status_e state_handler_idle(timer_state_struct_t *state, event_t *event);
// this is from the start action to the idle state.
void state_machine_init(timer_state_struct_t *state)
{
    event_t event;
    event.signal = ENTRY;
    // state->current_state = IDLE;
    state->current_state = IDLE; // as you can see instead of using an enum we use the state handler immediately.
    state->productive_time = 0;
    (*state->state_table[IDLE * MAX_SIGNALS +ENTRY])(state,&event);

}

// Nested switch implementation this is the function of the state machine

// helper function prototypes
void display_time(uint32_t time);
void display_message(String s, uint8_t colum, uint8_t row);
void display_clear(void);
void do_beep(void);

// event_status_e state_handler_time_set(timer_state_struct_t *state, event_t *event);
// event_status_e state_handler_countdown(timer_state_struct_t *state, event_t *event);
// event_status_e state_handler_pause(timer_state_struct_t *state, event_t *event);
// event_status_e state_handler_stat(timer_state_struct_t *state, event_t *event);

//////////////////////////////Switch aproach////////////////////////////////////////////////////////////////

// /// @brief
// /// @param state
// /// @param event
// /// @return
// event_status_e state_machine_switch_approach(timer_state_struct_t *state, event_t *event)
// {
//     switch (state->current_state)
//     {
//     case IDLE:
//         return state_handler_idle(state, event);

//     case TIME_SET:
//         /* code */
//         return state_handler_time_set(state, event);
//     case COUNTDOWN:
//         /* code */
//         return state_handler_countdown(state, event);

//     case PAUSE:
//         /* code */
//         return state_handler_pause(state, event);

//     case STAT:
//         /* code */
//         return state_handler_stat(state, event);
//     }
// }

//////////////////////////////Switch aproach////////////////////////////////////////////////////////////////

/////////////////////////////State table approach/////////////////////////////////////////////////////////
// For each case in the state handlers i need to create a function.

event_status_e Idle_State_Entry(timer_state_struct_t *state, event_t *event)
{
    state->current_time = 0;
    state->elapsed_time = 0;
    display_time(0);
    display_message("Set", 0, 0);
    display_message("time", 0, 1);
    return EVENT_HANDLED;
}

event_status_e Idle_State_Exit(timer_state_struct_t *state, event_t *event)
{
    display_clear();
    return EVENT_HANDLED;
}

event_status_e Idle_State_Dec_Time(timer_state_struct_t *state, event_t *event)
{
    return EVENT_IGNORED;
}

event_status_e Idle_State_Inc_Time(timer_state_struct_t *state, event_t *event)
{

    state->current_time += 60;
    // HERE THERE IS A TRANSITION
    state->current_state = TIME_SET;

    return EVENT_TRANSITION;
}

event_status_e Idle_State_Start_Pause(timer_state_struct_t *state, event_t *event)
{
    // HERE THERE IS A TRANSITION  WITH NO ACTION
    state->current_state = STAT;
    Serial.println("Status transition.");
    return EVENT_TRANSITION;
}

event_status_e Idle_State_Time_Tick(timer_state_struct_t *state, event_t *event)
{
    // FOR THE IDLE STATE THE TIM-TICK IS INTERNAL TRANSITION WITH QUARD
    if (((tick_event_t *)event)->ss == 5)
    {
        do_beep();
        return EVENT_HANDLED;
    }
    else
    {
        return EVENT_IGNORED;
    }
}

event_status_e Idle_State_Abrt(timer_state_struct_t *state, event_t *event)
{
    return EVENT_IGNORED;
}

/// @brief This function represent the actions the idle state will take
/// @param state the state_obj this mean the global variables each state has access to
/// @param event the event that brought the current state to idle
/// @return event_status enum (OK,ERROR)
// event_status_e state_handler_idle(timer_state_struct_t *state, event_t *event)
// {
//     // NOW THAT WE ARE INSIDE THE IDLE STATE WE WILL USE ANOTHER SWITCH TO SWTICH BETWEEN THE ACTION THAT NEED TO BE DONE (the switch will be for the signal/event)
//     switch (event->signal)
//     {
//     case ENTRY:
//         /* code */
//         state->current_time = 0;
//         state->elapsed_time = 0;
//         display_time(0);
//         display_message("Set", 0, 0);
//         display_message("time", 0, 1);
//         return EVENT_HANDLED;
//     case EXIT:
//         /* code */
//         // clear the display
//         display_clear();
//         return EVENT_HANDLED;
//     case INC_TIME:

//         state->current_time += 60;
//         // HERE THERE IS A TRANSITION
//         state->current_state = &state_handler_time_set;

//         return EVENT_TRANSITION;
//     case DEC_TIME:
//         return EVENT_IGNORED;
//     case START_PAUSE:
//         // HERE THERE IS A TRANSITION  WITH NO ACTION
//         state->current_state = &state_handler_stat;
//         Serial.println("Status transition.");
//         return EVENT_TRANSITION;
//     case TIME_TICK:
//         // FOR THE IDLE STATE THE TIM-TICK IS INTERNAL TRANSITION WITH QUARD
//         if (((tick_event_t *)event)->ss == 5)
//         {
//             do_beep();
//             return EVENT_HANDLED;
//         }
//         else
//         {
//             return EVENT_IGNORED;
//         }
//     case ABRT:
//         return EVENT_IGNORED;
//     }
// }

event_status_e Time_Set_Entry(timer_state_struct_t *state, event_t *event)
{
    display_time(state->current_time);
    return EVENT_HANDLED;
}

event_status_e Time_Set_Exit(timer_state_struct_t *state, event_t *event)
{
    display_clear();
    return EVENT_HANDLED;
}

event_status_e Time_Set_Dec_Time(timer_state_struct_t *state, event_t *event)
{
    if (state->current_time >= 60)
    {
        state->current_time -= 60;
        display_time(state->current_time);
        return EVENT_HANDLED;
    }
    return EVENT_IGNORED;
}

event_status_e Time_Set_Inc_Time(timer_state_struct_t *state, event_t *event)
{
    state->current_time += 60;
    display_time(state->current_time);
    return EVENT_HANDLED;
}

event_status_e Time_Set_Start_Pause(timer_state_struct_t *state, event_t *event)
{
    if (state->current_time >= 60)
    {
        state->current_state = COUNTDOWN;
        return EVENT_TRANSITION;
    }
    return EVENT_IGNORED;
}

event_status_e Time_Set_Time_Tick(timer_state_struct_t *state, event_t *event)
{
    return EVENT_IGNORED;
}

event_status_e Time_Set_Abrt(timer_state_struct_t *state, event_t *event)
{
    state->current_state = IDLE;
    return EVENT_TRANSITION;
}

/// @brief This function represent the actions the time_set state will take
/// @param state the state_obj this mean the global variables each state has access to
/// @param event the event that brought the current state to time_set
/// @return event_status enum (OK,ERROR)
// event_status_e state_handler_time_set(timer_state_struct_t *state, event_t *event)
// {
//     switch (event->signal)
//     {
//     case ENTRY:
//         display_time(state->current_time);
//         return EVENT_HANDLED;
//         /* code */

//     case EXIT:
//         /* code */
//         display_clear();
//         return EVENT_HANDLED;

//     case INC_TIME:
//         state->current_time += 60;
//         display_time(state->current_time);
//         return EVENT_HANDLED;

//     case DEC_TIME:
//         if (state->current_time >= 60)
//         {
//             state->current_time -= 60;
//             display_time(state->current_time);
//             return EVENT_HANDLED;
//         }
//         return EVENT_IGNORED;

//     case START_PAUSE:

//         if (state->current_time >= 60)
//         {
//             state->current_state = &state_handler_countdown;
//             return EVENT_TRANSITION;
//         }
//         return EVENT_IGNORED;

//     case TIME_TICK:
//         return EVENT_IGNORED;

//     case ABRT:
//         state->current_state = &state_handler_idle;
//         return EVENT_TRANSITION;
//     }
// }

event_status_e Count_Down_Entry(timer_state_struct_t *state, event_t *event)
{
    return EVENT_IGNORED;
}

event_status_e Count_Down_Exit(timer_state_struct_t *state, event_t *event)
{
    state->productive_time += state->elapsed_time;
    state->elapsed_time = 0;
    display_clear();
    return EVENT_HANDLED;
}

event_status_e Count_Down_Dec_Time(timer_state_struct_t *state, event_t *event)
{
    return EVENT_IGNORED;
}

event_status_e Count_Down_Inc_Time(timer_state_struct_t *state, event_t *event)
{
    return EVENT_IGNORED;
}

event_status_e Count_Down_Start_Pause(timer_state_struct_t *state, event_t *event)
{
    state->current_state = PAUSE;
    return EVENT_TRANSITION;
}

event_status_e Count_Down_Time_Tick(timer_state_struct_t *state, event_t *event)
{
    if (((tick_event_t *)event)->ss == 10)
    {
        state->current_time--;
        state->elapsed_time++;
        display_time(state->current_time);
        if (state->current_time == 0)
        {
            state->current_state = IDLE;
            return EVENT_TRANSITION;
        }
        return EVENT_HANDLED;
    }
    return EVENT_IGNORED;
}

event_status_e Count_Down_Abrt(timer_state_struct_t *state, event_t *event)
{
    state->current_state = IDLE;
    return EVENT_TRANSITION;
}

/// @brief This function represent the actions the countdown state will take
/// @param state the state_obj this mean the global variables each state has access to
/// @param event the event that brought the current state to countdown
/// @return event_status enum (OK,ERROR)
// event_status_e state_handler_countdown(timer_state_struct_t *state, event_t *event)
// {
//     switch (event->signal)
//     {
//     case ENTRY:
//         return EVENT_IGNORED;

//     case EXIT:
//         state->productive_time += state->elapsed_time;
//         state->elapsed_time = 0;
//         display_clear();
//         return EVENT_HANDLED;

//     case INC_TIME:
//         return EVENT_IGNORED;
//     case DEC_TIME:
//         return EVENT_IGNORED;
//     case START_PAUSE:
//         state->current_state = &state_handler_pause;
//         return EVENT_TRANSITION;
//     case TIME_TICK:
//         if (((tick_event_t *)event)->ss == 10)
//         {
//             state->current_time--;
//             state->elapsed_time++;
//             display_time(state->current_time);
//             if (state->current_time == 0)
//             {
//                 state->current_state = &state_handler_idle;
//                 return EVENT_TRANSITION;
//             }
//             return EVENT_HANDLED;
//         }
//         return EVENT_IGNORED;
//     case ABRT:
//         state->current_state = &state_handler_idle;
//         return EVENT_TRANSITION;
//     }
// }

event_status_e Pause_Entry(timer_state_struct_t *state, event_t *event)
{
    /* code */
    display_message("paused", 5, 1);

    return EVENT_HANDLED;
}

event_status_e Pause_Exit(timer_state_struct_t *state, event_t *event)
{
    /* code */
    display_clear();
    return EVENT_HANDLED;
}

event_status_e Pause_Dec_Time(timer_state_struct_t *state, event_t *event)
{
    if (state->current_time >= 60)
    {
        state->current_time -= 60;
        display_time(state->current_time);
        return EVENT_HANDLED;
    }
    return EVENT_IGNORED;
}

event_status_e Pause_Inc_Time(timer_state_struct_t *state, event_t *event)
{
    state->current_time += 60;
    state->current_state = TIME_SET;
    return EVENT_TRANSITION;
}

event_status_e Pause_Start_Pause(timer_state_struct_t *state, event_t *event)
{
    return EVENT_IGNORED;
}

event_status_e Pause_Time_Tick(timer_state_struct_t *state, event_t *event)
{
    return EVENT_IGNORED;
}

event_status_e Pause_Abrt(timer_state_struct_t *state, event_t *event)
{
    state->current_state = TIME_SET;
    return EVENT_TRANSITION;
}

/// @brief This function represent the actions the pause state will take
/// @param state the state_obj this mean the global variables each state has access to
/// @param event the event that brought the current state to pause
/// @return event_status enum (OK,ERROR)
// event_status_e state_handler_pause(timer_state_struct_t *state, event_t *event)
// {
//     switch (event->signal)
//     {
//     case ENTRY:
//         /* code */
//         display_message("paused", 5, 1);

//         return EVENT_HANDLED;

//     case EXIT:
//         /* code */
//         display_clear();
//         return EVENT_HANDLED;

//     case INC_TIME:
//         state->current_time += 60;
//         state->current_state = &state_handler_time_set;
//         return EVENT_TRANSITION;
//     case DEC_TIME:
//         if (state->current_time >= 60)
//         {
//             state->current_time -= 60;
//             display_time(state->current_time);
//             return EVENT_HANDLED;
//         }
//         return EVENT_IGNORED;
//     case START_PAUSE:
//         return EVENT_IGNORED;
//     case TIME_TICK:
//         return EVENT_IGNORED;
//     case ABRT:
//         state->current_state = &state_handler_idle;
//         return EVENT_TRANSITION;
//     }
// }

event_status_e Stat_Entry(timer_state_struct_t *state, event_t *event)
{
    /* code */
    Serial.println("Status entry.");
    display_time(state->productive_time);
    display_message("prod time", 0, 1);
    return EVENT_HANDLED;
}

event_status_e Stat_Exit(timer_state_struct_t *state, event_t *event)
{
    Serial.println("Status exit.");
    display_clear();
    return EVENT_HANDLED;
}

event_status_e Stat_Dec_Time(timer_state_struct_t *state, event_t *event)
{
    return EVENT_IGNORED;
}

event_status_e Stat_Inc_Time(timer_state_struct_t *state, event_t *event)
{
    return EVENT_IGNORED;
}

event_status_e Stat_Start_Pause(timer_state_struct_t *state, event_t *event)
{
    return EVENT_IGNORED;
}

event_status_e Stat_Time_Tick(timer_state_struct_t *state, event_t *event)
{
    static uint8_t tick_count = 0;
    Serial.println("Status tick.");
    if (tick_count == 20)
    {
        tick_count = 0;
        state->current_state = IDLE;
        return EVENT_TRANSITION;
    }
    tick_count++;
    return EVENT_IGNORED;
}

event_status_e Stat_Abrt(timer_state_struct_t *state, event_t *event)
{
    return EVENT_IGNORED;
}

/// @brief This function represent the actions the stat state will take
/// @param state the state_obj this mean the global variables each state has access to
/// @param event the event that brought the current state to stat
/// @return event_status enum (OK,ERROR)
// event_status_e state_handler_stat(timer_state_struct_t *state, event_t *event)
// {
//     static uint8_t tick_count = 0;
//     switch (event->signal)
//     {
//     case ENTRY:
//         /* code */
//         Serial.println("Status entry.");
//         display_time(state->productive_time);
//         display_message("prod time", 0, 1);
//         return EVENT_HANDLED;

//     case EXIT:
//         Serial.println("Status exit.");
//         display_clear();
//         return EVENT_HANDLED;
//         /* code */

//     case INC_TIME:
//         return EVENT_IGNORED;
//     case DEC_TIME:
//         return EVENT_IGNORED;
//     case START_PAUSE:
//         return EVENT_IGNORED;
//     case TIME_TICK:
//         Serial.println("Status tick.");
//         if (tick_count == 20)
//         {
//             tick_count = 0;
//             state->current_state = &state_handler_idle;
//             return EVENT_TRANSITION;
//         }
//         tick_count++;
//         return EVENT_IGNORED;
//     case ABRT:
//         return EVENT_IGNORED;
//     }
// }

/////////////////////////////State table approach/////////////////////////////////////////////////////////
///// HELPER FUNCTIONS //////////////
void display_time(uint32_t time)
{
    char buf[7];
    String time_message;
    uint16_t m = time / 60;
    uint8_t s = time % 60;
    sprintf(buf, "%03d:%02d", m, s);
    time_message = (String)buf;
    lcd_set_cursor(5, 0);
    lcd_print_string(time_message);
}

void display_message(String s, uint8_t colum, uint8_t row)
{
    lcd_set_cursor(row, colum);
    lcd_print_string(s);
}

void display_clear(void)
{
    lcd_clear();
}

void do_beep(void)
{
    tone(PIN_BUZZER, 4000, 25);
}