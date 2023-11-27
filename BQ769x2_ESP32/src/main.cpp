//Direccion del sensor BMP280: 0x76

//----------------------------------LIBRERIAS----------------------------------
//#include <Wire.h>
#include <Arduino.h>
#include <stdint.h>

#include "BQ76942.h"

//----------------------------------DEFINICIONES-------------------------------
#define BMS_address 0x08
// uint8_t checkConnection (uint8_t slave);


//----------------------------------PROTOTIPOS---------------------------------
void tryBuffSerial( uint8_t* data,  uint8_t size );

//----------------------------------VARIABLES----------------------------------
uint8_t check = 0;
uint16_t voltaje = 0;

bool bit = 0;

BQ76942 BMS1( BMS_address );

bool okflag = 0;

//----------------------------------CODIGO PRINCIPAL----------------------------------
void setup() {

  Serial.begin(115200);     //Inicia la comunicacion serial

  // tryBuffSerial(tryarray, 3);

  BMS1.begin();

  //Revisar la comunicacion entre el esp32 y el BQ76942
  check = BMS1.checkConnection();
  if( check == 0){
    Serial.println("Conexión exitosa");
    okflag = 1;
  }
  else{
    Serial.printf("Error de conexión, codigo: %d ", check);
    okflag = 0;
  }

  if(okflag) //Si la comunicacion es exitosa
  {
    //Probando la funcion readCellVoltage para leer voltajes de celdas
    Serial.println("Leyendo voltaje de celda 10 ");
    voltaje = BMS1.readCellVoltage(10);
    Serial.println(voltaje);

    //__Hacer una prueba de escritura a la configuracion del BQ76942__
    //Leer registros antes de la sobreescritura
    Serial.println("Lectura de <Enable protections A> antes del cambio:");
    Serial.println(BMS1.readEnableProtectionsA(), HEX);

    Serial.println("Modificando la configuración...");
    BMS1.defaultSettings();

    Serial.println("Lectura de <Enable protections A> después del cambio:");
    Serial.println(BMS1.readEnableProtectionsA(), HEX);

    //Realizar una prueba de lectura de subcomando
    Serial.println("Leyendo subcomando: Device Number");
    Serial.println(BMS1.readDeviceNo());

    //__________Pruebas de funciones de usuario___________
    
    Serial.println("Leyendo bit safetyAlertsA.COV:");
    bit = BMS1.safetyAlertA_bits().COV;
    Serial.println(bit);

    Serial.println("Leyendo bit safetyStatusA.OCD2:");
    bit = BMS1.safetyStatusA_bits().OCD2;
    Serial.println(bit);


    Serial.println("Leyendo bit safetyAlertsB.OTF:");
    bit = BMS1.safetyAlertB_bits().OTF;
    Serial.println(bit);

    Serial.println("Leyendo bit safetyStatusB.UTINT:");
    bit = BMS1.safetyStatusB_bits().UTINT;
    Serial.println(bit);

    //Modificar el registro Enable protections A
    Serial.println("Leyendo EnableProtectionsA antes del cambio:");
    Serial.println(BMS1.readEnableProtectionsA(), HEX);

    BMS1.writeEnableProtectionsA(0xBB);

    Serial.println("Leyendo EnableProtectionsA después del cambio:");
    Serial.println(BMS1.readEnableProtectionsA(), HEX);

    Serial.println(" ");
    Serial.println("__Realizar un cambio bitwise a Enable protections A__");
    Serial.println("Leyendo bit EnableProtectionsA.OCC antes del cambio:");
    bit = BMS1.readEnableProtectionsA_bits().OCC;
    Serial.println(bit);
    //Realizar un cambio bitwise a Enable protections A
    BMS1.overCurrenC(0);

    Serial.println("Leyendo bit EnableProtectionsA.OCC después del cambio:");
    bit = BMS1.readEnableProtectionsA_bits().OCC;
    Serial.println(bit);

    Serial.println("Nuevamente revertir dicho cambio y leer EnableProtectionsA.OCC:");
    BMS1.overCurrenC(1);
    bit = BMS1.readEnableProtectionsA_bits().OCC;
    Serial.println(bit);


  }




}
 
void loop() {
  Serial.println("FIN");
  while (1)
  {
    /* code */
  }
 
}



//----------------------------------FUNCIONES----------------------------------

void tryBuffSerial( uint8_t* data,  uint8_t size ){

  for (uint8_t i = 0; i < size; i++){
    Serial.println(data[i]);
  }

}