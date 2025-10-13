/**
 * Emscripten library functions
 * These are the C function declarations that map to JavaScript implementations
 */

mergeInto(LibraryManager.library, {
  // Audio bridge functions
  js_audio_create_object: function (object_id, type_ptr) {
    window.audioBridgeAPI.createObject(object_id, type_ptr);
  },

  js_audio_connect: function (src_id, src_output, dst_id, dst_input) {
    window.audioBridgeAPI.connect(src_id, src_output, dst_id, dst_input);
  },

  js_audio_set_param: function (object_id, param_ptr, value) {
    window.audioBridgeAPI.setParam(object_id, param_ptr, value);
  },

  js_audio_set_param_string: function (object_id, param_ptr, value_ptr) {
    // Not yet implemented
  },

  js_audio_trigger: function (object_id, event_ptr, value) {
    window.audioBridgeAPI.trigger(object_id, event_ptr, value);
  },

  js_audio_set_array: function (object_id, param_ptr, data_ptr, length) {
    window.audioBridgeAPI.setArray(object_id, param_ptr, data_ptr, length);
  },

  // Arduino timing functions
  js_performance_now: function () {
    return window.audioBridgeAPI.performanceNow();
  },

  js_console_log: function (msg_ptr) {
    window.audioBridgeAPI.consoleLog(msg_ptr);
  },

  // Pin I/O
  js_set_pin: function (pin, value) {
    window.audioBridgeAPI.setPin(pin, value);
  },

  js_get_pin: function (pin) {
    return window.audioBridgeAPI.getPin(pin);
  },

  // Timer functions
  js_set_timer: function (timer_id, microseconds) {
    window.audioBridgeAPI.setTimer(timer_id, microseconds);
  },

  js_clear_timer: function (timer_id) {
    window.audioBridgeAPI.clearTimer(timer_id);
  },
});
