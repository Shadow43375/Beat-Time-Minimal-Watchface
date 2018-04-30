#include <pebble.h>
static Window *s_main_window;
// static TextLayer *s_time_layer;

// Window *window;
static TextLayer *time_layer;
TextLayer *date_layer;
Layer* motherLayer;
Layer *myCanvas;


char* getMonthName(int month) {
    char* month_name = "";
    APP_LOG(APP_LOG_LEVEL_DEBUG, "month = %d", month);

    switch(month) {
        case 0:
            month_name = "January";
            break;
        case 1:
            month_name = "Febuary";
            break;
        case 2:
            month_name = "March";
            break;
        case 3:
            month_name = "April";
            break;
        case 4:
            month_name = "May";
            break;
        case 5:
            month_name = "June";
            break;
        case 6:
            month_name = "July";
            break;
        case 7:
            month_name = "August";
            break;
        case 8:
            month_name = "September";
            break;
        case 9:
            month_name = "October";
            break;
        case 10:
            month_name = "November";
            break;
        case 11:
            month_name = "December";
            break;
    }
    return month_name;
}



static void updateGame(Layer *layer, GContext *ctx)
{
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_line(ctx, GPoint(10, 99), GPoint(131,99));
}

void create_layout() {
     GFont time_font =  fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
     GFont date_font =  fonts_get_system_font(FONT_KEY_GOTHIC_18);  
  
     myCanvas = layer_create(GRect(0, 0, 143, 160));
     motherLayer = window_get_root_layer(s_main_window);
     layer_add_child(motherLayer, myCanvas);
     layer_set_update_proc(myCanvas, updateGame);

     date_layer = text_layer_create(GRect(10,78,122,20));
     text_layer_set_background_color(date_layer, GColorClear);
     text_layer_set_text_color(date_layer, GColorWhite);
     layer_add_child(motherLayer, text_layer_get_layer(date_layer));
     text_layer_set_text(date_layer, "November    1");
     text_layer_set_font(date_layer, date_font);
      
      time_layer = text_layer_create(GRect(10,95,122,50));
      text_layer_set_background_color(time_layer, GColorClear);
      text_layer_set_text_color(time_layer, GColorWhite);
      layer_add_child(motherLayer, text_layer_get_layer(time_layer));
      text_layer_set_text(time_layer, "391");
      text_layer_set_text_alignment(time_layer, GTextAlignmentLeft);
      text_layer_set_font(time_layer, time_font);
}

char *itoa(int num)
{
  static char buff[20] = {};
  int i = 0, temp_num = num, length = 0;
  char *string = buff;
  
  if(num >= 0) { //See NOTE
    // count how many characters in the number
    while(temp_num) {
      temp_num /= 10;
      length++;
    }
    // assign the number to the buffer starting at the end of the 
    // number and going to the begining since we are doing the
    // integer to character conversion on the last number in the
    // sequence
    for(i = 0; i < length; i++) {
      buff[(length-1)-i] = '0' + (num % 10);
      num /= 10;
    }
    buff[i] = '\0'; // can't forget the null byte to properly end our string
  }
  else {
    return "Unsupported Number";
  }
  
  return string;
}

int convertTime(struct tm* timeDataGMT) {
    int beats;
    int offset = 1;
    int hr = timeDataGMT->tm_hour + offset;
    int min = timeDataGMT->tm_min;
    int sec = timeDataGMT->tm_sec;
    int seconds_in_hour = 60*60;
    int seconds_in_min = 60;
    float seconds_in_beat = 86.4;
     
    if (hr>=24) {
        hr = hr%24;
    }
    beats = (hr*seconds_in_hour) + (min*seconds_in_min) + sec;
    beats = beats/seconds_in_beat;
    return beats;
}


static void update_time() {
  int beats;
  int month = 0;
  static char month_day[40] = "";
  static char full_str[5] = "";
  static char* month_name = "";
  static char* day_name = "";
  
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "month_day = %s", month_day);
  
  //set full_str = "" to avoid text ad infinitum... eg 054054054.
  memset(&full_str[0], 0, sizeof(full_str));
  //might need to wipe this too... not sure if it causes error... seems to but is needed. 
  memset(&month_day[0], 0, sizeof(month_day));

  
  // Get a tm structure
  time_t currentTime;
  time(&currentTime);
    
  // time struct for GMT
  struct tm* timeDataGMT = gmtime(&currentTime);
  beats = convertTime(timeDataGMT);
  
  //time struct for local time (important for getting local day/date).
  struct tm* timeDataLocal = localtime(&currentTime);

     int day = timeDataLocal-> tm_mday;
     month = timeDataLocal->tm_mon;
     month_name = getMonthName(month);
  
     day_name = itoa(day);
     strcat(month_day, month_name);
     strcat(month_day, "  ");
     strcat(month_day, day_name);

  
  // Write the current hours and minutes into a buffer
  static char* s_buffer;
  s_buffer = itoa(beats);
  strcat(full_str, "@");

  if (beats < 10) {
      strcat(full_str, "00");
  }
  if (beats < 100 && beats >= 10) {
      strcat(full_str, "0");
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "s_buffer before: %s", s_buffer);
  strcat(full_str, s_buffer);
  s_buffer = full_str;  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "s_buffer after: %s", s_buffer);

  // Display this time on the TextLayer
  text_layer_set_text(time_layer, s_buffer);
  text_layer_set_text(date_layer, month_day);
}

static void tick_handler(struct tm *timeData, TimeUnits units_changed) {
  update_time();
}


static void main_window_load(Window *window) {
   create_layout();
}  

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(time_layer);
  text_layer_destroy(date_layer);
  layer_destroy(myCanvas);
}



static void init() {
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  s_main_window = window_create();
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_background_color(s_main_window, GColorBlack);
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
  update_time();
}

static void deinit() {
   // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}