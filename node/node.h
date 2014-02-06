#ifndef node_h
#define node_h

#include <WProgram.h>

class node {
  public:
    node(str address);
    ~node();
    void note();
    void flush();
  private:
    str _address;
    float temp[];
    float hum[];
    int light1[];
    int light2[];
};

#endif
    
