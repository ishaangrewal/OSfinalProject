#include "idt.h"
#include "stdint.h"
#include "debug.h"
#include "machine.h"
#include "smp.h"
#include "idt.h"

extern uint32_t keyBoardHandler_;
uint32_t readFromCommandPort;
uint32_t inputBit;
uint32_t outputBit;
//uint32_t valueFromPort;

void KeyBoard::init(void) {
    //port, byte 
    //index 8 or 9 
    //map scan code
    //outb writes one byte to port 
    //inb reads one byte from port
    // inb(0x21, 0x64);
    // //valueFromPort = inb(0x21);
    // readFromCommandPort = inb(0x64);
    // inputBit = *(uint8_t *)&readFromCommandPort[1] & 0x0;
    // outb(0x21, 0x64);
    // outputBit = *(uint8_t *)&readFromCommandPort[0] & 0x1;
    // valueFromPort = inb(0x60);

    Debug::printf("interrupt scan code: %c", (char) valueFromPort);
    //outb(0x20, 0x20);
    IDT::interrupt(9,(uint32_t)&keyBoardHandler_);
}
//registers as interrupt in interrupt handler
//check status of keyboard if changed, read in info and display
//every time interrupt called keyboard handler called
//use inb to read 0x64 see if its changed 
//call init in kernel or config 
extern "C" void keyBoardHandler() {
    readFromCommandPort = inb(0x64);
    inputBit = *(uint8_t *)&readFromCommandPort[1] & 0x0;
    outb(0x21, 0x64);
    outputBit = *(uint8_t *)&readFromCommandPort[0] & 0x1;
    valueFromPort = inb(0x60);
    uint32_t letter = scanCodeToLetter(valueFromPort);




    // Debug::printf("interrupt scan code: %c", (char) inb(0x60));
    // outb(0x20, 0x20);
    int full = inb(0x64) & 0x1;
    // if (full) {
    //     int clearInputBuffer = inb(0x64) & 0x10;
    //     if (!clearInputBuffer) {
    //         outb(0x60,0x21);
    //     }
    // }
}

