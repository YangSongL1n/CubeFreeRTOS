#ifndef __GLOBAL_H_
#define __GLOBAL_H_


#include "main.h"
#include "stdio.h"
#include "stdint.h"
typedef enum{
  Sta1=0,
  Sta2,
  Sta3
} My_State;
typedef struct 
{
        uint16_t disp_vol;
        My_State stpe;
}LCD_Data_t;



#endif
