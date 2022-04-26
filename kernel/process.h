#include "semaphore.h"
#include "ext2.h"
#include "future.h"


struct FileDescriptor {
    Shared<Node> node;
    uint32_t offset;
    Atomic<uint32_t> ref_count = {0};
    bool valid;
    FileDescriptor(Shared<Node> n) {
        node = n;
        valid = false;
    } 
};

struct PCB
{
    uint32_t pid;
    Shared<Future<uint32_t>> future;
    Shared<FileDescriptor> fd[10];
    Shared<PCB> cp[10];
    Shared<Semaphore> sp[10];
    Atomic<uint32_t> ref_count = 0;
    PCB() {
        for(int i = 0; i < 10; i++) {
            cp[i] = Shared<PCB>();
            if (i == 0 || i == 1 || i == 2) {
                fd[i] = Shared<FileDescriptor>::make(Shared<Node>());
                fd[i]->valid = true;
            }
            else {
                fd[i] = Shared<FileDescriptor>();
            }
            sp[i] = Shared<Semaphore>();
        }
        future = Shared<Future<uint32_t>>::make();
    }
};
