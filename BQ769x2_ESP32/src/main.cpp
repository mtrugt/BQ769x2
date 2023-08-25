//Direccion del sensor BMP280: 0x76

//----------------------------------LIBRERIAS----------------------------------
#include <Wire.h>
#include <Arduino.h>
#include <stdint.h>

#include "BQ76942.h"

//----------------------------------DEFINICIONES-------------------------------
#define BMS_address 0x08
// uint8_t checkConnection (uint8_t slave);


//----------------------------------PROTOTIPOS---------------------------------
void tryBuffSerial( uint8_t* data,  uint8_t size );

//----------------------------------VARIABLES----------------------------------
uint8_t i2c_status = 0;
uint8_t i2c_data = 0;
uint8_t check = 0;
uint16_t voltaje = 0;

uint8_t i = 0;
uint8_t data = 0;

BQ76942 BMS1( BMS_address );

uint8_t tryarray[] = {0x01, 0x02, 0x03};



//----------------------------------CODIGO PRINCIPAL----------------------------------
void setup() {

  Serial.begin(115200);     //Inicia la comunicacion serial

  // tryBuffSerial(tryarray, 3);

  BMS1.begin();

  //Revisar la comunicacion entre el esp32 y el BQ76942
  check = BMS1.checkConnection();
  if( check == 0){
    Serial.println("Conexión exitosa");
  }
  else{
    Serial.printf("Error de conexión, codigo: %d ", check);
  }

  //Probando la funcion readCellVoltage para leer voltajes de celdas
  Serial.println("Leyendo registro ");
  voltaje = BMS1.readCellVoltage(10);
  Serial.println(voltaje, HEX);

  //Hacer una prueba de escritura a los registros del BQ76942
  Serial.println("Leyendo registro Protecciones A ");
  BMS1.requestRegisters(0x02, 1);
  while ( Wire.available() ){
        data = Wire.read();
        Serial.println(data, BIN);
        // i++;
  }

  BMS1.defaultSettings();

 
  
  Serial.println("Leyendo registro Protecciones A modificado");
  BMS1.requestRegisters(0x02, 1);
  while ( Wire.available() ){
        data = Wire.read();
        Serial.println(data, BIN);
        // i++;
  }



}
 
void loop() {
  delay(5000);
 
}



//----------------------------------FUNCIONES----------------------------------

void tryBuffSerial( uint8_t* data,  uint8_t size ){

  for (uint8_t i = 0; i < size; i++){
    Serial.println(data[i]);
  }

}