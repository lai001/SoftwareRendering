use crate::{
    depth_cmp::EDepthCmpFunc,
    shader::{Shader, ShaderVertexData},
};
use nalgebra::{Vector1, Vector3, Vector4};
use std::sync::{Arc, Mutex};

#[derive(Clone, Copy)]
pub enum EFaceCullingMode {
    None,
    Front,
    Back,
}

pub enum EBlendOp {
    Add,
    Subtract,
    RevSubtract,
    Min,
    Max,
    Custom(fn(Vector4<f32>, Vector4<f32>) -> Vector4<f32>),
}

pub enum EBlendColor {
    Zero,
    One,
    SrcColor,
    InvSrcColor,
    SrcAlpha,
    InvSrcAlpha,
    DestAlpha,
    InvDestAlpha,
    DestColor,
    InvDestColor,
    SrcAlphaSat,
    BlendFactor,
    InvBlendFactor,
    Src1Color,
    InvSrc1Color,
    Src1Alpha,
    InvSrc1Alpha,
    Custom(Vector3<f32>),
}

pub enum EBlendAlpha {
    Zero,
    One,
    SrcAlpha,
    InvSrcAlpha,
    DestAlpha,
    InvDestAlpha,
    SrcAlphaSat,
    BlendFactor,
    InvBlendFactor,
    Src1Alpha,
    InvSrc1Alpha,
    Custom(Vector1<f32>),
}

pub struct BlendDescriptor {
    pub is_blend_enable: bool,
    pub src_blend: EBlendColor,
    pub dest_blend: EBlendColor,
    pub src_blend_alpha: EBlendAlpha,
    pub dest_blend_alpha: EBlendAlpha,
    pub blend_op: EBlendOp,
    pub blend_op_alpha: EBlendOp,
}

pub struct GraphicsPipeline<T: ShaderVertexData> {
    pub(crate) shader: Arc<Mutex<dyn Shader<T>>>,
    pub depth_cmp_func: EDepthCmpFunc,
    pub face_culling_mode: EFaceCullingMode,
    pub blend_descriptor: BlendDescriptor,
}

impl<T: ShaderVertexData> GraphicsPipeline<T> {
    pub fn from_shader(shader: Arc<Mutex<dyn Shader<T>>>) -> GraphicsPipeline<T> {
        GraphicsPipeline {
            shader,
            depth_cmp_func: EDepthCmpFunc::Less,
            face_culling_mode: EFaceCullingMode::Back,
            blend_descriptor: BlendDescriptor {
                is_blend_enable: false,
                src_blend: EBlendColor::One,
                dest_blend: EBlendColor::Zero,
                src_blend_alpha: EBlendAlpha::One,
                dest_blend_alpha: EBlendAlpha::Zero,
                blend_op: EBlendOp::Add,
                blend_op_alpha: EBlendOp::Add,
            },
        }
    }
}
