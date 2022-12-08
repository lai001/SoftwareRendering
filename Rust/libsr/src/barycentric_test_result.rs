use nalgebra::{Vector2, Vector3};

#[derive(Clone, Copy, Debug)]
pub struct BarycentricTestResult {
    pub is_inside_triangle: bool,
    pub w1: f32,
    pub w2: f32,
    pub w3: f32,
}

impl BarycentricTestResult {
    pub fn test(
        a: &Vector2<f32>,
        b: &Vector2<f32>,
        c: &Vector2<f32>,
        x: f32,
        y: f32,
    ) -> BarycentricTestResult {
        let check = |value: f32| (value >= 0.0) && (value <= 1.0);
        let u = Vector3::new(c.x - a.x, b.x - a.x, a.x - x).cross(&Vector3::new(
            c.y - a.y,
            b.y - a.y,
            a.y - y,
        ));
        let w1 = 1.0 - (u.x + u.y) / u.z;
        let w2 = u.y / u.z;
        let w3 = u.x / u.z;
        BarycentricTestResult {
            is_inside_triangle: check(w1) && check(w2) && check(w3),
            w1: w1,
            w2: w2,
            w3: w3,
        }
    }

    pub fn weight(&self) -> Vector3<f32> {
        Vector3::new(self.w1, self.w2, self.w3)
    }
}
