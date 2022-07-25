#![no_std]
#![no_main]

mod interrupt;
mod output;

use core::panic::PanicInfo;

use interrupt::idle;
use output::{FrameBuffer, Projector, PSF1Font};


#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {
        idle();
    }
}

#[no_mangle]
pub extern "C" fn _start(buff: &mut FrameBuffer, font: &mut PSF1Font) -> ! {
    unsafe {
        let mut projector = Projector { buff, font };
        projector.kprint("Hello, world!".chars());
    }

    loop {
        idle();
    }
}
