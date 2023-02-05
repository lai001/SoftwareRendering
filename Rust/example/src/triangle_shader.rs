use nalgebra::{Vector2, Vector4};
use sr::shader::{EShaderAttribute, RasterizationData, Shader, ShaderVertexData};

pub struct TriangleShader {}

#[derive(Debug, Clone, Copy)]
pub struct TriangleShaderVertex {
    pub position: Vector2<f32>,
    pub color: Vector4<f32>,
}

impl ShaderVertexData for TriangleShaderVertex {}

impl Shader<TriangleShaderVertex> for TriangleShader {
    fn vertex(&self, vertex_data: &TriangleShaderVertex) -> RasterizationData {
        let out = RasterizationData {
            position: Vector4::new(vertex_data.position.x, vertex_data.position.y, 0.0, 1.0),
            attributes: vec![EShaderAttribute::Vec4(vertex_data.color)],
        };
        out
    }

    fn fragment(&self, rasterization_data: &RasterizationData) -> Vector4<f32> {
        match &rasterization_data.attributes[0] {
            EShaderAttribute::Vec4(color) => *color,
            _ => Vector4::from_element(0.0),
        }
    }
}
