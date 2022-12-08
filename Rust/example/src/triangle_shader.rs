use nalgebra::{Vector2, Vector4};
use sr::shader::*;

pub struct TriangleShader {}

#[derive(Debug)]
pub struct TriangleShaderVertex {
    pub position: Vector2<f32>,
    pub color: Vector4<f32>,
}

impl ShaderVertexData for TriangleShaderVertex {}

impl TShader<TriangleShaderVertex> for TriangleShader {
    fn vertex(&self, vertex_data: &TriangleShaderVertex) -> RasterizationData {
        let out = RasterizationData {
            position: Vector4::new(vertex_data.position.x, vertex_data.position.y, 0.0, 1.0),
            extra_datas: vec![EShaderExtraData::Vec4(vertex_data.color)],
        };
        out
    }

    fn fragment(&self, rasterization_data: &RasterizationData) -> Vector4<f32> {
        match &rasterization_data.extra_datas[0] {
            EShaderExtraData::Vec4(color) => *color,
            _ => Vector4::from_element(0.0),
        }
    }
}
