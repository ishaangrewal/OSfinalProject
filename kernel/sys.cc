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
#include "libk.h"
#include "elf.h"

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
void unmap(uint32_t* pd, uint32_t va) {
    auto pdi = va >> 22;
    auto pti = (va >> 12) & 0x3FF;
    auto pde = pd[pdi];
    if ((pde & 1) == 0) return;
    auto pt = (uint32_t*) (pd[pdi] & 0xFFFFF000);
    auto pte = pt[pti];
    if ((pte & 1) == 0) return;
    auto pa = pte & 0xFFFFF000;
    pt[pti] = 0;
    PhysMem::dealloc_frame(pa);
    invlpg(va);
}
template <typename T>
gheith::TCB* makeThread(T work) {
    using namespace gheith;
    delete_zombies();
    auto tcb = new TCBImpl<T>(work);
    return tcb;

}
uint32_t close(uint32_t fd) {
    if (fd < 0 || fd > 30) {
        return -1;
    }
    Shared<PCB> cur_process = gheith::current()->process;
    if (fd < 10) {
        cur_process->fd[fd] = nullptr;
    }
    else if (fd < 20) {
        cur_process->sp[fd - 10] = nullptr;
    }
    else {
        cur_process->cp[fd - 20] = nullptr;
    }
    return 0;
}
int fork(uint32_t user_eip, uint32_t user_esp) {
    //Debug::printf("*** test1\n");
    Shared<PCB> cur_process = gheith::current()->process;
    gheith::TCB* newThread = makeThread([user_eip, user_esp] {
        //Debug::printf("*** eip: %x esp: %x", user_eip, user_esp);
        switchToUser(user_eip, user_esp, 0);
    });
    //newThread->process = Shared<PCB>::make(new PCB());
    uint32_t index = 20;
    while (index < 30 && cur_process->cp[index - 20] != nullptr) {
        index++;
    }
    //Debug::printf("*** test2\n");
    if (index == 30) {
        return -1;
    }
    //Debug::printf("*** test3\n");
    cur_process->cp[index - 20] = newThread->process;
    // newThread->process->fd = Shared<FileDescriptor>::make();
    // newThread->process->sp = Shared<Semaphore>::make();
    for (int i = 0; i < 10; i++) {
        newThread->process->fd[i] = cur_process->fd[i];
        newThread->process->sp[i] = cur_process->sp[i];
    }
    //newThread->process->future = Shared<Future<uint32_t>>::make();
    //newThread->process->pid = 0;
    uint32_t* pd = gheith::current()->pd;
    uint32_t va = 0;
    //Debug::printf("*** test4\n");
    
    for (int i = 512; i < 1024; i++) {
        if (pd[i] % 2 == 1) {
            uint32_t* pt = (uint32_t*)(((uint32_t)pd[i]) & 0xfffff000);
            for (int j = 0; j < 1024; j++) {
                va = (i << 22) + (j * 4096);
                if (pt[j] % 2 == 1 && va != kConfig.ioAPIC && va != kConfig.localAPIC) {
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
    return index;
}
int execl(char* path, char** argv, uint32_t length) {
    //Debug::printf("*** ARGC ISS  :::: %d", length);
    //for (int i = 0 ; i < 4; i++) {
    //    Debug::printf("string is this: %s \n", argv[i]);
    //}
    Shared<PCB> cur_process = gheith::current()->process;
    for (int i = 0; i < 10; i++) {
        cur_process->sp[i] = nullptr;
        cur_process->cp[i] = nullptr;
    }
    
    uint32_t* pd = gheith::current()->pd;
    uint32_t va = 0;
    //Debug::printf("*** test4\n");
    
    for (int i = 512; i < 1024; i++) {
        if (pd[i] % 2 == 1) {
            uint32_t* pt = (uint32_t*)(((uint32_t)pd[i]) & 0xfffff000);
            for (int j = 0; j < 1024; j++) {
                va = (i << 22) + (j * 4096);
                if (pt[j] % 2 == 1 && va != kConfig.ioAPIC && va != kConfig.localAPIC) {
                    unmap(pd,va);
                }
            }
        }
    }
    //for (int i = 0 ; i < 4; i++) {
    //    Debug::printf("string is this: %s \n", argv[i]);
    //}
    uint32_t* userEsp = (uint32_t*) 0xefffe000;
    //Debug::printf("*** esp: %x\n", userEsp);
    uint32_t* stack_list = new uint32_t[length];
    uint32_t cur_len;
    for (uint32_t i = 0; i < length; i++) {
        cur_len = K::strlen(argv[i]) + 1;
        while (cur_len % 4 != 0) {
            cur_len++;
        }
        //if (cur_len % 4 != 0) {
        //    cur_len += (4 - (cur_len % 4));
        //}
        memcpy(userEsp, argv[i], cur_len);
        stack_list[i] = (uint32_t)userEsp;
        //Debug::printf("*** cur_len: %x\n", cur_len);
        //Debug::printf("*** string: %s\n", argv[i]);
        userEsp -= (cur_len / 4);
        //Debug::printf("*** esp: %x\n", userEsp);
    }
    *userEsp = 0;
    userEsp--;
    //Debug::printf("*** esp: %x\n", userEsp);
    for (int i = length - 1; i >= 0; i--) {
        *userEsp = (uint32_t)argv[i];
        userEsp--;
        //Debug::printf("*** esp: %x\n", userEsp);
    }
    //Debug::printf("*** esp: %x\n", userEsp);
    *userEsp = (uint32_t)(userEsp + 1);
    userEsp--;
    //Debug::printf("*** esp: %x\n", userEsp);
    *userEsp = length;
    Shared<Node> n = fs->find(fs->root, path);
    if (n == nullptr) {
        Debug::panic("*** node is nullptr\n");
    }
    switchToUser(ELF::load(n),(uint32_t)userEsp,0);
    Debug::panic("*** passed switch\n");
    return -1;
}
int wait(int id, uint32_t *status) {
    //Debug::printf("*** Test1!\n");
    Shared<PCB> cur_process = gheith::current()->process;
    //Debug::printf("*** Test2!\n");
    if (cur_process->cp[id - 20] == nullptr) {
        Debug::printf("no child!\n");
        return -1;
    }
    //Debug::printf("*** Test3!\n");
    if (cur_process->cp[id - 20]->future == nullptr) {
        Debug::printf("no future in child!\n");
        return -1;
    }
    //Debug::printf("*** Test4!\n");
    *status = cur_process->cp[id - 20]->future->get();
    //Debug::printf("*** Test5!\n");
    return 0;
}
int up(int id) {
    if (id < 10 || id > 19) {
        return -1;
    }
    Shared<PCB> cur_process = gheith::current()->process;
    cur_process->sp[id - 10]->up();
    return 0;
}
int down(int id) {
    if (id < 10 || id > 19) {
        return -1;
    }
    Shared<PCB> cur_process = gheith::current()->process;
    cur_process->sp[id - 10]->down();
    return 0;
}
int sem(uint32_t initial) {
    Shared<PCB> cur_process = gheith::current()->process;
    uint32_t i = 10;
    Shared<Semaphore> marker = cur_process->sp[i - 10];
    while (marker != nullptr && i < 19) {
        i++;
        marker = cur_process->sp[i - 10];
    }
    if (marker != nullptr) {
        return -1;
    }
    cur_process->sp[i - 10] = Shared<Semaphore>::make(initial);
    return i;
}
ssize_t read(int fd, void* buf, size_t nbyte) {
    if ((uint32_t)buf < 0x80000000 || ((uint32_t)(buf) >= kConfig.localAPIC && (uint32_t)(buf) < kConfig.localAPIC + 4096) || ((uint32_t)(buf) >= kConfig.ioAPIC && (uint32_t)(buf) < kConfig.ioAPIC + 4096)) {
        return -1;
    }
    if (fd < 3 || fd > 9) {
        if (fd < 3 && fd >= 0 && !gheith::current()->process->fd[fd]->valid) {
            //return -1;
        }
        else {
            return -1;
        }
    }
    Shared<PCB> cur_process = gheith::current()->process;
    Shared<FileDescriptor> marker = cur_process->fd[fd];
    if (marker == nullptr) {
        return -1;
    }
    //if (marker->offset > marker->node->size_in_bytes()) {
    //    return -1;
    //}
    if ((marker->offset + nbyte) > marker->node->size_in_bytes()) {
        nbyte = marker->node->size_in_bytes() - marker->offset;
    }
    uint32_t ret = marker->node->read_all(marker->offset ,nbyte, (char*)buf);
    marker->offset += ret;
    return ret;
}
ssize_t write(int fd, void* buf, size_t nbyte) {
    char* buffer = (char*) buf;
    if ((uint32_t)buf < 0x80000000 || (fd != 0 && fd != 1 && fd != 2) || ((uint32_t)(buf) >= kConfig.localAPIC && (uint32_t)(buf) < kConfig.localAPIC + 4096) || ((uint32_t)(buf) >= kConfig.ioAPIC && (uint32_t)(buf) < kConfig.ioAPIC + 4096)) {
        return -1;
    }
    if (gheith::current()->process->fd[fd] == nullptr || !(gheith::current()->process->fd[fd]->valid)) {
        return -1;
    }
    for (uint32_t i = 0; i < nbyte; i++) {
        if ((uint32_t)(buffer +i) < 0x80000000 || ((uint32_t)(buffer + i) >= kConfig.localAPIC && (uint32_t)(buffer + i) < kConfig.localAPIC + 4096) || ((uint32_t)(buffer + i) >= kConfig.ioAPIC && (uint32_t)(buffer + i) < kConfig.ioAPIC + 4096)) {
            return i;
        }
        Debug::printf("%c", ((char*)(buf))[i]);
    } 
    return nbyte;
}
ssize_t len(int fd) {
    Shared<PCB> cur_process = gheith::current()->process;
    Shared<FileDescriptor> marker = cur_process->fd[fd];
    if (marker == nullptr){
        return -1;
    }
    if ((fd == 0 && marker->valid) || (fd == 1 && marker->valid )|| (fd == 2 && marker->valid)) {
        return 0;
    }
    return marker->node->size_in_bytes();  
}
int open(const char* fn, int flags) {
    Shared<PCB> cur_process = gheith::current()->process;
    uint32_t i = 0;
    Shared<FileDescriptor> marker = cur_process->fd[i];
    while (marker != nullptr && i < 9) {
        i++;
        marker = cur_process->fd[i];
    }
    if ((marker != nullptr)) {
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
    Shared<PCB> cur_process = gheith::current()->process;
    cur_process->future->set(rc);
    stop();

}
int shutdown() {
    Debug::shutdown();
    return 0;
}
int seek(uint32_t fd, uint32_t offset) {
    if (fd < 0 || fd > 9) {
        return -1;
    }
    if (fd < 3 && (gheith::current()->process->fd[fd] == nullptr || gheith::current()->process->fd[fd]->node == nullptr)) {
        return -1;
    }
    gheith::current()->process->fd[fd]->offset = offset;
    return offset;
}
// read directory
int ls() {
    //find size of current directory
    uint32_t len = fs->current->get_length_of_dir_entries();
    //allocate memory for directory entries
    char* dir_entries = new char[len + 1];
    //read directory entries
    fs->current->get_dir_entries(dir_entries);
    //print directory entries
    Debug::printf("%s\n", dir_entries);
    return 0;

}
int getDirEntriesLength(int fd) {
    if (fd < 0 || fd > 9) {
        return -1;
    }
    if ((gheith::current()->process->fd[fd] == nullptr || gheith::current()->process->fd[fd]->node == nullptr)) {
        return -1;
    }
    if (gheith::current()->process->fd[fd]->node->is_dir() == false) {
        return -1;
    }
    Shared<Node> node = gheith::current()->process->fd[fd]->node;
    return node->get_length_of_dir_entries();
}

int changeDir(const char* path) {
    //get the current path
    char *cwd = fs->cwd;

    if (path[0] == '/') {
        fs->current = fs->root;
        fs->cwd = "/";
        return 0;
    }

    if (path[0] == '.' && path[1] == '.') {
        fs->current = fs->find(fs->current, "..");
        // get index of the last occurrence of backslash using a while loop
        int i = 0;
        uint32_t lastBackSlash = 0;
        while (fs->cwd[i] != '\0') {
            if (fs->cwd[i] == '/') {
                lastBackSlash = i;
            }
            i++;
        }
        if (lastBackSlash == 0) {
            fs->cwd = "/";
        } else {
            for (size_t i = lastBackSlash; i < 100; i++)
            {
                fs->cwd[i] = '\0';
            }
            
        }
        return 0;

        
    } else if (path[0] == '.') {
        return 0;
    }

    
    // append the new path to the current path
    // get to the end of the current path
    int i = 0;
    while (cwd[i] != '\0') {
        i++;
    }
    // append the new path
    if (path[i-1] != '/') {
        cwd[i] = '/';
    }
    int j = 0;
    while (path[j] != '\0') {
        cwd[i] = path[j];
        i++;
        j++;
    }
    cwd[i] = '\0';
    fs->current = fs->find(fs->current, cwd);

    return 0;
}
int pwd() {
    char *cwd = fs->cwd;
    Debug::printf("%s\n", cwd);
    return 0;
}
extern "C" int sysHandler(uint32_t eax, uint32_t *frame) {
    uint32_t* esp;
    esp = (uint32_t*)frame[3];
    uint32_t user_eip = frame[0];
    char** argv;
    char* path;
    char* cpyPath;
    int count;
    uint32_t size_of_path;
    switch (eax) {
        case 0:
            //exit
            exit(esp[1]);
            //return -1;
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
            return fork(user_eip, (uint32_t)esp);
        case 3:
            //sem
            return sem(esp[1]);
        case 4:
            return up(esp[1]);
        case 5:
            return down(esp[1]);
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
            count = 0;
            //path = new char[1];
            cpyPath = (char*)esp[1];
            size_of_path = 0;
            while (cpyPath[size_of_path] != 0) {
                size_of_path++;
            }
            path = new char[size_of_path + 1];
            memcpy(path, (char*)esp[1], size_of_path + 1);
            path[size_of_path] = 0;

            while (esp[count + 2] != 0) {
                count++;
            }
            argv = new char* [count];
            for (int i = 0; i < count; i++) {
                char* cur = (char*)esp[i + 2];
                uint32_t length = K::strlen(cur) +1;
                while (length % 4 != 0) {
                    length++;
                }
                //if (length % 4 != 0) {
                //    length += (4 - (K::strlen(cur) % 4));
                //}
                argv[i] = new char[length];
                memcpy((void*)argv[i], cur, length);
                
            }
            //for (int i = 0 ; i < 4; i++) {
            //    Debug::printf("string is: %s \n", argv[i]);
            //}
            return execl(path, argv, count);
            //return execl((const char*)esp[1], (const char**)esp[2]);
        case 10:
            return open((char*)esp[1], esp[2]);
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
        case 14:
            //readdir
            return ls();
        case 15:
            //getDirEntriesLength
            return getDirEntriesLength(esp[1]);
        case 16:
            //ls
            return changeDir((char*)esp[1]);
        case 17:
            //pwd
            return pwd();
        default:
            Debug::panic("*** NOT IMPLEMENTED\n");
    }
    return 0;
}

void SYS::init(void) {
    IDT::trap(48,(uint32_t)sysHandler_,3);
}