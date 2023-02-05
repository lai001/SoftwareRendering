use nalgebra::{Matrix4, Vector3};

pub struct Camera {
    projection: Matrix4<f32>,
    view: Matrix4<f32>,
}

impl Camera {
    pub fn new(aspect: f32, fov: f32, near_z: f32, far_z: f32) -> Camera {
        let mut camera = Camera {
            projection: Matrix4::identity(),
            view: Matrix4::identity(),
        };
        camera.set_projection(aspect, fov, near_z, far_z);
        camera.set_view(
            &Vector3::new(0.0_f32, 0.0, 0.0),
            &Vector3::new(0.0_f32, 0.0, 1.0),
            &Vector3::y(),
        );
        camera
    }

    pub fn get_view(&self) -> &Matrix4<f32> {
        &self.view
    }

    pub fn get_projection(&self) -> &Matrix4<f32> {
        &self.projection
    }

    pub fn set_eye(&mut self, eye: &Vector3<f32>) {
        let translation = Matrix4::new_translation(&(-1.0_f32 * eye));
        self.view = translation * self.view;
    }

    fn set_projection(&mut self, aspect: f32, fov: f32, near_z: f32, far_z: f32) {
        let near_sub_far = near_z - far_z;
        self.projection.fill(0.0_f32);
        self.projection[(0, 0)] = 1.0_f32 / (fov / 2.0).tan();
        self.projection[(1, 1)] = self.projection[(0, 0)] * aspect;
        self.projection[(2, 2)] = (-near_z - far_z) / near_sub_far;
        self.projection[(2, 3)] = 2.0_f32 * near_z * far_z / near_sub_far;
        self.projection[(3, 2)] = 1.0_f32;
    }

    fn set_view(&mut self, eye: &Vector3<f32>, target: &Vector3<f32>, up: &Vector3<f32>) {
        let translation = Matrix4::new_translation(&(-1.0_f32 * eye));
        let z_axis = (target - eye).normalize();
        let x_axis = up.cross(&z_axis).normalize();
        let y_axis = z_axis.cross(&x_axis);
        let rotation = Matrix4::new(
            x_axis.x, y_axis.x, z_axis.x, 0.0_f32, //
            x_axis.y, y_axis.y, z_axis.y, 0.0_f32, //
            x_axis.z, y_axis.z, z_axis.z, 0.0_f32, //
            0.0_f32, 0.0_f32, 0.0_f32, 1.0_f32, //
        )
        .transpose();
        self.view = rotation * translation;
    }
}
