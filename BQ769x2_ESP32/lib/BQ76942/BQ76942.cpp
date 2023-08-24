#include "BQ76942.h"

BQ76942::BQ76942( uint8_t _bqAdress ){
    bqAdress = _bqAdress;
}


void BQ76942::begin(){
    Wire.begin();             //Inicia I2C en los pines por defecto: SDA = GPIO21, SCL = GPIO22
}


uint8_t BQ76942::checkConnection(){
    uint8_t comm_status;

    Wire.beginTransmission( bqAdress );            //Inicializar la comunicación
    comm_status = Wire.endTransmission();     //Terminar comunicación

    return comm_status;
}


uint8_t BQ76942::requestRegisters( uint8_t reg, uint8_t size ){
    uint8_t comm_status;
    
    comm_status = checkConnection();
    if ( comm_status != 0 ){                   //Si la comunicación no es exitosa
        return comm_status;                   //se interrumpe la ejecución.
    }

    Wire.beginTransmission( bqAdress );            //Inicializar la comunicación.
    Wire.write( reg );                          //Ubicarse en la dirección del registro.
    Wire.endTransmission();                   //Terminar comunicación.
    
    Wire.requestFrom( bqAdress, size );            //Solicitar registros al esclavo.

    return comm_status;                       
}


uint8_t BQ76942::write2Register( uint8_t reg, uint8_t data ){
    uint8_t comm_status;

    comm_status = checkConnection();
    if ( comm_status != 0 ){                   //Si la comunicación no es exitosa
        return comm_status;                    //se interrumpe la ejecución.
    }

    Wire.beginTransmission( bqAdress );        //Inicializar la comunicación.
    Wire.write( reg );                         //Ubicarse en la dirección del registro.
    Wire.write( data );                        //Escribir el dato al registro.
    Wire.endTransmission();                    //Terminar comunicación.

    return comm_status;

}


uint16_t BQ76942::readCellVoltage( uint8_t Ncell ){
    uint8_t cell_adrs[10] = {0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x26};
    uint16_t data[2] = {0, 0};
    uint8_t i = 0;
    uint16_t voltage = 0;

    if( Ncell<1 || Ncell>10 ){
        return 0xFFFF;
    }

    //Solicita 2 bytes de información al esclavo los cuales
    //contienen el valor de voltaje de la celda seleccionada.
    requestRegisters(cell_adrs[Ncell-1], 2);

    while ( Wire.available() || i == 2){
        data[i] = Wire.read();
        i++;
    }

    voltage = (data[1] << 8) | (data[0]);

    return voltage;
}


// A partir de este punto el código está basado en los ejemplos proporcionados por el fabricante
// y modificado para poder ser utilizado en el microcontrolador esp32.

