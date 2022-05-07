#![no_std]
#![no_main]

mod interrupt;
mod output;

use core::panic::PanicInfo;

use interrupt::idle;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[no_mangle]
pub extern "C" fn _start() -> ! {
    loop {
        idle();
    }
}
