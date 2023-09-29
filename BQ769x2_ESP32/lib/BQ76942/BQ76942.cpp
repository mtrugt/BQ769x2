
//____________________________________LIBRERIAS_____________________________________
#include "BQ76942.h"
#include <Arduino.h>

//___________________________________DEFINICIONES___________________________________
#define MAX_BUFFER_SIZE 10

#define R 0 // Read; Used in DirectCommands and Subcommands functions
#define W 1 // Write; Used in DirectCommands and Subcommands functions
#define W2 2 // Write data with two bytes; Used in Subcommands function
//----------------------------------VARIABLES----------------------------------
uint8_t RX_data [2] = {0x00, 0x00}; // used in several functions to store data read from BQ769x2
uint8_t RX_32Byte [32] = {0}; //used in Subcommands read function



//__________________________________CONSTRUCTORES E INICIALIZADORES_________________________________

BQ76942::BQ76942( uint8_t _bqAdress )
{
    bqAdress = _bqAdress;
}

void BQ76942::begin()
{
    Wire.begin();             //Inicia I2C en los pines por defecto: SDA = GPIO21, SCL = GPIO22
}



//_______________________________FUNCIONES REQUERIDAS PARA LA COMUNICACIÓN__________________________

unsigned char BQ76942::checkSum(unsigned char *ptr, unsigned char len)				//OK
{// Calculates the checkSum when writing to a RAM register. The checkSum is the inverse of the sum of the bytes.
	unsigned char i;
	unsigned char checkSum = 0;

	for(i=0; i<len; i++)
		checkSum += ptr[i];

	checkSum = 0xff & ~checkSum;

	return(checkSum);
}

unsigned char CRC8(unsigned char *ptr, unsigned char len)							
//Calculates CRC8 for passed bytes. Used in i2c read and write functions 
{
	unsigned char i;
	unsigned char crc=0;
	while(len--!=0)
	{
		for(i=0x80; i!=0; i/=2)
		{
			if((crc & 0x80) != 0)
			{
				crc *= 2;
				crc ^= 0x107;
			}
			else
				crc *= 2;

			if((*ptr & i)!=0)
				crc ^= 0x107;
		}
		ptr++;
	}
	return(crc);
}


uint8_t BQ76942::checkConnection()													//OK
{
    uint8_t comm_status;

    Wire.beginTransmission( bqAdress );            //Inicializar la comunicación
    comm_status = Wire.endTransmission();     //Terminar comunicación

    return comm_status;
}

uint8_t BQ76942::requestRegisters( uint8_t reg, uint8_t size )						//OK
{
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

uint8_t BQ76942::ui2cWrite( uint8_t reg, uint8_t* data,  uint8_t size)				//OK
{
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

uint8_t BQ76942::ui2cRead( uint8_t reg, uint8_t* data,  uint8_t size)				//OK
{
    uint8_t comm_status;
	uint8_t i = 0;

    comm_status = checkConnection();
    if ( comm_status != 0 ){                   //Si la comunicación no es exitosa
        return comm_status;                    //se interrumpe la ejecución.
    }

	requestRegisters(reg, size);			   //Solicitar los registros

	//Leer informacion
	while ( Wire.available() || i == size){
        data[i] = Wire.read();
        i++;
		delay(10);
    }

    return comm_status;

}

void BQ76942::i2cWriteReg(uint8_t reg_addr, uint8_t *reg_data, uint8_t count)		//OK
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
    ui2cWrite(reg_addr, reg_data, count);
			
#endif
}

int BQ76942::i2cReadReg(uint8_t reg_addr, uint8_t *reg_data, uint8_t count)			//OK
{
	unsigned int RX_CRC_Fail = 0;  // reset to 0. If in CRC Mode and CRC fails, this will be incremented.
	uint8_t RX_Buffer [MAX_BUFFER_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
#if CRC_Mode
	{
		uint8_t crc_count = 0;
		uint8_t ReceiveBuffer [10] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
		crc_count = count * 2;
		unsigned int j;
		unsigned int i;
		unsigned char CRCc = 0;
		uint8_t temp_crc_buffer [3];

		HAL_I2C_Mem_Read(&hi2c1, DEV_ADDR, reg_addr, 1, ReceiveBuffer, crc_count, 1000);
		uint8_t crc1stByteBuffer [4] = {0x10, reg_addr, 0x11, ReceiveBuffer[0]};
		CRCc = CRC8(crc1stByteBuffer,4);
		if (CRCc != ReceiveBuffer[1])
		{
			RX_CRC_Fail += 1;
		}
		RX_Buffer[0] = ReceiveBuffer[0];

		j = 2;
		for (i=1; i<count; i++)
		{
			RX_Buffer[i] = ReceiveBuffer[j];
			temp_crc_buffer[0] = ReceiveBuffer[j];
			j = j + 1;
			CRCc = CRC8(temp_crc_buffer,1);
			if (CRCc != ReceiveBuffer[j])
				RX_CRC_Fail += 1;
			j = j + 1;
		}
		CopyArray(RX_Buffer, reg_data, crc_count);
	}
#else
	ui2cRead(reg_addr, reg_data, count);
#endif
	return 0;
}



//_______________________________________FUNCIONES AVANZADAS________________________________________

void BQ76942::bqSetRegister(uint16_t reg_addr, uint32_t reg_data, uint8_t datalen)	//OK
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

void BQ76942::commandOnlySubcommands(uint16_t command) 								//OK
{	//For Command only Subcommands
	//For DEEPSLEEP/SHUTDOWN subcommand you will need to call this function twice consecutively
	// See the TRM or the BQ76952 header file for a full list of Command-only subcommands
	
	uint8_t TX_Reg[2] = {0x00, 0x00};

	//TX_Reg in little endian format
	TX_Reg[0] = command & 0xff;
	TX_Reg[1] = (command >> 8) & 0xff;

	i2cWriteReg( 0x3E, TX_Reg, 2 ); 
	delay(2);
}

void BQ76942::subCommands(uint16_t command, uint16_t data, uint8_t type)			//OK
{// See the TRM or the BQ76952 header file for a full list of Subcommands
	//security keys and Manu_data writes dont work with this function (reading these commands works)
	//max readback size is 32 bytes i.e. DASTATUS, CUV/COV snapshot
	uint8_t TX_Reg[4] = {0x00, 0x00, 0x00, 0x00};
	uint8_t TX_Buffer[2] = {0x00, 0x00};

	//TX_Reg in little endian format
	TX_Reg[0] = command & 0xff;
	TX_Reg[1] = (command >> 8) & 0xff;

	if (type == R) {//read
		i2cWriteReg(0x3E, TX_Reg, 2);
		delay(2000);
		i2cReadReg(0x40, RX_32Byte, 32); //RX_32Byte is a global variable
	}
	else if (type == W) {
		//FET_Control, REG12_Control
		TX_Reg[2] = data & 0xff; 
		i2cWriteReg(0x3E,TX_Reg,3);
		delay(1000);
		TX_Buffer[0] = checkSum(TX_Reg, 3);
		TX_Buffer[1] = 0x05; //combined length of registers address and data
		i2cWriteReg(0x60, TX_Buffer, 2);
		delay(1000); 
	}
	else if (type == W2){ //write data with 2 bytes
		//CB_Active_Cells, CB_SET_LVL
		TX_Reg[2] = data & 0xff; 
		TX_Reg[3] = (data >> 8) & 0xff;
		i2cWriteReg(0x3E,TX_Reg,4);
		delay(1000);
		TX_Buffer[0] = checkSum(TX_Reg, 4); 
		TX_Buffer[1] = 0x06; //combined length of registers address and data
		i2cWriteReg(0x60, TX_Buffer, 2);
		delay(1000); 
	}
}

void BQ76942::directCommands(uint8_t command, uint16_t data, uint8_t type)
// See the TRM or the BQ76952 header file for a full list of Direct Commands
{	//type: R = read, W = write
	uint8_t TX_data[2] = {0x00, 0x00};

	//little endian format
	TX_data[0] = data & 0xff;
	TX_data[1] = (data >> 8) & 0xff;

	if (type == R) {//Read
		i2cReadReg(command, RX_data, 2); //RX_data is a global variable
		delay(2000);
	}
	if (type == W) {//write
    //Control_status, alarm_status, alarm_enable all 2 bytes long
		i2cWriteReg(command,TX_data,2);
		delay(2000);
	}
}


void BQ76942::defaultSettings()				//OK
{

    // Ingresa el comando CONFIGUPDATE mode (Subcommand 0x0090)
	// - requerido para modifidar la configuración
	commandOnlySubcommands(SET_CFGUPDATE);

	//POWERCONFIG
	// Modificar inmediatamente los registros WK_SPD_x en 00 ya que la configuracion 
	// por defecto en 01 genera bugs.
	bqSetRegister(PowerConfig, 0x2980, 2);

    //PROTECCIONES A
	// Activa las protecciones: SCD (short-circuit), OCD1 (over-current in discharge), OCC (over-current in charge),
	// COV (over-voltage), CUV (under-voltage)
	bqSetRegister(EnabledProtectionsA, 0xBC, 1);

    // Salir de CONFIGUPDATE mode  - Subcommand 0x0092
	commandOnlySubcommands(EXIT_CFGUPDATE);

}


//_____________________________________FUNCIONES A NIVEL DE USUARIO__________________________________


//-----------------------------------Direct commands----------------------------------------------------

uint16_t BQ76942::readCellVoltage( uint8_t Ncell )			//OK
{
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

uint16_t BQ76942::intTemp()	
{
	int16_t temp = 0;
	uint8_t i = 0;
	uint8_t data[2] = {0};

	//Solicitar 2 bytes que contienen el valor de la
	// temperatura interna
	requestRegisters(IntTemperature, 2);

	while ( Wire.available() && i < 2){
        data[i] = Wire.read();
        i++;
    }

    temp = (data[1] << 8) | (data[0]);

    return temp;
}

uint16_t BQ76942::ts1Temp()	
{
	int16_t temp = 0;
	uint8_t i = 0;
	uint8_t data[2] = {0};

	//Solicitar 2 bytes que contienen el valor de la
	// temperatura interna
	requestRegisters(TS1Temperature, 2);

	while ( Wire.available() || i == 2){
        data[i] = Wire.read();
        i++;
    }

    temp = (data[1] << 8) | (data[0]);

    return temp;
}

uint16_t BQ76942::ts2Temp()	
{
	int16_t temp = 0;
	uint8_t i = 0;
	uint8_t data[2] = {0};

	//Solicitar 2 bytes que contienen el valor de la
	// temperatura interna
	requestRegisters(TS2Temperature, 2);

	while ( Wire.available() || i == 2){
        data[i] = Wire.read();
        i++;
    }

    temp = (data[1] << 8) | (data[0]);

    return temp;
}

uint16_t BQ76942::ts3Temp()	
{
	int16_t temp = 0;
	uint8_t i = 0;
	uint8_t data[2] = {0};

	//Solicitar 2 bytes que contienen el valor de la
	// temperatura interna
	requestRegisters(TS3Temperature, 2);

	while ( Wire.available() || i == 2){
        data[i] = Wire.read();
        i++;
    }

    temp = (data[1] << 8) | (data[0]);

    return temp;
}

//Funciones relacionadas a: Alerta, estado y protecciones A

uint8_t BQ76942::safetyAlertA()	
{
	uint8_t data = 0;
	uint8_t i = 0;

	//Solicitar informacion
	requestRegisters(SafetyAlertA, 1);

	//Leer informacion
	while ( Wire.available() && i < 1){
        data = Wire.read();
        i++;
    }

    return data;
}

_safetyAlertA_bits BQ76942::safetyAlertA_bits()	
{
	uint8_t data = safetyAlertA();
	_safetyAlertA_bits alerts(data);

	return alerts;
	
}

uint8_t BQ76942::safetyStatusA()	
{
	uint8_t data = 0;
	uint8_t i = 0;

	//Solicitar informacion
	requestRegisters(SafetyStatusA, 1);

	//Leer informacion
	while ( Wire.available() && i < 1){
        data = Wire.read();
        i++;
    }

    return data;
}

_safetyStatusA_bits BQ76942::safetyStatusA_bits()	
{
	uint8_t data = safetyStatusA();
	_safetyStatusA_bits alerts(data);

	return alerts;
	
}


uint8_t BQ76942::readEnableProtectionsA()			//OK
{
	uint8_t data = 0;

	subCommands(EnabledProtectionsA, 0, R);

	data = RX_32Byte[0];

	return data;
}

_readEnableProtectionsA_bits BQ76942::readEnableProtectionsA_bits()
{
	uint8_t data = readEnableProtectionsA();
	_readEnableProtectionsA_bits respuesta(data);

	return respuesta;

}

void BQ76942::writeEnableProtectionsA( uint8_t value)			//OK
{
	// Ingresa el comando CONFIGUPDATE mode (Subcommand 0x0090)
	// - requerido para modifidar la configuración
	commandOnlySubcommands(SET_CFGUPDATE);

	bqSetRegister(EnabledProtectionsA, value, 1);

	// Salir de CONFIGUPDATE mode  - Subcommand 0x0092
	commandOnlySubcommands(EXIT_CFGUPDATE);


}

void BQ76942::writeEnableProtectionsA_SCD(bool value) 
{	
	uint8_t respuesta = 0;
	uint8_t mask = 0b10000000;
	uint8_t anterior = readEnableProtectionsA();
	
	if (value == 0)
	{
		mask = ~mask;
		respuesta = anterior & mask;
	}
	else
	{
		respuesta = anterior | mask;
	}

	writeEnableProtectionsA(respuesta);

}

void BQ76942::writeEnableProtectionsA_OCD2(bool value) 
{	
	uint8_t respuesta = 0;
	uint8_t mask = 0b01000000;
	uint8_t anterior = readEnableProtectionsA();
	
	if (value == 0)
	{
		mask = ~mask;
		respuesta = anterior & mask;
	}
	else
	{
		respuesta = anterior | mask;
	}

	writeEnableProtectionsA(respuesta);

}

void BQ76942::writeEnableProtectionsA_OCD1(bool value) 
{	
	uint8_t respuesta = 0;
	uint8_t mask = 0b00100000;
	uint8_t anterior = readEnableProtectionsA();
	
	if (value == 0)
	{
		mask = ~mask;
		respuesta = anterior & mask;
	}
	else
	{
		respuesta = anterior | mask;
	}

	writeEnableProtectionsA(respuesta);

}

void BQ76942::writeEnableProtectionsA_OCC(bool value) 
{	
	uint8_t respuesta = 0;
	uint8_t mask = 0b00010000;
	uint8_t anterior = readEnableProtectionsA();
	
	if (value == 0)
	{
		mask = ~mask;
		respuesta = anterior & mask;
	}
	else
	{
		respuesta = anterior | mask;
	}

	writeEnableProtectionsA(respuesta);

}

void BQ76942::writeEnableProtectionsA_COV(bool value) 
{	
	uint8_t respuesta = 0;
	uint8_t mask = 0b00001000;
	uint8_t anterior = readEnableProtectionsA();
	
	if (value == 0)
	{
		mask = ~mask;
		respuesta = anterior & mask;
	}
	else
	{
		respuesta = anterior | mask;
	}

	writeEnableProtectionsA(respuesta);

}

void BQ76942::writeEnableProtectionsA_CUV(bool value) 
{	
	uint8_t respuesta = 0;
	uint8_t mask = 0b00000100;
	uint8_t anterior = readEnableProtectionsA();
	
	if (value == 0)
	{
		mask = ~mask;
		respuesta = anterior & mask;
	}
	else
	{
		respuesta = anterior | mask;
	}

	writeEnableProtectionsA(respuesta);

}


//Funciones relacionadas a: Alerta, estado y protecciones B
uint8_t BQ76942::safetyAlertB()	
{
	uint8_t data = 0;
	uint8_t i = 0;

	//Solicitar informacion
	requestRegisters(SafetyAlertB, 1);

	//Leer informacion
	while ( Wire.available() && i < 1){
        data = Wire.read();
        i++;
    }

    return data;
}

_safetyAlertB_bits BQ76942::safetyAlertB_bits()	
{
	uint8_t data = safetyAlertB();
	_safetyAlertB_bits alerts(data);

	return alerts;
	
}

uint8_t BQ76942::safetyStatusB()	
{
	uint8_t data = 0;
	uint8_t i = 0;

	//Solicitar informacion
	requestRegisters(SafetyStatusB, 1);

	//Leer informacion
	while ( Wire.available() && i < 1){
        data = Wire.read();
        i++;
    }

    return data;
}

_safetyStatusB_bits BQ76942::safetyStatusB_bits()	
{
	uint8_t data = safetyStatusB();
	_safetyStatusB_bits alerts(data);

	return alerts;
	
}








//Relacionado a subcomandos

uint16_t BQ76942::readDeviceNo()			//OK
{
	uint16_t id = 0;

	subCommands(DEVICE_NUMBER, 0, R);

	id = (RX_32Byte[1] << 8) | RX_32Byte[0];

	return id;
}





