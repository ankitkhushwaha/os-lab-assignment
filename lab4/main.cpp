#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

#include "PageTable.h"
#include "FrameManager.h"

// Helper function to parse policies
ReplacementPolicy parse_replacement_policy(const std::string& s) {
    if (s == "OPTIMAL") return ReplacementPolicy::OPTIMAL;
    if (s == "FIFO") return ReplacementPolicy::FIFO;
    if (s == "LRU") return ReplacementPolicy::LRU;
    if (s == "RANDOM") return ReplacementPolicy::RANDOM;
    throw std::invalid_argument("Invalid replacement policy: " + s);
}

AllocationPolicy parse_allocation_policy(const std::string& s) {
    if (s == "GLOBAL") return AllocationPolicy::GLOBAL;
    if (s == "LOCAL") return AllocationPolicy::LOCAL;
    throw std::invalid_argument("Invalid allocation policy: " + s);
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0] << " <page-size> <num-frames> <repl-policy> <alloc-policy> <trace-file>" << std::endl;
        std::cerr << "Example: " << argv[0] << " 4096 256 LRU GLOBAL combined.trace" << std::endl;
        return 1;
    }

    try {
        // 1. Parse command line arguments
        uint64_t page_size = std::stoull(argv[1]);
        uint64_t num_frames = std::stoull(argv[2]);
        ReplacementPolicy repl_policy = parse_replacement_policy(argv[3]);
        AllocationPolicy alloc_policy = parse_allocation_policy(argv[4]);
        std::string trace_file_path = argv[5];

        srand(time(0)); // Seed for random replacement

        // 2. Read trace file
        std::vector<MemoryAccess> memory_accesses;
        std::ifstream trace_file(trace_file_path);
        if (!trace_file.is_open()) {
            std::cerr << "Error: Could not open trace file " << trace_file_path << std::endl;
            return 1;
        }

        std::string line;
        while (std::getline(trace_file, line)) {
            std::stringstream ss(line);
            std::string pid_str, vaddr_str;
            if (std::getline(ss, pid_str, ',') && std::getline(ss, vaddr_str)) {
                 memory_accesses.push_back({std::stoi(pid_str), std::stoull(vaddr_str, nullptr, 16)});
            }
        }
        trace_file.close();

        // 3. Initialize simulation components
        std::vector<PageTable> page_tables(4);
        for(int i=0; i<4; ++i) {
            page_tables[i].setProcessId(i);
        }

        FrameManager frame_manager(num_frames, repl_policy, alloc_policy, memory_accesses);

        // Statistics
        uint64_t total_page_faults = 0;
        std::vector<uint64_t> process_page_faults(4, 0);
        uint64_t page_offset_bits = log2(page_size);

        // 4. Run simulation
        for (uint64_t i = 0; i < memory_accesses.size(); ++i) {
            const auto& access = memory_accesses[i];
            int pid = access.process_id;
            uint64_t vaddr = access.virtual_address;
            uint64_t vpn = vaddr >> page_offset_bits;

            PageTable& current_pt = page_tables[pid];

            if (current_pt.isPagePresent(vpn)) {
                // Page hit
                uint64_t frame_number = current_pt.getFrame(vpn);
                frame_manager.recordAccess(frame_number);
            } else {
                // Page fault
                total_page_faults++;
                process_page_faults[pid]++;

                uint64_t frame_number = frame_manager.getFrame(&current_pt, vpn, i, page_tables, page_offset_bits);
                current_pt.mapPageToFrame(vpn, frame_number);
            }
        }

        // 5. Print results
        std::cout << "Total Page Faults: " << total_page_faults << std::endl;
        for (int i = 0; i < 4; ++i) {
            std::cout << "Process " << i << " Page Faults: " << process_page_faults[i] << std::endl;
        }

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Number out of range. " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
