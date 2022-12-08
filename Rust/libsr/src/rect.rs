use nalgebra::Vector2;

#[derive(Debug, Clone, Copy)]
pub struct Rect {
    pub x: f32,
    pub y: f32,
    pub width: f32,
    pub height: f32,
}

impl Rect {
    pub fn bounding_box(a: &Vector2<f32>, b: &Vector2<f32>, c: &Vector2<f32>) -> Rect {
        let x = a.x.min(b.x).min(c.x);
        let y = a.y.min(b.y).min(c.y);
        Rect {
            x: x,
            y: y,
            width: a.x.max(b.x).max(c.x) - x,
            height: a.y.max(b.y).max(c.y) - y,
        }
    }
}
