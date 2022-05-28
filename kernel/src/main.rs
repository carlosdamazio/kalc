#![no_std]
#![no_main]

mod interrupt;
mod output;

use core::panic::PanicInfo;

use interrupt::idle;
use output::{FrameBuffer, PSF1Font};


#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[no_mangle]
pub extern "C" fn _start(_buff: &mut FrameBuffer, _font: &mut PSF1Font) -> ! {
    loop {
        idle();
    }
}
