#include <stdio.h>
#include <pebble.h>

static Window *window;
static TextLayer *upper_layer;
static TextLayer *lower_layer;
static int upper_num;
static int lower_num;
static uint32_t upper_num_key;
static uint32_t lower_num_key;
static char s_upper[5];
static char s_lower[5];
static bool positive;
static StatusBarLayer *s_status_bar;

static GColor get_color(bool positive, bool is_bkg_layer) {
  #if defined(PBL_COLOR)
  return positive ?
    ( is_bkg_layer ? GColorBlue : GColorInchworm ) :
    ( is_bkg_layer ? GColorRed : GColorChromeYellow );
  #elif defined(PBL_BW)
  return positive ?
    ( is_bkg_layer ? GColorWhite : GColorBlack ) :
    ( is_bkg_layer ? GColorBlack : GColorWhite );
  #endif
}

static void redraw() {
  window_set_background_color(window, get_color(positive, true));
  text_layer_set_background_color(upper_layer, get_color(positive, true));
  text_layer_set_background_color(lower_layer, get_color(positive, true));
  text_layer_set_text_color(upper_layer, get_color(positive, false));
  text_layer_set_text_color(lower_layer, get_color(positive, false));
  status_bar_layer_set_colors(s_status_bar, get_color(positive, true), get_color(positive, false));
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  positive = !positive;
  redraw();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  upper_num += positive ? 1 : -1;
  snprintf(s_upper, sizeof(s_upper), "%d", upper_num);
  text_layer_set_text(upper_layer, s_upper);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  lower_num += positive ? 1 : -1;
  snprintf(s_lower, sizeof(s_lower), "%d", lower_num);
  text_layer_set_text(lower_layer, s_lower);
}

void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  upper_num = 0;
  lower_num = 0;
  
  snprintf(s_upper, sizeof(s_upper), "%d", upper_num);
  text_layer_set_text(upper_layer, s_upper);
  
  snprintf(s_lower, sizeof(s_lower), "%d", lower_num);
  text_layer_set_text(lower_layer, s_lower);
  
  vibes_short_pulse();
}

void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  
  window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  upper_layer = text_layer_create((GRect) { .origin = { 0, (bounds.size.h / 2) - 48 - 4 }, .size = { bounds.size.w, 40 } });
  text_layer_set_text(upper_layer, s_upper);
  text_layer_set_text_alignment(upper_layer, GTextAlignmentCenter);
  text_layer_set_font(upper_layer, fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS));
  layer_add_child(window_layer, text_layer_get_layer(upper_layer));
  
  lower_layer = text_layer_create((GRect) { .origin = { 0, (bounds.size.h / 2) + 8 - 4 }, .size = { bounds.size.w, 40 } });
  text_layer_set_text(lower_layer, s_lower);
  text_layer_set_text_alignment(lower_layer, GTextAlignmentCenter);
  text_layer_set_font(lower_layer, fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS));
  layer_add_child(window_layer, text_layer_get_layer(lower_layer));
  
  s_status_bar = status_bar_layer_create();
  layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));
  
}

static void window_unload(Window *window) {
  text_layer_destroy(upper_layer);
  text_layer_destroy(lower_layer);
  status_bar_layer_destroy(s_status_bar);
}

static void init(void) {
  positive = true;
  upper_num = persist_read_int(upper_num_key);
  lower_num = persist_read_int(lower_num_key);
  snprintf(s_upper, sizeof(s_upper), "%d", upper_num);
  snprintf(s_lower, sizeof(s_lower), "%d", lower_num);
  
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	  .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
  redraw();
}

static void deinit(void) {
  persist_write_int(upper_num_key, upper_num);
  persist_write_int(lower_num_key, lower_num);
  window_destroy(window);
}

int main(void) {
  upper_num_key = 0;
  lower_num_key = 1;
  init();
  app_event_loop();
  deinit();
}