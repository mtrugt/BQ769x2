/**
 * Libreria para utilizar el integrado BQ76942 con un microcontrolador esp32.
*/

//----------------------------------LIBRERIAS----------------------------------
#include "BQ76942.h"


//----------------------------------DEFINICIONES-------------------------------
#define MAX_BUFFER_SIZE 10

//----------------------------------VARIABLES----------------------------------



//----------------------------------CODIGO PRINCIPAL----------------------------------

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


uint8_t BQ76942::i2cWrite( uint8_t reg, uint8_t* data,  uint8_t size){
    uint8_t comm_status;

    comm_status = checkConnection();
    if ( comm_status != 0 ){                   //Si la comunicación no es exitosa
        return comm_status;                    //se interrumpe la ejecución.
    }

    Wire.beginTransmission( bqAdress );        //Inicializar la comunicación.
    Wire.write( reg );                         //Ubicarse en la dirección del registro.
    //Escribir la información
    for (uint8_t i = 0; i < size; i++){
        Wire.write( data[i] );                 //Enviar los datos
    }

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

unsigned char checkSum(unsigned char *ptr, unsigned char len)
// Calculates the checkSum when writing to a RAM register. The checkSum is the inverse of the sum of the bytes.	
{
	unsigned char i;
	unsigned char checkSum = 0;

	for(i=0; i<len; i++)
		checkSum += ptr[i];

	checkSum = 0xff & ~checkSum;

	return(checkSum);
}



void BQ76942::i2cWriteReg(uint8_t reg_addr, uint8_t *reg_data, uint8_t count)
{
	uint8_t TX_Buffer [MAX_BUFFER_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
#if CRC_Mode
	{
		uint8_t crc_count = 0;
		crc_count = count * 2;
		uint8_t crc1stByteBuffer [3] = {0x10, reg_addr, reg_data[0]};
		unsigned int j;
		unsigned int i;
		uint8_t temp_crc_buffer [3];

		TX_Buffer[0] = reg_data[0];
		TX_Buffer[1] = CRC8(crc1stByteBuffer,3);

		j = 2;
		for(i=1; i<count; i++)
		{
			TX_Buffer[j] = reg_data[i];
			j = j + 1;
			temp_crc_buffer[0] = reg_data[i];
			TX_Buffer[j] = CRC8(temp_crc_buffer,1);
			j = j + 1;
		}
		HAL_I2C_Mem_Write(&hi2c1, DEV_ADDR, reg_addr, 1, TX_Buffer, crc_count, 1000);
	}
#else 
    i2cWrite(reg_addr, reg_data, count);
			
#endif
}


void BQ76942::bqSetRegister(uint16_t reg_addr, uint32_t reg_data, uint8_t datalen)
{
	uint8_t TX_Buffer[2] = {0x00, 0x00};
	uint8_t TX_RegData[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	//TX_RegData in little endian format
	TX_RegData[0] = reg_addr & 0xff; 
	TX_RegData[1] = (reg_addr >> 8) & 0xff;
	TX_RegData[2] = reg_data & 0xff; //1st byte of data

	switch(datalen)
    {
		case 1: //1 byte datalength
      		i2cWriteReg(0x3E, TX_RegData, 3);
			delay(2);
			TX_Buffer[0] = checkSum(TX_RegData, 3); 
			TX_Buffer[1] = 0x05; //combined length of register address and data
      		i2cWriteReg(0x60, TX_Buffer, 2); // Write the checkSum and length
			delay(2);
			break;
		case 2: //2 byte datalength
			TX_RegData[3] = (reg_data >> 8) & 0xff;
			i2cWriteReg(0x3E, TX_RegData, 4);
			delay(2);
			TX_Buffer[0] = checkSum(TX_RegData, 4); 
			TX_Buffer[1] = 0x06; //combined length of register address and data
      		i2cWriteReg(0x60, TX_Buffer, 2); // Write the checkSum and length
			delay(2);
			break;
		case 4: //4 byte datalength, Only used for CCGain and Capacity Gain
			TX_RegData[3] = (reg_data >> 8) & 0xff;
			TX_RegData[4] = (reg_data >> 16) & 0xff;
			TX_RegData[5] = (reg_data >> 24) & 0xff;
			i2cWriteReg(0x3E, TX_RegData, 6);
			delay(2);
			TX_Buffer[0] = checkSum(TX_RegData, 6); 
			TX_Buffer[1] = 0x08; //combined length of register address and data
      		i2cWriteReg(0x60, TX_Buffer, 2); // Write the checkSum and length
			delay(2);
			break;
    }
}

void BQ76942::CommandSubcommands(uint16_t command) //For Command only Subcommands
// See the TRM or the BQ76952 header file for a full list of Command-only subcommands
{	//For DEEPSLEEP/SHUTDOWN subcommand you will need to call this function twice consecutively
	
	uint8_t TX_Reg[2] = {0x00, 0x00};

	//TX_Reg in little endian format
	TX_Reg[0] = command & 0xff;
	TX_Reg[1] = (command >> 8) & 0xff;

	i2cWriteReg( 0x3E, TX_Reg, 2 ); 
	delay(2);
}






void BQ76942::defaultSettings(){
    // Enter CONFIGUPDATE mode (Subcommand 0x0090) - It is required to be in CONFIG_UPDATE mode to program the device RAM settings
	// See TRM for full description of CONFIG_UPDATE mode
	CommandSubcommands(SET_CFGUPDATE);

    // Enable protections in 'Enabled Protections A' 0x9261 = 0xBC
	// Enables SCD (short-circuit), OCD1 (over-current in discharge), OCC (over-current in charge),
	// COV (over-voltage), CUV (under-voltage)
	bqSetRegister(EnabledProtectionsA, 0xBC, 1);

    // Exit CONFIGUPDATE mode  - Subcommand 0x0092
	CommandSubcommands(EXIT_CFGUPDATE);

}