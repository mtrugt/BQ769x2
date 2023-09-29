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
     * @brief 
     * 
    */
    uint8_t ui2cWrite(uint8_t reg, uint8_t* data, uint8_t size);

    /**
     * @brief 
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

    void i2cWriteReg(uint8_t reg_addr, uint8_t *reg_data, uint8_t count);

    int i2cReadReg(uint8_t reg_addr, uint8_t *reg_data, uint8_t count);



 //_______________________________________FUNCIONES AVANZADAS________________________________________

    /**
     * @brief 
     * Funcion utilizada para realizar cambios en los registros de configuración.
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

    void subCommands(uint16_t command, uint16_t data, uint8_t type);

    void directCommands(uint8_t command, uint16_t data, uint8_t type);

    /**
     * @brief
     * Esta función permite configurar al integrado a ciertos valores por defecto  
     * que el usuario puede modificar a conveniencia en esta librería.
     * 
     * @note 
     * El formato de esta función es como se recomienda realizar una función
     * propia en el codigo principal en caso de necesitar realizar cambios en
     * los registros de configuración del integrado.
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

    uint16_t intTemp();

    uint16_t ts1Temp();

    uint16_t ts2Temp();

    uint16_t ts3Temp();

    //Funciones relacionadas a: Alertas y estados <A>
    uint8_t safetyAlertA();
    _safetyAlertA_bits safetyAlertA_bits();

    uint8_t safetyStatusA();
    _safetyStatusA_bits safetyStatusA_bits();


    
    //Funciones relacionadas a: Alertas y estados <B>
    uint8_t safetyAlertB();
    _safetyAlertB_bits safetyAlertB_bits();

    uint8_t safetyStatusB();
    _safetyStatusB_bits safetyStatusB_bits();

 //---------------------------------------------Subcommands--------------------------------------------

    uint16_t readDeviceNo ();



 //---------------------------------------------Data memory--------------------------------------------
    
    //Funciones relacionadas a: protecciones <A>
    uint8_t readEnableProtectionsA();
    _readEnableProtectionsA_bits readEnableProtectionsA_bits();
    void writeEnableProtectionsA(uint8_t value);
    void writeEnableProtectionsA_SCD(bool value);
    void writeEnableProtectionsA_OCD2(bool value);
    void writeEnableProtectionsA_OCD1(bool value);
    void writeEnableProtectionsA_OCC(bool value);
    void writeEnableProtectionsA_COV(bool value);
    void writeEnableProtectionsA_CUV(bool value);


    
};



#endif