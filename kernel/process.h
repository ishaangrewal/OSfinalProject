#include "semaphore.h"
#include "ext2.h"
#include "future.h"


struct FileDescriptor {
    Shared<Node> node;
    uint32_t offset;
    Atomic<uint32_t> ref_count = {0};
    FileDescriptor(Shared<Node> n) {
        node = n;
    }
};

struct PCB
{
    uint32_t pid;
    Future<uint32_t>* future;
    Shared<FileDescriptor> fd[10];
    Shared<PCB> cp[10];
    Shared<Semaphore> sp[10];
    Atomic<uint32_t> ref_count = 0;
};
