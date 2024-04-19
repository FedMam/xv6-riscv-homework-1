struct mutex {
    struct sleeplock lock;
    int used_by; // count of processes that use the mutex
};