#ifndef _KeyBoard_H_
#define _KeyBoard_H_

#include "stdint.h"
extern char* buf;
extern uint32_t bufSize;
extern bool done;

class KeyBoard {
    
public:
    static void init(void);
    
};

#endif