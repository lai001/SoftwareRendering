use crate::texture::*;
use std::{cell::RefCell, rc::Rc};

#[derive(Debug)]
pub struct FrameBuffer {
    pub(crate) texture: Rc<RefCell<Texture>>,
    pub(crate) z_buffer: Box<Vec<f32>>,
}

impl FrameBuffer {
    pub fn new(texture: Texture) -> FrameBuffer {
        assert_eq!(texture.descriptor.r#type, ETextureType::Dim2D);
        assert_eq!(texture.descriptor.array_size, 1);
        assert_eq!(texture.descriptor.format, ETextureFormat::R8g8b8a8Unorm);
        let z_buffer: Vec<f32> = vec![
            1.0;
            (texture.descriptor.width * texture.descriptor.height * 4)
                .try_into()
                .unwrap()
        ];
        let z_buffer: Box<Vec<f32>> = Box::new(z_buffer);
        FrameBuffer {
            texture: Rc::new(RefCell::new(texture)),
            z_buffer,
        }
    }

    pub fn get_color_attachment(&self) -> &Rc<RefCell<Texture>> {
        &self.texture
    }
}
