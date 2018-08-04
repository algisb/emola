#ifndef MMU_H_
#define MMU_H_

struct MMU;
typedef struct MMU MMU;

int createMMU(MMU ** _mmu);
int destroyMMU(MMU ** _mmu);


#endif
