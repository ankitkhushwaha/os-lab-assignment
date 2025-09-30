#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <cstdint>
#include <unordered_map>
#include <vector>

class PageTable {
public:
    PageTable();
    bool isPagePresent(uint64_t virtual_page_number);
    uint64_t getFrame(uint64_t virtual_page_number);
    void mapPageToFrame(uint64_t virtual_page_number, uint64_t frame_number);
    uint64_t unmapFrame(uint64_t frame_number);
    const std::unordered_map<uint64_t, uint64_t>& getPageToFrameMap() const;
    const std::unordered_map<uint64_t, uint64_t>& getFrameToPageMap() const;
    int getProcessId() const;
    void setProcessId(int pid);
    
private:
    int process_id;
    std::unordered_map<uint64_t, uint64_t> page_to_frame; // virtual page -> frame
    std::unordered_map<uint64_t, uint64_t> frame_to_page; // frame -> virtual page
};

#endif // PAGETABLE_H
