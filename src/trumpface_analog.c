#include "trumpface_analog.h"

#include "pebble.h"

static Window    *s_window;
static Layer     *s_hands_layer, *s_simple_bg_layer;
static TextLayer *s_trump_layer;

static GPath     *s_tick_paths[NUM_CLOCK_TICKS];
static GPath     *s_minute_arrow, *s_hour_arrow;


static void bg_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorWhite);
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    const int x_offset = PBL_IF_ROUND_ELSE(18, 0);
    const int y_offset = PBL_IF_ROUND_ELSE(6, 0);
    gpath_move_to(s_tick_paths[i], GPoint(x_offset, y_offset));
    gpath_draw_filled(ctx, s_tick_paths[i]);
  }
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds  = layer_get_bounds(layer);

  time_t now    = time(NULL);
  struct tm *t  = localtime(&now);

  // minute/hour hand
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);

  gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_draw_filled(ctx, s_minute_arrow);
  gpath_draw_outline(ctx, s_minute_arrow);

  // dot in the middle
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 - 1, 3, 3), 0, GCornerNone);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_window));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds        = layer_get_bounds(window_layer);

  s_simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_simple_bg_layer);

  s_hands_layer       = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);

  TextLayer *s_trump_layer = text_layer_create(bounds);

  s_trump_layer = text_layer_create(
  GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  text_layer_set_background_color(s_trump_layer, GColorClear);
  text_layer_set_text_color(s_trump_layer, GColorWhite);
  text_layer_set_text_alignment(s_trump_layer, GTextAlignmentCenter);
  text_layer_set_text(s_trump_layer, "Trump");

  layer_add_child(window_layer, text_layer_get_layer(s_trump_layer));
}

static void window_unload(Window *window) {
  layer_destroy(s_simple_bg_layer);
  layer_destroy(s_hands_layer);
  text_layer_destroy(s_trump_layer);
}

static void init() {
  s_window  = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load   = window_load,
    .unload = window_unload,
  });

  window_stack_push(s_window, true);

  // init hand paths
  s_minute_arrow      = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow        = gpath_create(&HOUR_HAND_POINTS);

  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds        = layer_get_bounds(window_layer);
  GPoint center       = grect_center_point(&bounds);
  gpath_move_to(s_hour_arrow, center);
  gpath_move_to(s_minute_arrow, center);

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void deinit() {
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
