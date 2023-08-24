//Direccion del sensor BMP280: 0x76

//----------------------------------LIBRERIAS----------------------------------
#include <Wire.h>
#include <Arduino.h>
#include <stdint.h>

#include "BQ76942.h"


// extern "C" {
// #include <BQ76942.h>
// }




//----------------------------------DEFINICIONES-------------------------------
#define BMS_address 0x08
// uint8_t checkConnection (uint8_t slave);

//----------------------------------VARIABLES----------------------------------
uint8_t i2c_status = 0;
uint8_t i2c_data = 0;
uint8_t check = 0;
uint16_t voltaje = 0;

uint8_t i = 0;
uint8_t data = 0;

BQ76942 BMS1( BMS_address );





//----------------------------------CODIGO PRINCIPAL----------------------------------
void setup() {

  Serial.begin(115200);     //Inicia la comunicacion serial
  BMS1.begin();

  check = BMS1.checkConnection();
  if( check == 0){
    Serial.println("Conexión exitosa");
  }
  else{
    Serial.printf("Error de conexión, codigo: %d ", check);
  }

  Serial.println("Leyendo registro ");
  voltaje = BMS1.readCellVoltage(10);
  Serial.println(voltaje, HEX);


  Serial.println("Leyendo registro Protecciones A ");
  BMS1.requestRegisters(0x02, 1);
  while ( Wire.available() ){
        data = Wire.read();
        Serial.println(data, BIN);
        //i++;
  }

  BMS1.write2Register(0x02, 0b00000100);
  
  Serial.println("Leyendo registro Protecciones A modificado");
  BMS1.requestRegisters(0x02, 1);
  while ( Wire.available() ){
        data = Wire.read();
        Serial.println(data, BIN);
        //i++;
  }

  //BMS1.write2Register();

}
 
void loop() {
  delay(5000);
 
}