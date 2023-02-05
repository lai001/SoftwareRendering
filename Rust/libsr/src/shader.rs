use nalgebra::{Vector1, Vector2, Vector3, Vector4};

#[derive(Debug)]
pub enum EShaderAttribute {
    Vec4(Vector4<f32>),
    Vec3(Vector3<f32>),
    Vec2(Vector2<f32>),
    Vec1(Vector1<f32>),
}

#[derive(Debug)]
pub struct RasterizationData {
    pub position: Vector4<f32>,
    pub attributes: Vec<EShaderAttribute>,
}

pub trait ShaderVertexData: Clone + Copy + Send + Sync {}

pub trait Shader<T: ShaderVertexData>: Send + Sync {
    fn vertex(&self, vertex_data: &T) -> RasterizationData;
    fn fragment(&self, rasterization_data: &RasterizationData) -> Vector4<f32>;
}
