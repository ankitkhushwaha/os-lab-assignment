#include "PageTable.h"

PageTable::PageTable() : process_id(-1) {}

bool PageTable::isPagePresent(uint64_t virtual_page_number) {
    return page_to_frame.count(virtual_page_number) > 0;
}

uint64_t PageTable::getFrame(uint64_t virtual_page_number) {
    return page_to_frame.at(virtual_page_number);
}

void PageTable::mapPageToFrame(uint64_t virtual_page_number, uint64_t frame_number) {
    page_to_frame[virtual_page_number] = frame_number;
    frame_to_page[frame_number] = virtual_page_number;
}

uint64_t PageTable::unmapFrame(uint64_t frame_number) {
    uint64_t virtual_page_number = frame_to_page.at(frame_number);
    page_to_frame.erase(virtual_page_number);
    frame_to_page.erase(frame_number);
    return virtual_page_number;
}

const std::unordered_map<uint64_t, uint64_t>& PageTable::getPageToFrameMap() const {
    return page_to_frame;
}

const std::unordered_map<uint64_t, uint64_t>& PageTable::getFrameToPageMap() const {
    return frame_to_page;
}

int PageTable::getProcessId() const {
    return process_id;
}

void PageTable::setProcessId(int pid) {
    process_id = pid;
}
