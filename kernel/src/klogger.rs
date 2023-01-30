use bootloader_api::info::FrameBufferInfo;
use bootloader_x86_64_common::logger::LockedLogger;
use conquer_once::spin::OnceCell;
use log;

static KPRINT: OnceCell<LockedLogger> = OnceCell::uninit();

pub fn klog(buffer: &'static mut [u8], buffer_info: FrameBufferInfo) {
    let logger = KPRINT.get_or_init(move || {
        LockedLogger::new(buffer, buffer_info)
    });

    log::set_logger(logger).expect("klogger is already set");
    log::set_max_level(log::LevelFilter::Trace);

    log::info!("KalcOS v0.1.0");
}
