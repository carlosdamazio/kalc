pub struct FrameBuffer {
    base:  *mut u64,
    size:  u32,
    x_res: u32,
    y_res: u32,
    pub ppsl:  u32
}

struct PSF1Header {
    magic: [char; 2],
    mode:  u8,
    size:  u8
}

pub struct PSF1Font {
    glyph_buffer: *mut u64,
    header:       *const PSF1Header
}

struct ScreenCursor {
    x: u32,
    y: u32
}

static OUTPUT_CURSOR: ScreenCursor = ScreenCursor {
    x: 0,
    y: 0
};

const WHITE: u32 = 0xFFFFFF;
const RED:   u32 = 0xFF0000;

unsafe fn put_char(buff: &mut FrameBuffer, font: &mut PSF1Font, chr: char, x_offset: u32, y_offset: u32) -> () {
    let pixel = buff.base;
    let font = font
        .glyph_buffer
        .add(((chr as u64 * (*font.header).size as u64)) as usize) as *mut char;

    let x = 0 as u32;
    let y = 0 as u32;
    while y < y_offset + 16 {
        while x < x_offset + 8 {
            if (*font as u64 & (0b10000000 >> (x - x_offset))) > 0 {
                *(pixel.add((x + (y * buff.ppsl)) as usize)) = WHITE as u64; 
            }
        }
    }
}
