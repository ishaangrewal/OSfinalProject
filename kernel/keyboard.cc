#include "idt.h"
#include "stdint.h"
#include "debug.h"
#include "machine.h"
#include "smp.h"
#include "idt.h"

extern uint32_t keyBoardHandler_;

void KeyBoard::init(void) {
    outb(0x21, 0xfd);
    IDT::interrupt(71, keyBoardHandler_);
}
extern "C" void keyBoardHandler(uintptr_t va_, uintptr_t *saveState) {
}

