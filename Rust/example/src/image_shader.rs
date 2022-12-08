use nalgebra::{Vector2, Vector3, Vector4};
use sr::{shader::*, texture::*};

pub struct ImageShader<'a> {
    pub texture: &'a Texture,
}

#[derive(Debug)]
pub struct ImageShaderVertex {
    pub position: Vector2<f32>,
    pub uv: Vector2<f32>,
}

impl ShaderVertexData for ImageShaderVertex {}

impl TShader<ImageShaderVertex> for ImageShader<'_> {
    fn vertex(&self, vertex_data: &ImageShaderVertex) -> RasterizationData {
        let out = RasterizationData {
            position: Vector4::new(vertex_data.position.x, vertex_data.position.y, 0.0, 1.0),
            extra_datas: vec![EShaderExtraData::Vec2(vertex_data.uv)],
        };
        out
    }

    fn fragment(&self, rasterization_data: &RasterizationData) -> Vector4<f32> {
        match &rasterization_data.extra_datas[0] {
            EShaderExtraData::Vec2(uv) => {
                let uv: Vector3<f32> = Vector3::new(uv.x, uv.y, 0.0);
                let color = self.texture.sample(&uv);
                color
            }
            _ => Vector4::from_element(0.0),
        }
    }
}
