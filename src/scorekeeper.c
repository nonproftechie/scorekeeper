#include <stdio.h>
#include <pebble.h>

static Window *window;
static TextLayer *upper_layer;
static TextLayer *lower_layer;
static int upper_num;
static int lower_num;
static char s_upper[5];
static char s_lower[5];
static bool positive;

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
  snprintf(s_lower, sizeof(s_upper), "%d", lower_num);
  text_layer_set_text(lower_layer, s_lower);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  upper_layer = text_layer_create((GRect) { .origin = { 0, (bounds.size.h / 2) - 48 - 4 }, .size = { bounds.size.w, 40 } });
  text_layer_set_text(upper_layer, "0");
  text_layer_set_text_alignment(upper_layer, GTextAlignmentCenter);
  text_layer_set_font(upper_layer, fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS));
  layer_add_child(window_layer, text_layer_get_layer(upper_layer));
  
  lower_layer = text_layer_create((GRect) { .origin = { 0, (bounds.size.h / 2) + 8 - 4 }, .size = { bounds.size.w, 40 } });
  text_layer_set_text(lower_layer, "0");
  text_layer_set_text_alignment(lower_layer, GTextAlignmentCenter);
  text_layer_set_font(lower_layer, fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS));
  layer_add_child(window_layer, text_layer_get_layer(lower_layer));
  
}

static void window_unload(Window *window) {
  text_layer_destroy(upper_layer);
  text_layer_destroy(lower_layer);
}

static void init(void) {
  positive = true;
  upper_num = 0;
  lower_num = 0;
  
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	  .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
  redraw();
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}