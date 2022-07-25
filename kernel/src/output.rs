use core::str::Chars;

struct ScreenCursor {
    x: u32,
    y: u32
}

static mut OUTPUT_CURSOR: ScreenCursor = ScreenCursor {
    x: 0,
    y: 0
};

const WHITE: u32 = 0xFFFFFF;
const RED:   u32 = 0xFF0000;

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
    header:       *mut PSF1Header
}

pub struct Projector {
    pub buff: *mut FrameBuffer,
    pub font: *mut PSF1Font
}

impl Projector {
    pub unsafe fn kprint(&mut self, string: Chars) -> () {
        for chr in string {
            self.put_char(chr as u64, OUTPUT_CURSOR.x, OUTPUT_CURSOR.y);
            OUTPUT_CURSOR.x += 8;
            if OUTPUT_CURSOR.x + 8 > (*self.buff).x_res {
                wrap();
            }
        }
        wrap();
    }

    unsafe fn put_char(&mut self, chr: u64, x_offset: u32, y_offset: u32) -> () {
        let mut pixel = (*self.buff).base;
        let mut font = (*self.font)
            .glyph_buffer
            .add(((chr * (*(*self.font).header).size as u64)) as usize);

        let mut x = x_offset;
        let mut y = y_offset;
        while y < y_offset + 16 {
            while x < x_offset + 8 {
                if (font as usize & (0b10000000 >> (x - x_offset))) > 0 {
                    pixel = pixel.add((x + (y * (*self.buff).ppsl)) as usize);
                    *pixel = WHITE as u64;
                }
                x += 1;
            }

            font = font.add(1 as usize);
            y += 1
        }
    }
}

unsafe fn wrap() -> () {
    OUTPUT_CURSOR.x = 0;
    OUTPUT_CURSOR.y += 16;
}
