use nalgebra::{Vector2, Vector3, Vector4};
use sr::{shader::*, texture::*};

pub struct ModelShader<'a> {
    pub texture: &'a Texture,
    pub model_matrix: nalgebra::Matrix4<f32>,
    pub view_matrix: nalgebra::Matrix4<f32>,
    pub projection_matrix: nalgebra::Matrix4<f32>,
}

#[derive(Debug)]
pub struct ModelShaderVertex {
    pub position: Vector3<f32>,
    pub vertext_color: Vector4<f32>,
    pub uv: Vector2<f32>,
}

impl ShaderVertexData for ModelShaderVertex {}

impl TShader<ModelShaderVertex> for ModelShader<'_> {
    fn vertex(&self, vertex_data: &ModelShaderVertex) -> RasterizationData {
        let position = self.projection_matrix
            * self.view_matrix
            * self.model_matrix
            * Vector4::new(
                vertex_data.position.x,
                vertex_data.position.y,
                vertex_data.position.z,
                1.0,
            );
        let uv = Vector3::new(vertex_data.uv.x, vertex_data.uv.y, 0.0);
        let out = RasterizationData {
            position: position,
            extra_datas: vec![
                EShaderExtraData::Vec4(vertex_data.vertext_color),
                EShaderExtraData::Vec3(uv),
            ],
        };
        out
    }

    fn fragment(&self, rasterization_data: &RasterizationData) -> Vector4<f32> {
        if let (EShaderExtraData::Vec4(color), EShaderExtraData::Vec3(uv)) = (
            &rasterization_data.extra_datas[0],
            &rasterization_data.extra_datas[1],
        ) {
            // self.texture.sample(uv).component_mul(color)
            self.texture.sample(uv)
            // *color
        } else {
            Vector4::new(0.0, 0.0, 0.0, 1.0)
        }
    }
}
