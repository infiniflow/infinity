#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <cstdint>

int main() {
    mach_vm_size_t vmsize;
    uintptr_t addr = reinterpret_cast<uintptr_t>(&vmsize);
    uintptr_t page_addr = addr & ~(4096 - 1);
    mach_vm_address_t address = (mach_vm_address_t)page_addr;
    vm_region_basic_info_data_t info;
    mach_msg_type_number_t info_count =
        sizeof(size_t) == 8 ? VM_REGION_BASIC_INFO_COUNT_64 : VM_REGION_BASIC_INFO_COUNT;
    memory_object_name_t object;
    mach_vm_region(
        mach_task_self(),
        &address,
        &vmsize,
        VM_REGION_BASIC_INFO,
        (vm_region_info_t)&info,
        &info_count,
        &object
    );
}
