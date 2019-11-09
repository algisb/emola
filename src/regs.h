#ifndef REGISTER_H_
#define REGISTER_H_
struct Regs
{
//     union
//     {
//         struct
//         {
            union
            {   
                unsigned short AF;
                struct
                {
                    unsigned char F; //Flag register
                    unsigned char A; //Hi
                };
            };
            
            union
            {
                unsigned short BC;
                struct
                {
                    unsigned char C;
                    unsigned char B;
                };
                
            };
            union
            {
                unsigned short DE;
                struct
                {
                    unsigned char E;
                    unsigned char D;
                };
            };
            union
            {
                unsigned short HL;
                struct
                {
                    unsigned char L;
                    unsigned char H;
                };
            };
            
            unsigned short SP; //Stack pointer
            
            unsigned short PC; //Program counter
            
            unsigned char IR; //Instruction register
            
//         };
//         
//     };
    
};
typedef struct Regs Regs;

int resetRegs(Regs * _regs);
#endif
