pub mod preset_color {
    use nalgebra::Vector4;

    pub fn black() -> Vector4<f32> {
        Vector4::new(0.0, 0.0, 0.0, 1.0)
    }

    pub fn red() -> Vector4<f32> {
        Vector4::new(1.0, 0.0, 0.0, 1.0)
    }

    pub fn green() -> Vector4<f32> {
        Vector4::new(0.0, 1.0, 0.0, 1.0)
    }

    pub fn blue() -> Vector4<f32> {
        Vector4::new(0.0, 0.0, 1.0, 1.0)
    }

    pub fn white() -> Vector4<f32> {
        Vector4::new(1.0, 1.0, 1.0, 1.0)
    }
}
