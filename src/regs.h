#ifndef REGISTER_H_
#define REGISTER_H_
struct Regs
{
    union
    {
        struct
        {
            union
            {   
                unsigned short AF;
                struct
                {
                    unsigned char A; //Hi
                    unsigned char F; //Flag register
                };
            };
            
            union
            {
                unsigned short BC;
                struct
                {
                    unsigned char B;
                    unsigned char C;
                };
                
            };
            union
            {
                unsigned short DE;
                struct
                {
                    unsigned char D;
                    unsigned char E;
                };
            };
            union
            {
                unsigned short HL;
                struct
                {
                    unsigned char H;
                    unsigned char L;
                };
            };
            
            unsigned short SP; //Stack pointer
            
            unsigned short PC; //Program counter
        };
        
    };
    
};
typedef struct Regs Regs;
#endif
