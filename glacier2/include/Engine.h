#pragma once

namespace Glacier {

  class Console;
  class Scripting;
  class Graphics;

  class Engine {
  public:
    enum Signal {
      Signal_None = 0,
      Signal_Stop
    };
  protected:
    Console* mConsole;
    Scripting* mScripting;
    Graphics* mGraphics;
    HANDLE mProcess;
    HANDLE mThread;
    HINSTANCE mInstance;
    volatile Signal mSignal;
    void adjustPrivileges();
    void fixupThreadAffinity();
  public:
    Engine( HINSTANCE instance );
    ~Engine();
    void initialize();
    void run();
    void shutdown();
    Console* getConsole();
    Scripting* getScripting();
    Graphics* getGraphics();
  };

}

extern Glacier::Engine* gEngine;