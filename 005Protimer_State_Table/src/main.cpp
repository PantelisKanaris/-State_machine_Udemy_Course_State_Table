
#include "main.h"
#include "lcd.h"
// this is the event dispatcher.
void state_machine_dispatcher(timer_state_struct_t *state, event_t *event);
uint8_t process_buton_pad_value(uint8_t pad_value);
// This is the timer state machine.
static timer_state_struct_t state_machine;
// put function declarations here:
void display_init();

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Productive time application\n");
  Serial.print("===========================");
  display_init();
  pinMode(PIN_BUTTON1,INPUT);
  pinMode(PIN_BUTTON2,INPUT);
  pinMode(PIN_BUTTON3,INPUT);
  state_machine_init(&state_machine);
}

void loop()
{
  uint8_t button1, button2, button3;
  uint8_t pad_value;
  user_event_t user_event;
  // only one time it will be initialized.
  static tick_event_t tick_event;
  static uint32_t current_time = millis();
  // 1. read the button pad status
  button1 = digitalRead(PIN_BUTTON1);
  button2 = digitalRead(PIN_BUTTON2);
  button3 = digitalRead(PIN_BUTTON3);
  pad_value = (button1 << 2) | (button2 << 1) | button3;
  // software debouncing
  pad_value = process_buton_pad_value(pad_value);
  if (pad_value)
  {
    // 2. make an event
    if (pad_value == BTN_PAD_VALUE_INC_TIME)
    {
      Serial.println("Incriment time event");
      user_event.super.signal = INC_TIME;
    }
    else if (pad_value == BTN_PAD_VALUE_DEC_TIME)
    {
      Serial.println("DECRIMENT time event");
      user_event.super.signal = DEC_TIME;
    }
    else if (pad_value == BTN_PAD_VALUE_SP)
    {
      Serial.println("Start pauseevent");
      user_event.super.signal = START_PAUSE;
    }
    else if (pad_value == BTN_PAD_VALUE_ABRT)
    {
      Serial.println("Abort event");
      user_event.super.signal = ABRT;
    }
    // 3. send it to event dispatcher.
    state_machine_dispatcher(&state_machine, &user_event.super);
  }

  // 4. dipsatch the time tick event every 100ms
  if (millis() - current_time >=100)
  {
    //100ms have passed
    //reset the current_limit
    current_time = millis();
    //add the signal.
    tick_event.super.signal = TIME_TICK;
    //this every time the 100ms have passed we incriment by one.
    tick_event.ss++;
    if(tick_event.ss >10)
    {
      tick_event.ss = 1;
    }
        // 3. send it to event dispatcher.
    state_machine_dispatcher(&state_machine, &tick_event.super);

  }
}

// put function definitions here:

/// @brief This dispatches the events from main to the state machine the events are categoriazed based on who sends them user/tick event
/// @param state 
/// @param event 
void state_machine_dispatcher(timer_state_struct_t *state, event_t *event)
{
  event_status_e status;
  // This holds the current event i am in before any action.
  state_handler_function_pointer source_state;
  source_state = state->current_state;
  // This holds the target state we trnasition to.
  state_handler_function_pointer target_state;

  // execute the signal given
  status = source_state(state,event);
  // if there is a transition what are the steps (we know the current state has changed to the new state and the action of the transition has been executed).
  // 1. run the exit action of the source state.
  // 2. run the entry action for the new state.
  if (status == EVENT_TRANSITION)
  {
    target_state = state->current_state;
    event_t new_event;
    // 1. run the exit action of the source state.
    new_event.signal = EXIT;
    source_state(state,&new_event);
    // 2. run the entry action for the new state.
    new_event.signal = ENTRY;
    target_state(state,&new_event);
  }

}

/// @brief This debounces the button using the a simple state machine approach
/// @param pad_value 
/// @return 
uint8_t process_buton_pad_value(uint8_t pad_value)
{
  // beacuse is static they will not rest each time the function is called.
  static button_state_e btn_state = DEPRESSED;
  static uint32_t current_time = millis();
  switch (btn_state)
  {
  case DEPRESSED:
    if(pad_value)
    {
      btn_state=BOUNCE;
      current_time = millis();
    }
    break;
    case PRESSED:
    if(!pad_value)
    {
      btn_state = BOUNCE;
      current_time = millis();
      break;
    }
    break;
    case BOUNCE:
    if(millis() - current_time >=50)
    {
      if(pad_value)
      {
        btn_state = PRESSED;
        return pad_value;
      }
      else
      {
        btn_state = DEPRESSED;
      }
    }
    break;
  }
  return 0;
}

void display_init()
{
  lcd_begin(16,2);
  lcd_clear();
  lcd_move_cursor_L_to_R();
  lcd_set_cursor(0,0);
  lcd_no_auto_scroll();
  lcd_cursor_off();
}


