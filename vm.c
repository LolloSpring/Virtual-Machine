#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#define MAX_REG 10
#define MAX_FLAG 1
#define STACK_SIZE 512
#define PASSWORD_SIZE 30

void author(){printf("----\nGL & HF :) I hope you'll enjoy my work\nCredz : Rawger\n----");}

void motd() {
  printf("+----------------------------------------+\n"
         "| Virtual Machine                        |\n"
         "|  - Dynamic Instruction Size            |\n"
         "|  - 32 bits registers                   |\n"
         "|  - LIFO Stack                          |\n"
         "+----------------------------------------+\n\n");
}

int thegame = 1;  
char password[PASSWORD_SIZE+1] = {0};

int _regs[MAX_REG] = {0};
int _eip = 0;
int _eflags[MAX_FLAG] = {0};
const int _zf = 0;
const int _cf = 1;

int stack[STACK_SIZE] = {0};
int _ebp = 0x0;
int _esp = 0x0;

unsigned char vm[] = {
 0x13,0x37,                               //signature
 0x19,                                    //entry point
 0xe8,0x8a,0x8e,0xde,0xe5,0x88,0xe5,0xec, //flag
 0xd3,0xc8,0xce,0xcf,0x8e,0xd6,0xe5,0xd7, //flag
 0x8e,0xd9,0xd2,0x8b,0xd4,0x89,           //flag
 0xd,0x0,                                 //pop r0 (r0 = len user input)
 0x1,0x1,0x1,                             //load r1,1
 0x4,0x5,0x5,                             //xor r5,r5
 0x3,0x0,0x1,                             //sub r0,r1
 0x15,0x3,0x0,                            //GetCode2Value offset 3 index r0
 0xd,0x2,                                 //pop flag char
 0xd,0x4,                                 //pop user input char
 0x1,0x6,0xba,                            //load r6, 0xba
 0x4,0x4,0x6,                             //xor r4,r6
 0x6,0x4,0x2,                             //cmp r4,r2
 0x8,0xa,                                 //jmpne exit
 0x2,0x9,0x1,                             //add r9,r1
 0x6,0x0,0x5,                             //cmp r0,r5
 0x14,0x21,                               //jmpane 0x21
 0x1,0x8,0x16,                            //load r8,0x16
 0x6,0x8,0x9,                             //cmp r8,r9
 0x8,0x4,                                 //jmpne exit
 0x12,0xff,0x0,                           //win
 0x0                                      //loose exit
};

void clean(const char *c) {
  char *cr = strchr(c, '\n');
  if(cr != NULL)
    *cr = 0;
  char *lf = strchr(c, '\r');
  if(lf != NULL)
    *lf = 0;
}

void load() {
  for(int i = 0; i<strlen(password);i++) {
    _esp+=1;
    *(stack+_esp) = *(password+i);
  }

  _esp+=1;
  *(stack+_esp) = strlen(password);
}

void win() {
  printf("\nCongratz, here is the flag BREIZHCTF{the password you have used}\n");
  exit(0);
}

void VM() {
  if(*vm == 0x13 && *(vm+1) == 0x37)
    _eip = *(vm+(_eip+2));
  else
    exit(42);
  while(thegame) {
    switch(*(vm+_eip)) {
      case 0:
        //printf("Stop VM\n");
        thegame = 0;
        break;
      case 1:
        //printf("Load\n");
        *(_regs+*(vm+(_eip+1))) = *(vm+(_eip+2));
        _eip+=3;
        break;
      case 2:
        //printf("Add\n");
        *(_regs+*(vm+(_eip+1))) += *(_regs+*(vm+(_eip+2)));
        _eip+=3;
        break;
      case 3:
        //printf("Sub\n");
        *(_regs+*(vm+(_eip+1))) -= *(_regs+*(vm+(_eip+2)));
        _eip+=3;
        break;
      case 4:
        //printf("Xor\n");
        *(_regs+*(vm+(_eip+1))) ^= *(_regs+*(vm+(_eip+2)));
        _eip+=3;
        break;
      case 5:
        //printf("Jump\n");
        _eip+=*(vm+(_eip+1));
        break;
      case 6:
        //printf("Cmp\n");
        if(*(_regs+*(vm+(_eip+1))) > *(_regs+*(vm+(_eip+2)))) {
          *(_eflags+_zf) = 0x0;
          *(_eflags+_cf) = 0x0;
        }
        else if(*(_regs+*(vm+(_eip+1))) < *(_regs+*(vm+(_eip+2)))) {
          *(_eflags+_zf) = 0x0;
          *(_eflags+_cf) = 0x1;
        }
        else if(*(_regs+*(vm+(_eip+1))) == *(_regs+*(vm+(_eip+2)))) {
          *(_eflags+_zf) = 0x1;
          *(_eflags+_cf) = 0x0;          
        }
        _eip+=3;
        break;
      case 7:
        //printf("Jmpe\n");
        if(*(_eflags+_zf) == 0x1 && *(_eflags+_cf) == 0x0)
          _eip += *(vm+(_eip+1));
        else
          _eip=_eip+2;
        break;
      case 8:
        //printf("Jmpne\n");
        if(*(_eflags+_zf) != 0x1 || *(_eflags+_cf) != 0x0)
          _eip += *(vm+(_eip+1));
        else
          _eip=_eip+2;
        break;
      case 9:
        //printf("Jmpgt\n");
        if(*(_eflags+_zf) == 0x0 && *(_eflags+_cf) == 0x0)
          _eip += *(vm+(_eip+1));
        else
          _eip=_eip+2;
        break;
      case 10:
        //printf("Jmplt\n");
        if(*(_eflags+_zf) == 0x0 && *(_eflags+_cf) == 0x1)
          _eip += *(vm+(_eip+1));
        else
          _eip=_eip+2;
        break;
      case 11:
        //printf("Nop\n");
        _eip++;
        break;
      case 12:
        //printf("Vpush\n");
        _esp++;
        *(stack+_esp) = *(_regs+*(vm+(_eip+1)));
        *(_regs+*(vm+(_eip+1))) = 0x0;
        _eip+=2;
        break;
      case 13:
        //printf("Vpop\n");
        *(_regs+*(vm+(_eip+1))) = *(stack+_esp);
        _esp--; _eip+=2;
        break;
      case 14:
        //printf("JmpA\n");
        _eip = *(vm+(_eip+1));
        break;
      case 15:
        //printf("And\n");
        *(_regs+*(vm+(_eip+1))) = *(_regs+*(vm+(_eip+1))) & *(_regs+*(vm+(_eip+2)));
        _eip+=3;
        break;
      case 16:
        //printf("Or\n");
        *(_regs+*(vm+(_eip+1))) = *(_regs+*(vm+(_eip+1))) | *(_regs+*(vm+(_eip+2)));
        _eip+=3;
        break;
      case 17:
        //printf("Mov\n");
        *(_regs+*(vm+(_eip+1))) = *(_regs+*(vm+(_eip+2)));
        *(_regs+*(vm+(_eip+2))) = 0x0;
        _eip+=3;
        break;
      case 18:
        //printf("Verify\n");
        if(*(vm+(_eip+1)) == 0xff) {
          win();
        }
        _eip=_eip+2;
        break;
      case 19:
        //printf("JmpAe\n");
        if(*(_eflags+_zf) == 0x1 && *(_eflags+_cf) == 0x0)
          _eip = *(vm+(_eip+1));
        else
          _eip=_eip+2;
        break;
      case 20:
        //printf("JmpAne\n");
        if(*(_eflags+_zf) != 0x1 || *(_eflags+_cf) != 0x0)
          _eip = *(vm+(_eip+1));
        else
          _eip=_eip+2;
        break;
      case 21:
        //printf("GetCode2Value\n");
        _esp++;
        *(stack+_esp) = *(vm+*(vm+(_eip+1)) + *(_regs+*(vm+(_eip+2))));
        _eip=_eip+3;
        break;
      default:
        __asm__("int 3");
        break;
    }
  }
}

void setup() {
  printf("\nStarting VM...\n> Password ? ");
  fgets(password,PASSWORD_SIZE,stdin);
  clean(password);
  load(password);
  VM();
}

int main(int argc, char *argv[]) {
  motd();
  signal(SIGTRAP, setup);
  __asm__("int 3");
  return 0;
}

