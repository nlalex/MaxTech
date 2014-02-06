#ifndef NODE_H
#define NODE_H

#include <WProgram.h>

class NODE {
  public:
    NODE(str address);
    ~NODE();
    void log();
    void flush();
  private:
    str _address;
    float temp[];
    float hum[];
    int light1[];
    int light2[];
};

#endif
    
