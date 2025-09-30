#include "FrameManager.h"
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <numeric>

FrameManager::FrameManager(uint64_t num_frames, ReplacementPolicy repl_policy, AllocationPolicy alloc_policy, const std::vector<MemoryAccess>& accesses)
    : num_frames(num_frames), replacement_policy(repl_policy), allocation_policy(alloc_policy), memory_accesses(accesses) {
    
    frame_table.resize(num_frames);
    for (uint64_t i = 0; i < num_frames; ++i) {
        frame_table[i] = {true, -1, 0};
        free_frames.push_back(i);
    }

    if (allocation_policy == AllocationPolicy::LOCAL) {
        process_frame_counts.resize(4, num_frames / 4);
        uint64_t remainder = num_frames % 4;
        for (uint64_t i = 0; i < remainder; ++i) {
            process_frame_counts[i]++;
        }
        process_frames.resize(4);
    }
}

uint64_t FrameManager::findFreeFrame() {
    if (free_frames.empty()) {
        return -1; // No free frames
    }
    uint64_t frame_number = free_frames.front();
    free_frames.pop_front();
    return frame_number;
}

void FrameManager::recordAccess(uint64_t frame_number) {
    if (replacement_policy == ReplacementPolicy::LRU) {
        if (lru_map.count(frame_number)) {
            lru_list.erase(lru_map[frame_number]);
        }
        lru_list.push_front(frame_number);
        lru_map[frame_number] = lru_list.begin();
    }
}

uint64_t FrameManager::getFrame(PageTable* page_table, uint64_t virtual_page_number, uint64_t current_access_index, std::vector<PageTable>& all_page_tables, uint64_t page_offset_bits) {
    uint64_t frame_number = -1;

    if (allocation_policy == AllocationPolicy::LOCAL) {
        int pid = page_table->getProcessId();
        if (process_frames[pid].size() < process_frame_counts[pid]) {
             frame_number = findFreeFrame();
             if (frame_number != (uint64_t)-1) {
                process_frames[pid].push_back(frame_number);
             }
        }
    } else { // GLOBAL
        frame_number = findFreeFrame();
    }

    if (frame_number == (uint64_t)-1) { // No free frames or local allocation full
        frame_number = evictFrame(page_table, current_access_index, all_page_tables, page_offset_bits);
    }

    frame_table[frame_number] = {false, page_table->getProcessId(), virtual_page_number};
    
    if (replacement_policy == ReplacementPolicy::FIFO) {
        fifo_queue.push_back(frame_number);
    }
    recordAccess(frame_number);

    return frame_number;
}

uint64_t FrameManager::evictFrame(PageTable* current_page_table, uint64_t current_access_index, std::vector<PageTable>& all_page_tables, uint64_t page_offset_bits) {
    uint64_t victim_frame = -1;

    switch (replacement_policy) {
        case ReplacementPolicy::OPTIMAL:
            victim_frame = evictOptimal(current_page_table, current_access_index, all_page_tables, page_offset_bits);
            break;
        case ReplacementPolicy::FIFO:
            victim_frame = evictFIFO(current_page_table);
            break;
        case ReplacementPolicy::LRU:
            victim_frame = evictLRU(current_page_table);
            break;
        case ReplacementPolicy::RANDOM:
            victim_frame = evictRandom(current_page_table);
            break;
    }

    Frame& victim = frame_table[victim_frame];
    PageTable& owner_pt = all_page_tables[victim.process_id];
    owner_pt.unmapFrame(victim_frame);

    if (allocation_policy == AllocationPolicy::LOCAL) {
        int old_pid = victim.process_id;
        process_frames[old_pid].remove(victim_frame);
        
        int current_pid = current_page_table->getProcessId();
        process_frames[current_pid].push_back(victim_frame);
    }

    return victim_frame;
}

uint64_t FrameManager::evictFIFO(PageTable* current_page_table) {
    if (allocation_policy == AllocationPolicy::LOCAL) {
        int pid = current_page_table->getProcessId();
        for (uint64_t frame : fifo_queue) {
            if (frame_table[frame].process_id == pid) {
                fifo_queue.remove(frame);
                return frame;
            }
        }
    }
    uint64_t victim_frame = fifo_queue.front();
    fifo_queue.pop_front();
    return victim_frame;
}

uint64_t FrameManager::evictLRU(PageTable* current_page_table) {
    if (allocation_policy == AllocationPolicy::LOCAL) {
        int pid = current_page_table->getProcessId();
        for (auto it = lru_list.rbegin(); it != lru_list.rend(); ++it) {
            if (frame_table[*it].process_id == pid) {
                uint64_t victim_frame = *it;
                lru_list.erase(std::next(it).base());
                lru_map.erase(victim_frame);
                return victim_frame;
            }
        }
    }
    uint64_t victim_frame = lru_list.back();
    lru_list.pop_back();
    lru_map.erase(victim_frame);
    return victim_frame;
}

uint64_t FrameManager::evictRandom(PageTable* current_page_table) {
    if (allocation_policy == AllocationPolicy::LOCAL) {
        int pid = current_page_table->getProcessId();
        auto it = process_frames[pid].begin();
        std::advance(it, rand() % process_frames[pid].size());
        return *it;
    } else {
        return rand() % num_frames;
    }
}

uint64_t FrameManager::evictOptimal(PageTable* current_page_table, uint64_t current_access_index, std::vector<PageTable>& all_page_tables, uint64_t page_offset_bits) {
    uint64_t victim_frame = -1;
    long long farthest_access = -1;

    std::vector<uint64_t> frames_to_check;
    if (allocation_policy == AllocationPolicy::LOCAL) {
        frames_to_check.assign(process_frames[current_page_table->getProcessId()].begin(), process_frames[current_page_table->getProcessId()].end());
    } else {
        frames_to_check.resize(num_frames);
        std::iota(frames_to_check.begin(), frames_to_check.end(), 0);
    }

    for (uint64_t frame_num : frames_to_check) {
        Frame& frame = frame_table[frame_num];
        uint64_t vpn = frame.virtual_page_number;

        long long next_use = -1;
        for (uint64_t i = current_access_index + 1; i < memory_accesses.size(); ++i) {
            if (memory_accesses[i].process_id == frame.process_id &&
                (memory_accesses[i].virtual_address >> page_offset_bits) == vpn) {
                next_use = i;
                break;
            }
        }

        if (next_use == -1) { // This page is not used again
            return frame_num;
        }

        if (next_use > farthest_access) {
            farthest_access = next_use;
            victim_frame = frame_num;
        }
    }
    
    if (victim_frame == (uint64_t)-1 && !frames_to_check.empty()) {
        return frames_to_check[0];
    }

    return victim_frame;
}
