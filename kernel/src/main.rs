#![no_std]
#![no_main]

extern crate bootloader_api;
extern crate bootloader_x86_64_common;
extern crate conquer_once;
extern crate log;

use core::panic::PanicInfo;
use bootloader_api::info::{FrameBufferInfo, FrameBuffer};
use log::{info, error};

mod klogger;

// bootloader config
const CONFIG: bootloader_api::BootloaderConfig = {
    let mut config = bootloader_api::BootloaderConfig::new_default();
    config.kernel_stack_size = 100 * 1024;
    config
};
bootloader_api::entry_point!(_start, config = &CONFIG);

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    error!("KalcOS kernel panic: {info}");
    loop {
        unsafe {
            core::arch::asm!("cli");
            core::arch::asm!("hlt");
        }
    }
}

fn _start(boot_info: &'static mut bootloader_api::BootInfo) -> ! {
    let framebuffer: &mut FrameBuffer = boot_info
        .framebuffer
        .as_mut()
        .unwrap();
    let framebuffer_info: FrameBufferInfo = framebuffer.info().clone();

    klogger::init(framebuffer.buffer_mut(), framebuffer_info);
    info!("KalcOS v0.1.0");

    loop {
        unsafe {
            core::arch::asm!("hlt");
        }
    }
}
