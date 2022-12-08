use nalgebra::Vector1;
use nalgebra::Vector2;
use nalgebra::Vector3;
use nalgebra::Vector4;

#[derive(Debug)]
pub enum EShaderExtraData {
    Vec4(Vector4<f32>),
    Vec3(Vector3<f32>),
    Vec2(Vector2<f32>),
    Vec1(Vector1<f32>),
}

#[derive(Debug)]
pub struct RasterizationData {
    pub position: Vector4<f32>,
    pub extra_datas: Vec<EShaderExtraData>,
}

pub trait ShaderVertexData {}

pub trait TShader<T: ShaderVertexData> {
    fn vertex(&self, vertex_data: &T) -> RasterizationData;
    fn fragment(&self, rasterization_data: &RasterizationData) -> Vector4<f32>;
}
