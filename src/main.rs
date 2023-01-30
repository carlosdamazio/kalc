#![no_std]
#![no_main]
extern crate bootloader_api;
extern crate bootloader_x86_64_common;

use bootloader_api::{
    config::{FrameBuffer, Mapping, Mappings},
    info::{FrameBufferInfo, MemoryRegions},
    *,
};
use bootloader_x86_64_common::logger::{LockedLogger, LOGGER};

use core::panic::PanicInfo;

// bootloader config
const CONFIG: bootloader_api::BootloaderConfig = {
    let mut config = bootloader_api::BootloaderConfig::new_default();
    config.kernel_stack_size = 100 * 1024;
    config
};
entry_point!(_start, config = &CONFIG);

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

fn _start(boot_info: &'static mut bootloader_api::BootInfo) -> ! {
    let mut framebuffer = boot_info
        .framebuffer
        .as_mut()
        .unwrap()
        .buffer_mut();
    loop {
        unsafe {
            core::arch::asm!("hlt");
        }
    }
}
