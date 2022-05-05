#include "idt.h"
#include "stdint.h"
#include "debug.h"
#include "machine.h"
#include "smp.h"
#include "keyboard.h"
#include "process.h"

extern uint32_t keyBoardHandler_;
uint8_t readFromCommandPort;
uint32_t inputBit;
uint32_t outputBit;
uint32_t valueFromPort;
bool done = false;
char* buf = new char[50];
uint32_t bufSize = 0;

char scanCodeToLetter(uint32_t code) {
    if (code == 0x1e) {
        return 'a';
    } else if (code == 0x30) {
        return 'b';
    } else if (code == 0x2e) {
        return 'c';
    } else if (code == 0x20) {
        return 'd';
    } else if (code == 0x12) {
        return 'e';
    } else if (code == 0x21) {
        return 'f';
    } else if (code == 0x22) {
        return 'g';
    } else if (code == 0x23) {
        return 'h';
    } else if (code == 0x17) {
        return 'i';    
    } else if (code == 0x24) {
        return 'j';
    } else if (code == 0x25) {
        return 'k';
    } else if (code == 0x26) {
        return 'l';
    } else if (code == 0x32) {
        return 'm';
    } else if (code == 0x31) {
        return 'n';
    } else if (code == 0x18) {
        return 'o';
    } else if (code == 0x19) {
        return 'p';
    } else if (code == 0x10) {
        return 'q';    
    } else if (code == 0x13) {
        return 'r';
    } else if (code == 0x1f) {
        return 's';
    } else if (code == 0x14) {
        return 't';
    } else if (code == 0x16) {
        return 'u';
    } else if (code == 0x2f) {
        return 'v';
    } else if (code == 0x11) {
        return 'w';
    } else if (code == 0x2d) {
        return 'x';
    } else if (code == 0x15) {
        return 'y';    
    }else if (code == 0x2c) {
        return 'z';
    } else if (code == 0x1c) {
        return '\n';
    } else if (code == 0x39) {
        return ' ';
    } else if (code == 0x35) {
        return '/';
    } else if (code == 0x34) {
        return '.';
    } else if (code == 0x43) {
        return '|';
    }
     else {
        return 0;
    }   
}

void KeyBoard::init(void) {
    done = false;
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
    //Debug::printf("interrupt scan code: %c", (char) valueFromPort);
    outb(0x21, 0xfd);
    IDT::interrupt(9,(uint32_t)&keyBoardHandler_);
}
//registers as interrupt in interrupt handler
//check status of keyboard if changed, read in info and display
//every time interrupt called keyboard handler called
//use inb to read 0x64 see if its changed 
//call init in kernel or config 
extern "C" void keyBoardHandler() {
    readFromCommandPort = inb(0x64);
    //outb()
    //inputBit = readFromCommandPort & 0x40;
    //outb(0x21, 0x64);
    //outputBit = readFromCommandPort & 0x1;
    valueFromPort = inb(0x60);

   // Shared<PCB> cur = gheith::current()->process;
    char letter = scanCodeToLetter(valueFromPort);
    if (letter != 0) {
        Debug::printf("%c", letter);
        if (letter != '\n') {
            // cur->input[cur->inp_size] = letter;
            // cur->inp_size++;
            buf[bufSize] = letter;
            bufSize++;

        } else {
            //cur->input[cur->inp_size] = '\0';
            //cur->done = true;
            // for (uint32_t i = 0; i < cur->inp_size; i++) {
            //     cur->input[i] = '\0';
            // }
            // cur->inp_size = 0;
            // cur->done = false;
            buf[bufSize] = '\0';
            done = true;
        }
    }
    outb(0x20, 0x20);
    




    // Debug::printf("interrupt scan code: %c", (char) inb(0x60));
    // outb(0x20, 0x20);
    //int full = inb(0x64) & 0x1;
    // if (full) {
    //     int clearInputBuffer = inb(0x64) & 0x10;
    //     if (!clearInputBuffer) {
    //         outb(0x60,0x21);
    //     }
    // }
}

