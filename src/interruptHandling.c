#include "interruptHandling.h"
#include <stdio.h>
#include "cpu_t.h"

enum INTREQ_TYPE
{
    INTREQ_VBLANK,
    INTREQ_LCDSTAT,
    INTREQ_TIMER,
    INTREQ_SERIAL,
    INTREQ_JOYPAD
};
typedef enum INTREQ_TYPE INTREQ_TYPE;


void handleInterrupts(CPU* _cpu, uint8_t* _memory)
{
    
    for (uint8_t i = 0; i<5; i++)
    {
        uint8_t isRequested = *_cpu->regs.IF & (1 << i);
        if(isRequested == 0)
            continue;
        
        switch(i)
        {
            case INTREQ_VBLANK: //V-Blank  Interrupt Request (INT 40h)
            {
                //TODO disable interrupts and push the current value of PC onto the stack (not sure if this needs to use up cycles)
                //TODO transfer control to the interrupt handling routine in the memory using a jump
                
                //transfer controll to address 0x0040
                
                //_cpu->regs.IME = 0;//disable interrupts while servicing an iterrupt
                //_cpu->regs.IME = 1;//reenable interrupts //Interrupts probably get renabled by the interrupt handler which probably calls RETI
                break;
            }
            
            case INTREQ_LCDSTAT: //LCD STAT Interrupt Request (INT 48h)
            {
                //transfer controll to address 0x0048
                break;
            }
            
            case INTREQ_TIMER: //Timer    Interrupt Request (INT 50h)
            {

                //transfer controll to address 0x0050
                break;
            }
            case INTREQ_SERIAL: //Serial   Interrupt Request (INT 58h)
            {
                //transfer controll to address 0x0058
                break;
            }
            
            case INTREQ_JOYPAD: //Joypad   Interrupt Request (INT 60h)
            {
                //transfer controll to address 0x0060
                break;
            }
            
            default:
            {
                printf("ERROR: interrupt %d undefined\n", i);
                break;
            }
        }
    }
    
}

