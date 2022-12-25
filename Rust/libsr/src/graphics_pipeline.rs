use crate::depth_cmp::*;
use crate::shader::*;

pub enum EFaceCullingMode {
    None,
    Front,
    Back,
}

pub struct GraphicsPipeline<'a, T> {
    pub(crate) shader: &'a dyn TShader<T>,
    pub depth_cmp_func: EDepthCmpFunc,
    pub face_culling_mode: EFaceCullingMode,
}

impl<'a, T> GraphicsPipeline<'a, T> {
    pub fn from_shader(shader: &'a dyn TShader<T>) -> GraphicsPipeline<'a, T> {
        GraphicsPipeline {
            shader: shader,
            depth_cmp_func: EDepthCmpFunc::Less,
            face_culling_mode: EFaceCullingMode::Back,
        }
    }
}
