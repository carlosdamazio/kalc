pub struct FrameBuffer {
    base:  *const u64,
    size:  u32,
    x_res: u32,
    y_res: u32,
    pub ppsl:  u32
}

struct PSF1Header {
    magic: [char; 2],
    mode:  char,
    size:  char
}

pub struct PSF1Font {
    glyph_buffer: *mut u64,
    header:       *const PSF1Header
}
