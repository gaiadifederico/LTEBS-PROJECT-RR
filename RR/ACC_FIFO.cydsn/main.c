/* ========================================
 *
 * Legge sempre lo stesso valore cioè il primo
 * ad ogni overrun
 *
 * ========================================
*/
#include <stdio.h>
#include "project.h"
#include "../src_shared/I2C_Interface.h"
#include "../src_shared/LIS3DH.h"

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    I2C_Peripheral_Start();
    UART_1_Start();
    
    CyDelay(5); //"The boot procedure is complete about 5 ms after device power-up."
    

    char message[32];
    uint8_t control_reg;
    

    
    //selecting FIFO mode   
    control_reg = 0x40;
    ErrorCode error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             LIS3DH_FIFO_CTRL_REG,
                                             control_reg);
      //enable FIFO buffer FIFO_EN=1
    control_reg = 0x40;
    error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             LIS3DH_CTRL_REG5,
                                             control_reg);
    if (error == NO_ERROR)
    {
        sprintf(message, "FIFO_MODE enabled\r\n");
        UART_1_PutString(message);
    }
    else
    {
        UART_1_PutString("\r\nError occured during I2C comm to set control register 1\r\n");
    }
    //check if CTRL_REG5 is written correctly
    uint8_t check;
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS, 
                                        LIS3DH_CTRL_REG5,
                                        &check);
    if( error == NO_ERROR ) {
        sprintf(message, "CTRL_REG5 register value: 0x%02X [Expected value: 0x%02X]\r\n", check, control_reg);
        UART_1_PutString(message);
    }
    else {
        UART_1_PutString("I2C error while reading LIS3DH_WHO_AM_I_REG_ADDR\r\n");
    }
        

    //select CTRL_REG1 activate High Power Mode 10Hz and enable axis
    control_reg= 0x27;
    error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG1,
                                        control_reg);
    //reading CTRL_REG1
    uint8_t reg1;
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG1,
                                        &reg1);
    if( error == NO_ERROR ) {
        sprintf(message, "Valore CTRL_REG1 0x%02X\r\n", reg1);
        UART_1_PutString(message);
        }
 
 
    //set full scale +/-2g (no change from default)    
    control_reg = 0xCF;
    error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_CTRL_REG4,
                                         control_reg);
       
    //enable interrupt on pin INT1 --> CTRL_REG3, I1_OVERRUN=1
//    control_reg = LIS3DH_CTRL_REG3|0x02;
//    error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
//                                         LIS3DH_CTRL_REG3,
//                                         control_reg);
//     
        
        
//    /*READ DATI*/
    
    uint8_t x_data[2];
    int out_x;
    float data_x;
    
    uint8_t y_data[2];
    int out_y;
    float data_y;

    uint8_t z_data[2];
    int out_z;
    float data_z;

    uint8_t address;
    float lis_data[96];
    uint8_t overrun=0;
    uint8_t accelerazioni=0;
    uint8_t acc[96];
//    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
//                                        LIS3DH_CTRL_REG4,
//                                        &address);
//    if (error == NO_ERROR)
//        {
//            sprintf(message, "\r\nIl registro 0x%02X\r\n", address);
//            UART_1_PutString(message);
//        }
    
    uint8_t flag_dati =0;
    /*FINE READ DATI*/
//        
   for(;;)
    {
//        /* Place your application code here. */
    
//    //reset fifo: enable bypass mode
//    control_reg = LIS3DH_FIFO_CTRL_REG&0x3F;
//    error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
//                                             LIS3DH_FIFO_CTRL_REG,
//                                             control_reg);
//        //check FIFO status: 0x20 if empty
//    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
//                                        FIFO_SRC_REG,
//                                        &address);
        //controllo overrun 
        error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_FIFO_SRC_REG,
                                        &overrun);
       
        //sprintf(message, "\r\nOverrun:  %d\r\n", overrun&0x40);
        //UART_1_PutString(message);
        
        /*LETTURA DATI AD OGNI OVERRUN*/
        if((overrun&0x40)==0x40)
        {
                sprintf(message, "FIFO overrun");
                UART_1_PutString(message);
//            //reset fifo: enable bypass mode
//                    control_reg = LIS3DH_FIFO_CTRL_REG&0x3F;
//                    error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
//                                                             LIS3DH_FIFO_CTRL_REG,
//                                                             control_reg);

 // il problema è che non scorre la FIFO ma stampa lo stesso valore 96 volte
                 for(int i=0; i<96; i+=3){
                    //x 
                    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                        LIS3DH_OUT_X_L,
                                                        &x_data[0]);
                    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                        LIS3DH_OUT_X_H,
                                                        &x_data[1]);
                    out_x=(x_data[0]|(x_data[1]<<8));
                    lis_data[i]=out_x/64.0;
                    //y
                    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                        LIS3DH_OUT_Y_L,
                                                        &y_data[0]);
                    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                        LIS3DH_OUT_Y_H,
                                                        &y_data[1]);
                    out_y=(y_data[0]|(y_data[1]<<8));
                    lis_data[i+1]=out_y/64.0;

                    //z
                    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                        LIS3DH_OUT_Z_L,
                                                        &z_data[0]);
                    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                        LIS3DH_OUT_Z_H,
                                                        &z_data[1]);
                    out_z=(z_data[0]|(z_data[1]<<8));
                    lis_data[i+2]=out_z/64.0;
                    CyDelay(3);
                    

            //        acc[i] = (float)(tmp_int16[i]) * LIS3DH_ACC_CONVERT_2G;
            //        acc[i+1] = (float)(tmp_int16[i+1]) * LIS3DH_ACC_CONVERT_2G;
            //        acc[i+2] = (float)(tmp_int16[i+2]) * LIS3DH_ACC_CONVERT_2G;}
                    }
                    flag_dati=1;
                                     
                            
            //        //reset fifo: enable bypass mode
                    control_reg = LIS3DH_FIFO_CTRL_REG&0x3F;
                    error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                                             LIS3DH_FIFO_CTRL_REG,
                                                             control_reg);
                   //enable fifo mode  
                control_reg = LIS3DH_FIFO_CTRL_REG|0x40;
                error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                                 LIS3DH_FIFO_CTRL_REG,
                                                 control_reg);
                
            
        }
        /*fine overrun*/
        
        //stampa lis data
        if(flag_dati==1){
 
                flag_dati=0;
                sprintf(message, "\r\nVALORI ACCELERAZIONE\r\n");
                UART_1_PutString(message);

            for(int i=0; i<96; i+=3){
                sprintf(message, "\r\nx: %.3f, y: %.3f, z: %.3f\r\n", lis_data[i],
                 lis_data[i+1],
                 lis_data[i+2]);
                UART_1_PutString(message);

            }
            
            
            
        }
            
//    
//    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
//                                        LIS3DH_STATUS_REG,
//                                        &address);
//    if (error == NO_ERROR)
//        {
//            sprintf(message, "\r\nStatus register 0x%02X\r\n", address);
//            UART_1_PutString(message);
//        }
        

       CyDelay(100);
        }
}

/* [] END OF FILE */

