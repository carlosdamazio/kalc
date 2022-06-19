use core::arch::asm;

// idle() idles the CPU to a constant activity
pub fn idle() -> () {
    unsafe { asm!("hlt") }
}
