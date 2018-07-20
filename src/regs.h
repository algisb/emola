#ifndef REGISTER_H_
#define REGISTER_H_
struct Regs
{
    union
    {
        unsigned char reg[10];
        struct
        {
            unsigned char A;
            unsigned char B;
            unsigned char C;
            unsigned char D;
            unsigned char E;
            unsigned char F;
            unsigned char G;
            unsigned char H;
        };
        
    };
    
};
typedef struct Regs Regs;
#endif
