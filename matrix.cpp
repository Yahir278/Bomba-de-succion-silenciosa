#include "matrix.h"

const char keys[countRows][countColumns] = {
    { '1','2','3', 'A' },
    { '4','5','6', 'B' },
    { '7','8','9', 'C' },
    { '#','0','*', 'D' }
};

void initTeclado() {
    for (byte i = 0; i < countRows; i++) {
        pinMode(rowsPins[i], OUTPUT);
        digitalWrite(rowsPins[i], HIGH); 
    }
    for (byte i = 0; i < countColumns; i++) {
        pinMode(columnsPins[i], INPUT_PULLUP); 
    }
}

char ingresoTeclado() {
    char teclaPresionada = '\0'; 

    for (byte r = 0; r < countRows; r++) {
        digitalWrite(rowsPins[r], LOW);

        for (byte c = 0; c < countColumns; c++) {
            
            if (digitalRead(columnsPins[c]) == LOW) { 
                
                delay(50); 
                
                if (digitalRead(columnsPins[c]) == LOW) {
                    teclaPresionada = keys[r][c];

                    while (digitalRead(columnsPins[c]) == LOW) {
                        delay(10);
                    }
                }
            }
        }
        digitalWrite(rowsPins[r], HIGH); 
    }

    return teclaPresionada;
}