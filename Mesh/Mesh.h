#ifndef Mesh_h
#define Mesh_h

#include <WProgram.h>
#include <XBee.h>
#include "Config.h"

class Mesh {
  public:
    Node(char address[]);
    ~Node();
    void stash();
    void flush();
    float convertTemp(int temp);
    float convertHum(int hum);
  private:
    char _address[][];
    float temp[][];
    float hum[][];
    int light1[][];
    int light2[][];
};

#endif
