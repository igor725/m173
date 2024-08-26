#pragma once

struct ScriptEvent {
  enum Type {
    onStart,
    onStop,
  };

  Type  type;
  void* args;
};
