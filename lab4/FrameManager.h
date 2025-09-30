#ifndef FRAMEMANAGER_H
#define FRAMEMANAGER_H

#include <vector>
#include <cstdint>
#include <string>
#include <list>
#include <unordered_map>
#include <numeric>
#include "PageTable.h"

enum class ReplacementPolicy {
    OPTIMAL,
    FIFO,
    LRU,
    RANDOM
};

enum class AllocationPolicy {
    GLOBAL,
    LOCAL
};

struct MemoryAccess {
    int process_id;
    uint64_t virtual_address;
};

struct Frame {
    bool is_free;
    int process_id;
    uint64_t virtual_page_number;
};

class FrameManager {
public:
    FrameManager(uint64_t num_frames, ReplacementPolicy repl_policy, AllocationPolicy alloc_policy, const std::vector<MemoryAccess>& accesses);
    uint64_t getFrame(PageTable* page_table, uint64_t virtual_page_number, uint64_t current_access_index, std::vector<PageTable>& all_page_tables, uint64_t page_offset_bits);
    void recordAccess(uint64_t frame_number);

private:
    uint64_t findFreeFrame();
    uint64_t evictFrame(PageTable* current_page_table, uint64_t current_access_index, std::vector<PageTable>& all_page_tables, uint64_t page_offset_bits);
    
    uint64_t evictOptimal(PageTable* current_page_table, uint64_t current_access_index, std::vector<PageTable>& all_page_tables, uint64_t page_offset_bits);
    uint64_t evictFIFO(PageTable* current_page_table);
    uint64_t evictLRU(PageTable* current_page_table);
    uint64_t evictRandom(PageTable* current_page_table);

    uint64_t num_frames;
    std::vector<Frame> frame_table;
    std::list<uint64_t> free_frames;
    
    ReplacementPolicy replacement_policy;
    AllocationPolicy allocation_policy;

    // For replacement policies
    std::list<uint64_t> fifo_queue;
    std::list<uint64_t> lru_list;
    std::unordered_map<uint64_t, std::list<uint64_t>::iterator> lru_map;
    const std::vector<MemoryAccess>& memory_accesses;
    
    // For local allocation
    std::vector<uint64_t> process_frame_counts;
    std::vector<std::list<uint64_t>> process_frames;
};

#endif // FRAMEMANAGER_H
