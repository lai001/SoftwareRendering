use crate::texture::{ETextureFormat, ETextureType, Texture, TextureDescriptor};

#[derive(Debug)]
pub struct FrameBuffer {
    pub(crate) texture: Texture,
    pub(crate) z_buffer: Box<Vec<f32>>,
}

impl FrameBuffer {
    pub fn from_texture(texture: Texture) -> FrameBuffer {
        assert_eq!(texture.descriptor.r#type, ETextureType::Dim2D);
        assert_eq!(texture.descriptor.array_size, 1);
        assert_eq!(texture.descriptor.format, ETextureFormat::R8g8b8a8Unorm);
        let z_buffer: Vec<f32> = vec![
            Self::DEFAULT_ZVALUE;
            (texture.descriptor.width * texture.descriptor.height * 4)
                .try_into()
                .unwrap()
        ];
        let z_buffer: Box<Vec<f32>> = Box::new(z_buffer);
        FrameBuffer {
            texture: texture,
            z_buffer,
        }
    }

    pub fn get_color_attachment(&self) -> &Texture {
        &self.texture
    }

    pub fn get_depth_texture(&self) -> Texture {
        let width = self.get_color_attachment().descriptor.width;
        let height = self.get_color_attachment().descriptor.height;
        let descriptor = TextureDescriptor {
            width: width,
            height: height,
            array_size: 1,
            format: ETextureFormat::R8g8b8a8Unorm,
            r#type: ETextureType::Dim2D,
        };
        let mut texture = Texture::from_descriptor(descriptor);
        let buffer = texture.get_mut_buffers().get_mut(0).unwrap();
        for hi in 0..height {
            for wi in 0..width {
                let index = hi * width + wi;
                let z_value = (self.z_buffer.get(index).unwrap() + 1.0) / 2.0 * 255.0;
                buffer[index * 4 + 0] = z_value as u8;
                buffer[index * 4 + 1] = z_value as u8;
                buffer[index * 4 + 2] = z_value as u8;
                buffer[index * 4 + 3] = 255;
            }
        }
        texture
    }
}

impl FrameBuffer {
    pub const DEFAULT_ZVALUE: f32 = 1.0;
}
