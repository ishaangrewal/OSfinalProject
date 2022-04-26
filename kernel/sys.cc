#include "sys.h"
#include "stdint.h"
#include "idt.h"
#include "debug.h"
#include "machine.h"
#include "config.h"
#include "threads.h"
#include "process.h"
#include "ext2.h"
#include "physmem.h"

auto d = Shared<Ide>::make(1);
auto fs = Shared<Ext2>::make(d);

void map(uint32_t* pd, uint32_t va, uint32_t pa) {
    auto pdi = va >> 22;
    auto pti = (va >> 12) & 0x3FF;
    auto pde = pd[pdi];
    if ((pde & 1) == 0) {
        pde = PhysMem::alloc_frame() | 7;
        pd[pdi] = pde;
    }
    auto pt = (uint32_t*) (pde & 0xFFFFF000);
    pt[pti] = pa | 7;
}
template <typename T>
gheith::TCB* makeThread(T work) {
    using namespace gheith;
    delete_zombies();
    auto tcb = new TCBImpl<T>(work);
    return tcb;

}
uint32_t close(uint32_t fd) {
    Shared<PCB> cur_process = gheith::current()->process;
    cur_process->fd[fd] = nullptr;
    return 0;
}
int fork(uint32_t user_eip, uint32_t user_esp) {
    //Debug::printf("*** test1\n");
    Shared<PCB> cur_process = gheith::current()->process;
    gheith::TCB* newThread = makeThread([user_eip, user_esp] {
        switchToUser(user_eip, user_esp, 0);
    });
    //newThread->process = Shared<PCB>::make();
    uint32_t index = 0;
    while (index < 10 && cur_process->cp[index] != nullptr) {
        index++;
    }
    //Debug::printf("*** test2\n");
    if (index == 10) {
        return -1;
    }
    //Debug::printf("*** test3\n");
    cur_process->cp[index] = newThread->process;
    // newThread->process->fd = Shared<FileDescriptor>::make();
    // newThread->process->sp = Shared<Semaphore>::make();
    for (int i = 0; i < 10; i++) {
        newThread->process->fd[i] = cur_process->fd[i];
        newThread->process->sp[i] = cur_process->sp[i];
    }
    uint32_t* pd = gheith::current()->pd;
    uint32_t va = 0;
    //Debug::printf("*** test4\n");
    for (int i = 512; i < 1024; i++) {
        if (pd[i] % 2 == 1) {
            uint32_t* pt = (uint32_t*)(((uint32_t)pd[i]) & 0xfffff000);
            for (int j = 0; j < 1024; j++) {
                va = (i << 22) + (j * 4096);
                if (pt[j] % 2 == 1) {
                    uint32_t pa = PhysMem::alloc_frame();
                    map(newThread->pd,va, pa);
                    memcpy((void*)pa,(void*)va,4096);
                }
            }
        }
    }
    //Debug::printf("*** test5\n");
    schedule(newThread);
    //Debug::printf("*** test6\n");
    return index + 100;
}
int execl(const char* path, const char* arg0) {
    return -1;
}
int wait(int id, uint32_t *status) {
    return -1;
}
int up(int id) {
    if (id < 0 || id > 9) {
        return -1;
    }
    Shared<PCB> cur_process = gheith::current()->process;
    cur_process->sp[id]->up();
    return 0;
}
int down(int id) {
    if (id < 0 || id > 9) {
        return -1;
    }
    Shared<PCB> cur_process = gheith::current()->process;
    cur_process->sp[id]->down();
    return 0;
}
int sem(uint32_t initial) {
    Shared<PCB> cur_process = gheith::current()->process;
    uint32_t i = 0;
    Shared<FileDescriptor> marker = cur_process->fd[i];
    while (marker != nullptr && i < 9) {
        i++;
        marker = cur_process->fd[i];
    }
    if (marker != nullptr) {
        return -1;
    }
    cur_process->sp[i] = Shared<Semaphore>();
    return i;
}
ssize_t read(int fd, void* buf, size_t nbyte) {
    Shared<PCB> cur_process = gheith::current()->process;
    Shared<FileDescriptor> marker = cur_process->fd[fd];
    if (marker == nullptr) {
        return -1;
    }
    uint32_t ret = marker->node->read_all(marker->offset ,nbyte, (char*)buf);
    marker->offset = ret;
    return ret;
}
ssize_t write(int fd, void* buf, size_t nbyte) {
    if ((uint32_t)buf < 0x80000000 || (fd != 1 && fd != 2) || ((uint32_t)(buf) >= kConfig.localAPIC && (uint32_t)(buf) < kConfig.localAPIC + 4096) || ((uint32_t)(buf) >= kConfig.ioAPIC && (uint32_t)(buf) < kConfig.ioAPIC + 4096)) {
        return -1;
    }
    for (uint32_t i = 0; i < nbyte; i++) {
        Debug::printf("%c", ((char*)(buf))[i]);
    } 
    return nbyte;
}
ssize_t len(int fd) {
    Shared<PCB> cur_process = gheith::current()->process;
    Shared<FileDescriptor> marker = cur_process->fd[fd];
    if (marker == nullptr) {
        return -1;
    }
    return marker->node->size_in_bytes();  
}
int open(const char* fn, int flags) {
    Shared<PCB> cur_process = gheith::current()->process;
    uint32_t i = 3;
    Shared<FileDescriptor> marker = cur_process->fd[i];
    while (marker != nullptr && i < 9) {
        i++;
        marker = cur_process->fd[i];
    }
    if (marker != nullptr) {
        return -1;
    }
    Shared<Node> node = fs->find(fs->root, fn);
    if (node == nullptr) {
        return -1;
    }
    cur_process->fd[i] = Shared<FileDescriptor>(new FileDescriptor(node));
    cur_process->fd[i]->offset = 0;
    return i;
}
void exit(int rc) {

}
int shutdown() {
    Debug::shutdown();
    return 0;
}
int seek(uint32_t fd, uint32_t offset) {
    gheith::current()->process->fd[fd]->offset = offset;
    return offset;
}
extern "C" int sysHandler(uint32_t eax, uint32_t *frame) {
    uint32_t* esp;
    esp = (uint32_t*)frame[3];
    switch (eax) {
        case 0:
            return shutdown();
            //change
        case 1:
            /*
            if (esp[2] < 0x80000000 || (esp[1] != 1 && esp[1] != 2) || ((uint32_t)(esp[2]) >= kConfig.localAPIC && (uint32_t)(esp[2]) < kConfig.localAPIC + 4096) || ((uint32_t)(esp[2]) >= kConfig.ioAPIC && (uint32_t)(esp[2]) < kConfig.ioAPIC + 4096)) {
                return -1;
            }
            for (uint32_t i = 0; i < esp[3]; i++) {
                Debug::printf("%c", ((char*)(esp[2]))[i]);
            } 
            return esp[3];
            */
            return write(esp[1], (char*)esp[2], esp[3]);
        case 2:
            return fork((uint32_t)frame[0], (uint32_t)frame[3]);
        case 3:
            //sem
            return sem(esp[1]);
        case 4:
            //return up(esp[1]);
        case 5:
            //return down(esp[1]);
        case 6:
            //close
            return close(esp[1]);
        case 7:
            //shutdown
            return shutdown();
        case 8:
            //wait
            return wait(esp[1], (uint32_t*)esp[2]);
        case 9:
            //execl
            //return execl();
        case 10:
            /*
            Shared<PCB> cur_process = gheith::current()->process;
            uint32_t i = 0;
            Shared<FileDescriptor> marker = cur_process->fd[i];
            while (marker != nullptr && i < 9) {
                i++;
                marker = cur_process->fd[i];
            }
            if (marker != nullptr) {
                return -1;
            }
            cur_process->fd[i]->node = fs->find(fs->root, (char*)esp[1]);
            cur_process->fd[i]->offset = 0;
            return i;
            */
           return open((char*)esp[1], esp[2]);
        case 11:
            /*
            Shared<PCB> cur_process = gheith::current()->process;
            Shared<FileDescriptor> marker = cur_process->fd[esp[1]];
            return marker->node->size_in_bytes();  
            */
            return len(esp[1]);           
        case 12:
            /*
            Shared<PCB> cur_process = gheith::current()->process;
            Shared<FileDescriptor> marker = cur_process->fd[esp[1]]; 
            marker->node->read_all(marker->offset ,esp[3], (char*)esp[2]);
            return esp[3];
            */
            return read(esp[1], (void*)esp[2], esp[3]);
        case 13:
            //seek
            return seek(esp[1], esp[2]);

        default:
            Debug::panic("*** NOT IMPLEMENTED\n");
    }
    return 0;
}

void SYS::init(void) {
    IDT::trap(48,(uint32_t)sysHandler_,3);
}