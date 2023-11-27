#ifndef __BQ76942_H__
#define __BQ76942_H__

  
/*Esta libreria incluye distintas funciones para poder operar
el integrado BQ76942 con mayor falicidad.*/




//----------------------------------LIBRERIAS----------------------------------
#include <Wire.h>
#include <stdint.h>
#include "structs&def.h"
#include <string>

//----------------------------------CLASES----------------------------------
class BQ76942{
private:
    
    uint8_t bqAdress; ///< Direccion I2C del integrado BQ76942

    /**
     * @brief Funcion requerida para la escritura I2C mediante libreria Wire. No
     * disponible para interaccion con el usuario.
     * 
    */
    uint8_t ui2cWrite(uint8_t reg, uint8_t* data, uint8_t size);

    /**
     * @brief Funcion requerida para la lectura I2C mediante libreria Wire. No
     * disponible para interaccion con el usuario.
     * 
    */
    uint8_t ui2cRead(uint8_t reg, uint8_t* data, uint8_t size);

public:
    
 //__________________________________CONSTRUCTORES E INICIALIZADORES_________________________________
    /**
     * @brief 
     * Este es el constructor para crear un objeto del tipo BQ76942
     * 
     * @param _bqAddres Dirección I2C del integrado BQ76942.
     * 
    */
    BQ76942( uint8_t _bqAdress );

    /**
     * @brief
     * Esta función permite inicializar todos los elementos necesarios para el correcto funcionamiento del 
     * integrado BQ76942.
     * 
    */
    void begin();
    
 //_______________________________FUNCIONES REQUERIDAS PARA LA COMUNICACIÓN__________________________

    unsigned char checkSum(unsigned char *ptr, unsigned char len);

    /**
     * @brief
     * Esta función se utiliza para conocer el estado de conexión
     * entre el dispositivo maestro y esclavo.
     * 
     * @return 
     *  0: Comunicación exitosa.
     *  1: Cantidad de datos mayor que el permitido por el buffer.
     *  2: NACK recibido durante la transmision de la direccción.
     *  3: NACK recibido durante la transmision de información.
     *  4: Error desconocido.
     *  5: Timeout.
     * 
     */
    uint8_t checkConnection ();

    /**
     * @brief Con esta función se pueden solicitar los registros que se deseen 
     * leer del dispositivo esclavo.
     * @note Es necesario utilizar posteriormente la función Wire.read() para
     * leer los datos solicitados.
     * 
     * @param reg   Dirección de registro al cual se desea realizar la lectura.
     * @param size  Cantidad de bytes a solicitar.
     * 
     * 
     * @return
     * 0: Comunicación exitosa. 
     * 1: Cantidad de datos mayor que el permitido por el buffer.
     * 2: NACK recibido durante la transmision de la direccción.
     * 3: NACK recibido durante la transmision de información.
     * 4: Error desconocido.
     * 5: Timeout. 
     */
    uint8_t requestRegisters (uint8_t reg, uint8_t size );
   
    /**
     * @brief Funcion requerida para la escritura I2C.
     * 
    */
    void i2cWriteReg(uint8_t reg_addr, uint8_t *reg_data, uint8_t count);

    /**
     * @brief Funcion requerida para la lectura I2C.
     * 
    */
    int i2cReadReg(uint8_t reg_addr, uint8_t *reg_data, uint8_t count);

 //_______________________________________FUNCIONES AVANZADAS________________________________________

    /**
     * @brief 
     * Funcion utilizada para realizar cambios en los registros de configuración.
     * 
     * @param reg_addr Dirección del registro que se desea modificar.
     * @param reg_data Nuevo valor.
     * @param datalen Número de bytes a enviar.
    */
    void bqSetRegister(uint16_t reg_addr, uint32_t reg_data, uint8_t datalen);

    /**
     * @brief 
     * Funcion para enviar comandos.
     * 
     * @return Las opciones de comandos son:
     *   - #Config
     *   - #update
    */
    void commandOnlySubcommands(uint16_t command);

    /**
     * @brief 
     * Funcion utilizada para la lectura/escritura a los registros de subcomandos
     * del integrado BQ76942.
     * 
     * @param command Comando el cual se quiere leer/escribir.
     * @param reg_data Nuevo valor (no importa que se coloque en este 
     * parámetro si la instruccion es de lectura).
     * @param type R(o bien 0) para lectura y W(o bien 1) para escritura.
    */
    void subCommands(uint16_t command, uint16_t data, uint8_t type);


    /**
     * @brief 
     * Funcion utilizada para la lectura/escritura a los registros de comandos
     * directos del integrado BQ76942.
     * 
     * @param command Comando el cual se quiere leer/escribir.
     * @param reg_data Nuevo valor (no importa que se coloque en este 
     * parámetro si la instruccion es de lectura).
     * @param type R(o bien 0) para lectura y W(o bien 1) para escritura.
    */
    void directCommands(uint8_t command, uint16_t data, uint8_t type);

    /**
     * @brief
     * Esta función configura al integrado para trabajar con el banco de baterias
     * propuesto durante la elaboracion de la tesis el cual utiliza 5 celdas modelo
     * B30 en serie.
     * 
     * @note 
     * Se puede utilizar esta función como ejemplo para elaborar una distinta
     * en el codigo principal en caso de necesitar realizar cambios en
     * los registros de configuración del integrado a un nivel de usuario "avanzado".
     * O bien, modificar esta misma en la libreria para que se ajuste a las necesidades
     * del usuario.
     * 
    */
    void defaultSettings();

    
 //_____________________________________FUNCIONES A NIVEL DE USUARIO__________________________________


 //-------------------------------------------Direct commands------------------------------------------

    /**
     * @brief Con esta función se puede leer el valor de voltaje
     * de una celda en específico en mV.
     * 
     * @param Ncell Celda de la cual se desea conocer el voltaje, el valor puede
     * estar entre 1-10.
     * 
    */
    uint16_t readCellVoltage ( uint8_t Ncell );

    /**
     * @brief Con esta función se puede leer el valor de la temperatura interna
     * del integrado.
    */
    uint16_t intTemp();

    /**
     * @brief Con esta función se puede leer el valor de la temperatura
     * del termistor TS1
     * .
    */
    uint16_t ts1Temp();

    /**
     * @brief Con esta función se puede leer el valor de la temperatura
     * del termistor TS2
     * .
    */
    uint16_t ts2Temp();
    
    /**
     * @brief Con esta función se puede leer el valor de la temperatura
     * del termistor TS3
     * .
    */
    uint16_t ts3Temp();

    //Funciones relacionadas a: Alertas y estados <A>

    /**
     * @brief Con esta función se puede leer el valor del registro safety Alerts A.
    */
    uint8_t safetyAlertA();
    /**
     * @brief Con esta estructura se pueden leer los valores de cada uno de
     * los bits por separado del registro safety Alerts A.
    */
    _safetyAlertA_bits safetyAlertA_bits();


    /**
     * @brief Con esta función se puede leer el valor del registro safety Status A.
    */
    uint8_t safetyStatusA();
    /**
     * @brief Con esta estructura se pueden leer los valores de cada uno de
     * los bits por separado del registro safety Status A.
    */
    _safetyStatusA_bits safetyStatusA_bits();


    
    //Funciones relacionadas a: Alertas y estados <B>
    /**
     * @brief Con esta función se puede leer el valor del registro safety Alert B.
    */
    uint8_t safetyAlertB();
    /**
     * @brief Con esta estructura se pueden leer los valores de cada uno de
     * los bits por separado del registro safety Alert B.
    */
    _safetyAlertB_bits safetyAlertB_bits();


    /**
     * @brief Con esta función se puede leer el valor del registro safety Status B.
    */
    uint8_t safetyStatusB();
    /**
     * @brief Con esta estructura se pueden leer los valores de cada uno de
     * los bits por separado del registro safety Status B.
    */
    _safetyStatusB_bits safetyStatusB_bits();

 //---------------------------------------------Subcommands--------------------------------------------

   /**
     * @brief Con esta función se puede leer el valor del id del integrado.
    */
   uint16_t readDeviceNo ();



 //---------------------------------------------Data memory--------------------------------------------
    
    //Funciones relacionadas a: protecciones <A>

    /**
     * @brief Con esta función se puede leer el valor del registro Enable protections A.
    */
    uint8_t readEnableProtectionsA();
    /**
     * @brief Con esta estructura se pueden leer los valores de cada uno de
     * los bits por separado del registro Enable protections A.
    */
    _readEnableProtectionsA_bits readEnableProtectionsA_bits();

    /**
     * @brief Con esta función se puede sobreescribir el valor del registro Enable protections A.
     * 
     * @param value Valor que se desea escribir al registro.
    */
    void writeEnableProtectionsA(uint8_t value);

    
    /**
     * @brief Funcion para activar o desactivar la proteccion por
     * corto circuito durante la descarga del registro Enable protections A.
     * 
     * @param value 0 para desactivar y 1 para activar.
    */
    void shortCircuitD(bool value);

    /**
     * @brief Funcion para activar o desactivar la proteccion por
     * sobre corriente durante la descarga 1 del registro Enable protections A.
     * 
     * @param value 0 para desactivar y 1 para activar.
    */
    void overCurrentD2(bool value);

    /**
     * @brief Funcion para activar o desactivar la proteccion por
     * sobre corriente durante la descarga 2 del registro Enable protections A.
     * 
     * @param value 0 para desactivar y 1 para activar.
    */
    void overCurrentD1(bool value);

    /**
     * @brief Funcion para activar o desactivar la proteccion por
     * sobre corriente durante la carga (Enable protections A).
     * 
     * @param value 0 para desactivar y 1 para activar.
    */
    void overCurrenC(bool value);

    /**
     * @brief Funcion para activar o desactivar la proteccion por
     * sobre voltaje de celda (Enable protections A).
     * 
     * @param value 0 para desactivar y 1 para activar.
    */
    void cellOverVolt(bool value);

    /**
     * @brief Funcion para activar o desactivar la proteccion por
     * bajo voltaje de celda (Enable protections A).
     * 
     * @param value 0 para desactivar y 1 para activar.
    */
    void cellUnderVolt(bool value);


    
};



#endif