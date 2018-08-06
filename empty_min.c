/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

/* Example/Board Header files */
#include "Board.h"

//Library of mathematical functions
#include <math.h>

//_______________________Variables of the IMU____________________________________
//ACCELEROMETER
long acc_x;
long acc_y;
long acc_z;

//GYROSCOPE
long gyro_x;
long gyro_y;
long gyro_z;

//_______________I2C variables for reading data from the IMU_______________

uint8_t txBuffer[2]; //SHIPPING: the orders are always written in pairs, first the record we want to write and then what we write in the
uint8_t rxBuffer[14]; //READING: 14 bytes in the case of the IMU
I2C_Handle i2c;
I2C_Params i2cParams;
I2C_Transaction i2cTransaction;




#define TASKSTACKSIZE 640
#define PI 3.14159

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

/*
* ======== taskFxn ========
* Task for this function is created statically. See the project's .cfg file.
*/
Void taskFxn(UArg arg0, UArg arg1)
{
    //This thread will be in charge of all communication management and data capture with the IMU


    //Initialization of the IMU:

    System_printf("Initialize the IMU: \n");
    System_flush();
    InicializaIMU(); //Subroutine that initializes the IMU

}

/*
* ======== main ========
*/
int main(void)
{
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initI2C();

    /* Construct tmp006 Task thread */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)taskFxn, &taskParams, NULL);

    //In this way you have already initialized a thread that will take care of taking measures by I2C

    /* Turn on user LED */
    GPIO_write(Board_LED0, Board_LED_ON);




    /* SysMin will only print to the console when you call flush or exit */


    /* Start BIOS */
    BIOS_start();

    return (0);
}



//=======================================================================================================
//--------------------------- IMU calibration ------------------------------------------------------
//=======================================================================================================


void Calibracion () {
    //This subroutine is in charge of calibrating the GIROSCOPES of the IMU,
    //to see the measure in stopped and to be able to subtract it to the next measures:

    int LecturaCorrecta; //Variable that tells us if the reading has been correct or not If Correct Reading = 1 => Correct reading, YES Correct Reading = 0
                         //=> Failed reading At the moment we do not calibrate anything, we simply take 20 measurements:
    int i;
    for ( i = 0; i < 20; i++){
        LecturaCorrecta = LecturaDatosIMU();


        if (LecturaCorrecta ==1) {
            System_printf ("Lectura correcta número %u \n", i);
        }
        else {
            //In case it has not been a good read:
            System_printf ("Fallo en la lectura \n");
        }
        System_flush();
        Task_sleep(1000);
    }




}


//========================================================================================================
//------------------------------------ Reading data from the IMU --------------------------------------
//========================================================================================================

int LecturaDatosIMU(){
    //Function that is responsible for reading the data of the IMU and save them in global variables:

    int LecturaCorrecta;



    //===========================================================================
    //------------ SETUP test of the IMU ---------------------------------------
    //===========================================================================

    // We try to make 20 consecutive measurements
    /* Point to the T ambient register and read its 2 bytes */

    i2c = I2C_open(Board_I2C_IMU, &i2cParams);
    txBuffer[0] = 0x3B;
    i2cTransaction.slaveAddress = Board_IMU_ADDR;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 14;

    if (I2C_transfer(i2c, &i2cTransaction)) {
        //We read all the data of the IMU As all data have 16 bits we have to move the lower part and the upper part:

        acc_x = (rxBuffer[0] << 8) | (rxBuffer[1]);
        acc_y = (rxBuffer[2] << 8) | (rxBuffer[3]);
        acc_z = (rxBuffer[4] << 8) | (rxBuffer[5]);



        gyro_x = (rxBuffer[8] << 8) | (rxBuffer[9]);
        gyro_y = (rxBuffer[10] << 8) | (rxBuffer[11]);
        gyro_z = (rxBuffer[12] << 8) | (rxBuffer[13]);

        LecturaCorrecta = 1;

    }
    else {
        LecturaCorrecta = 0;
    }



    /* Deinitialized I2C */
    I2C_close(i2c);
    return LecturaCorrecta;
}

void InicializaIMU (UArg arg0, UArg arg1){
    //Function that is responsible for initializing the records of the IMU, to later only have to read the data of the same:
    /* Create I2C for usage */

    //Creation of the I2C object for communication:
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz; //Same frequency for the IMU
    i2c = I2C_open(Board_I2C_IMU, &i2cParams);
    if (i2c == NULL) {
        System_abort("Error inicializando el I2C \n");
    }
    else {
        System_printf("I2C inicializado \n");
    }
    I2C_close(i2c); //We close the channel

    i2c = I2C_open(Board_I2C_IMU, &i2cParams);
    //Start request:
    txBuffer[0] = 0x6B; //Start request registration
    txBuffer[1] = 0x00; //Lo inicializamo
    i2cTransaction.slaveAddress = Board_IMU_ADDR;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 2;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;

    //We tried to write this and see if it gives a fault:
    if (I2C_transfer(i2c, &i2cTransaction)) {
        //In case there was no failure
        System_printf("Se ha inicializado la IMU \n");
    }
    else {
        System_printf("I2C Bus fault\n");
    }

    System_flush();
    I2C_close(i2c); //We close the channel


    i2c = I2C_open(Board_I2C_IMU, &i2cParams);
    // INITIALIZATION OF THE ACCELEROMETER
    txBuffer[0] = 0x1C; //Initialization record of accelerometers
    txBuffer[1] = 0x10; //Los inicializamos

    //We try to write the configuration, checking if it fails
    if (I2C_transfer(i2c, &i2cTransaction)) {
        //In case there has been no failure
        System_printf("Inicialización acelerómetros \n");
    }
    else {
        System_printf("I2C Bus fault\n");
    }

    System_flush();
    I2C_close(i2c); //We close the channel

    i2c = I2C_open(Board_I2C_IMU, &i2cParams);
    // INITIALISATION OF THE GIROSCOPE
    txBuffer[0] = 0x1B; //Initialization record of the gyroscope
    txBuffer[1] = 0x08; //Los inicializamos

    //We try to give you the configuration, checking if it fails
    if (I2C_transfer(i2c, &i2cTransaction)) {
        //In case there has been no failure
        System_printf("Inicialización giroscopios \n");
    }
    else {
        System_printf("I2C Bus fault\n");
    }

    System_flush();

    //Once all the parameters are configured, we close the channel
    I2C_close(i2c);
    System_printf("I2C cerrado \n");

    //We call the calibration function, to get the average noise from the gyroscopes:
    Calibracion();
}
