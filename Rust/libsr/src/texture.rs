use nalgebra::{Vector3, Vector4};
use std::convert::TryInto;

#[derive(PartialEq, Debug)]
pub enum ETextureType {
    Dim2D,
}

#[derive(PartialEq, Debug)]
pub enum ETextureFormat {
    R8g8b8a8Unorm,
    // R32g32b32a32Float,
}

#[derive(Debug)]
pub struct TextureDescriptor {
    pub width: usize,
    pub height: usize,
    pub array_size: usize,
    pub format: ETextureFormat,
    pub r#type: ETextureType,
}

pub struct Texture {
    pub(crate) buffers: Vec<Box<Vec<u8>>>,
    pub descriptor: TextureDescriptor,
}

impl std::fmt::Debug for Texture {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let mut d = String::from("");
        for (i, item) in self.buffers.iter().enumerate() {
            d.push_str(&format!(" {}: [{}] ", i, item.len()));
        }

        f.debug_struct("Texture")
            .field("buffers", &d)
            .field("descriptor", &self.descriptor)
            .finish()
    }
}

impl Texture {
    fn get_channels(&self) -> usize {
        match self.descriptor.format {
            ETextureFormat::R8g8b8a8Unorm => 4,
        }
    }
}

impl Texture {
    pub fn new(descriptor: TextureDescriptor) -> Texture {
        assert_eq!(descriptor.format, ETextureFormat::R8g8b8a8Unorm);
        assert_eq!(descriptor.r#type, ETextureType::Dim2D);
        assert_eq!(descriptor.array_size, 1);
        assert!(descriptor.width > 0);
        assert!(descriptor.height > 0);

        let mut buffers: Vec<Box<Vec<u8>>> = Vec::new();
        for i in 0..descriptor.array_size {
            let buffer: Vec<u8> = vec![
                0;
                (4 * descriptor.height * descriptor.width)
                    .try_into()
                    .unwrap()
            ];
            let buffer: Box<Vec<u8>> = Box::new(buffer);
            buffers.insert(i.try_into().unwrap(), buffer);
        }
        Texture {
            buffers,
            descriptor,
        }
    }

    pub fn from_file(file_path: &String, is_flipv: bool) -> Result<Texture, Box<dyn std::error::Error>> {
        match image::open(&file_path) {
            Ok(mut dynamic_image) => {
                if is_flipv {
                    dynamic_image = dynamic_image.flipv();
                }
                let image_width = dynamic_image.width() as usize;
                let image_height = dynamic_image.height() as usize;
                let image_buffers = &dynamic_image.into_rgba8();

                let descriptor = TextureDescriptor {
                    width: image_width,
                    height: image_height,
                    array_size: 1,
                    format: ETextureFormat::R8g8b8a8Unorm,
                    r#type: ETextureType::Dim2D,
                };

                let mut texture_buffers: Vec<Box<Vec<u8>>> = Vec::new();
                let mut texture_buffer: Vec<u8> = vec![
                    0;
                    (4 * descriptor.height * descriptor.width)
                        .try_into()
                        .unwrap()
                ];
                texture_buffer.copy_from_slice(image_buffers);
                let texture_buffer: Box<Vec<u8>> = Box::new(texture_buffer);
                texture_buffers.insert(0, texture_buffer);

                Ok(Texture::from_buffers(texture_buffers, descriptor))
            }
            Err(error) => Err(Box::new(error)),
        }
    }

    pub fn from_buffers(buffers: Vec<Box<Vec<u8>>>, descriptor: TextureDescriptor) -> Texture {
        assert_eq!(descriptor.format, ETextureFormat::R8g8b8a8Unorm);
        assert_eq!(descriptor.r#type, ETextureType::Dim2D);
        assert_eq!(descriptor.array_size, 1);
        assert!(descriptor.width > 0);
        assert!(descriptor.height > 0);
        Texture {
            buffers,
            descriptor,
        }
    }

    pub fn get_buffers(&self) -> &Vec<Box<Vec<u8>>> {
        &self.buffers
    }

    pub fn get_mut_buffers(&mut self) -> &mut Vec<Box<Vec<u8>>> {
        &mut self.buffers
    }

    pub fn sample(&self, uv: &Vector3<f32>) -> Vector4<f32> {
        let array_index = uv.z as usize;
        let uv: Vector3<f32> = Vector3::new(
            nalgebra::clamp(uv.x, 0.0, 1.0),
            nalgebra::clamp(uv.y, 0.0, 1.0),
            uv.z,
        );

        let x = nalgebra::min(
            (uv.x * self.descriptor.width as f32) as usize,
            self.descriptor.width - 1,
        );
        let y = nalgebra::min(
            (uv.y * self.descriptor.height as f32) as usize,
            self.descriptor.height - 1,
        );
        let index = (y * self.descriptor.width + x) * self.get_channels();

        let buffer = self.buffers.get(array_index).unwrap();

        Vector4::new(
            buffer[index + 0] as f32 / 255.0,
            buffer[index + 1] as f32 / 255.0,
            buffer[index + 2] as f32 / 255.0,
            buffer[index + 3] as f32 / 255.0,
        )
    }
}
