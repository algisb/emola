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

void jumpToInterruptHandler(CPU* _cpu, uint8_t* _memory, uint16_t _address)
{
    //disable interrupts while servicing an iterrupt
    _cpu->regs.IME = 0;
    //push PC to stack
    _cpu->regs.SP -= 2;
    uint16_t * loc = (uint16_t *)(&_memory[_cpu->regs.SP]);
    *loc = _cpu->regs.PC;
    //jump to handler address
    _cpu->regs.PC = _address;
    //not sure if this consumes cycles
    _cpu->cycles += 12;
}
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
                //transfer controll to address 0x0040
                jumpToInterruptHandler(_cpu, _memory, 0x0040);
                //_cpu->regs.IME = 1;//reenable interrupts //Interrupts probably get renabled by the interrupt handler which probably calls RETI
                break;
            }
            
            case INTREQ_LCDSTAT: //LCD STAT Interrupt Request (INT 48h)
            {
                //transfer controll to address 0x0048
                jumpToInterruptHandler(_cpu, _memory, 0x0048);
                break;
            }
            
            case INTREQ_TIMER: //Timer    Interrupt Request (INT 50h)
            {
                //transfer controll to address 0x0050
                jumpToInterruptHandler(_cpu, _memory, 0x0050);
                break;
            }
            case INTREQ_SERIAL: //Serial   Interrupt Request (INT 58h)
            {
                //transfer controll to address 0x0058
                jumpToInterruptHandler(_cpu, _memory, 0x0058);
                break;
            }
            
            case INTREQ_JOYPAD: //Joypad   Interrupt Request (INT 60h)
            {
                //transfer controll to address 0x0060
                jumpToInterruptHandler(_cpu, _memory, 0x0060);
                break;
            }
            
            default:
            {
                printf("ERROR: interrupt %d undefined\n", i);
                break;
            }
        }
        
        if(_cpu->regs.IME == 0)
            break;
    }
    
}

