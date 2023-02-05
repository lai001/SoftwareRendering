use nalgebra::{Vector2, Vector3, Vector4};
use sr::{
    shader::{EShaderAttribute, RasterizationData, Shader, ShaderVertexData},
    texture::Texture,
};
use std::sync::Arc;

pub struct ImageShader {
    pub texture: Arc<Texture>,
}

#[derive(Debug, Clone, Copy)]
pub struct ImageShaderVertex {
    pub position: Vector2<f32>,
    pub uv: Vector2<f32>,
}

impl ShaderVertexData for ImageShaderVertex {}

impl Shader<ImageShaderVertex> for ImageShader {
    fn vertex(&self, vertex_data: &ImageShaderVertex) -> RasterizationData {
        let out = RasterizationData {
            position: Vector4::new(vertex_data.position.x, vertex_data.position.y, 0.0, 1.0),
            attributes: vec![EShaderAttribute::Vec2(vertex_data.uv)],
        };
        out
    }

    fn fragment(&self, rasterization_data: &RasterizationData) -> Vector4<f32> {
        match &rasterization_data.attributes[0] {
            EShaderAttribute::Vec2(uv) => {
                let uv: Vector3<f32> = Vector3::new(uv.x, uv.y, 0.0);
                let color = self.texture.sample(&uv);
                color
            }
            _ => Vector4::from_element(0.0),
        }
    }
}
