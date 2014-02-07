#include "Mesh.h"

Mesh::Mesh(char address[]) {
  _address = address;
}

Mesh::~Mesh() {}

void Mesh::stash() {

}

void Mesh::flush() {

}

float Mesh::convertTemp(int temp) {
  return float(temp);
}

float Mesh::convertHum(int hum) {
  return float(hum);
}
