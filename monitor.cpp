#include "monitor.h"
#include <Arduino.h>

int monitorOut() {
    if ((digitalRead(2) == HIGH) || (digitalRead(3) == HIGH)) {
        return -1; 
    } else {
        return analogRead(A1); 
    }
}