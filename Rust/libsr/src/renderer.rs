use itertools::izip;
use std::rc::Rc;

use crate::{
    barycentric_test_result::BarycentricTestResult,
    depth_cmp::EDepthCmpFunc,
    frame_buffer::*,
    graphics_pipeline::*,
    shader::{EShaderExtraData, RasterizationData, ShaderVertexData},
};
use nalgebra::{Matrix3, Point2, Vector1, Vector2, Vector3, Vector4};

pub struct Renderer<'a> {
    pub(crate) frame_buffer: &'a mut FrameBuffer,
}

impl Renderer<'_> {
    fn clamp_i32(val: &Vector2<i32>, min: &Vector2<i32>, max: &Vector2<i32>) -> Vector2<i32> {
        let x: i32 = nalgebra::clamp(val.x, min.x, max.x);
        let y: i32 = nalgebra::clamp(val.y, min.y, max.y);
        Vector2::new(x, y)
    }

    fn clamp_ndc(val: &Vector2<f32>) -> Vector2<f32> {
        let x: f32 = nalgebra::clamp(val.x, -1.0, 1.0);
        let y: f32 = nalgebra::clamp(val.y, -1.0, 1.0);
        Vector2::new(x, y)
    }

    fn ndc_to_tex_coord(val: &Vector2<f32>) -> Vector2<f32> {
        let mat: Matrix3<f32> = Matrix3::new(
            1.0 / 2.0,
            0.0,
            1.0 / 2.0,
            0.0,
            1.0 / 2.0,
            1.0 / 2.0,
            0.0,
            0.0,
            1.0,
        );
        let mat1: Matrix3<f32> = Matrix3::new(1.0, 0.0, 0.0, 0.0, -1.0, 1.0, 0.0, 0.0, 1.0);
        let val: Vector3<f32> = Vector3::new(val.x, val.y, 1.0);
        let val: Vector3<f32> = mat1 * mat * val;
        Vector2::new(val.x, val.y)
    }

    fn ndc_to_viewport(&self, point: &Vector2<f32>) -> Option<Vector2<usize>> {
        let (width, height) = self.get_back_buffer_width_height();
        let min_value = -1.0;
        let max_value = 1.0;
        let d = max_value - min_value;
        if point.x >= min_value
            && point.x <= max_value
            && point.y >= min_value
            && point.y <= max_value
        {
            let x = (point.x - min_value) / d * (width - 1) as f32;
            let y = (max_value - point.y) / d * (height - 1) as f32;
            Some(Vector2::new(x as usize, y as usize))
        } else {
            None
        }
        // assert!(point.x >= min_value && point.x <= max_value);
        // assert!(point.y >= min_value && point.y <= max_value);
    }

    fn vector2_order<'a>(
        v1: &'a Vector2<f32>,
        v2: &'a Vector2<f32>,
    ) -> (&'a Vector2<f32>, &'a Vector2<f32>) {
        if v1.x < v2.x {
            (v1, v2)
        } else {
            (v2, v1)
        }
    }
}

impl<'a> Renderer<'a> {
    pub fn new(frame_buffer: &'a mut FrameBuffer) -> Renderer<'a> {
        Renderer { frame_buffer }
    }

    pub fn get_frame_buffer(&self) -> &FrameBuffer {
        &self.frame_buffer
    }

    pub fn get_back_buffer_width_height(&self) -> (usize, usize) {
        let texture = Rc::clone(&self.frame_buffer.texture);
        let descriptor = &texture.borrow().descriptor;
        (descriptor.width, descriptor.height)
    }

    pub fn clean_color(&mut self) {
        let texture = Rc::clone(&self.frame_buffer.texture);
        let mut texture = (&*texture).borrow_mut();
        let buffers = &mut texture.buffers;
        let buffer = buffers.get_mut(0).unwrap();
        buffer.fill(0);
    }

    pub fn clean_depth(&mut self) {
        self.frame_buffer.z_buffer.fill(1.0);
    }

    pub fn set_color_by_index(&mut self, index: usize, color: &Vector4<f32>) {
        let (width, height) = self.get_back_buffer_width_height();
        let texture = Rc::clone(&self.frame_buffer.texture);
        let mut texture = (&*texture).borrow_mut();
        let max_index = height * width;
        let buffers = &mut texture.buffers;
        let buffer = buffers.get_mut(0).expect("Buffer is empty");
        if index < max_index {
            buffer[index * 4 + 0] = (color.x * 255.0) as u8;
            buffer[index * 4 + 1] = (color.y * 255.0) as u8;
            buffer[index * 4 + 2] = (color.z * 255.0) as u8;
            buffer[index * 4 + 3] = (color.w * 255.0) as u8;
        }
    }

    pub fn set_color_by_whi_viewport(&mut self, wi: usize, hi: usize, color: &Vector4<f32>) {
        let (width, _) = self.get_back_buffer_width_height();
        let index = hi * width + wi;
        self.set_color_by_index(index, color);
    }

    pub fn set_color_by_point_ndc(&mut self, point: &Vector2<f32>, color: &Vector4<f32>) {
        let (_, height) = self.get_back_buffer_width_height();
        match self.ndc_to_viewport(point) {
            Some(viewport_pixel_index) => {
                let index = viewport_pixel_index.y * height + viewport_pixel_index.x;
                self.set_color_by_index(index, color);
            }
            None => {}
        }
    }

    pub fn set_color_depth_func(
        &mut self,
        point: &Vector3<f32>,
        color: &Vector4<f32>,
        depth_cmp_func: &EDepthCmpFunc,
    ) {
        let (_, height) = self.get_back_buffer_width_height();

        match self.ndc_to_viewport(&point.xy()) {
            Some(viewport_pixel_index) => {
                let z_value = self
                    .frame_buffer
                    .z_buffer
                    .get_mut(viewport_pixel_index.y * height + viewport_pixel_index.x)
                    .unwrap();
                let is_pass = depth_cmp_func.get_func()(point.z, *z_value);
                if is_pass {
                    *z_value = point.z;
                    self.set_color_by_point_ndc(&point.xy(), color);
                }
            }
            None => {}
        }
    }

    pub fn draw_line2d(&mut self, start: &Vector2<f32>, end: &Vector2<f32>, color: &Vector4<f32>) {
        let (width, height) = self.get_back_buffer_width_height();
        let width = width as f32;
        let height = height as f32;

        let (start, end): (&Vector2<f32>, &Vector2<f32>) = Renderer::vector2_order(start, end);
        let start: Vector2<f32> = Renderer::ndc_to_tex_coord(&Renderer::clamp_ndc(&start));
        let end: Vector2<f32> = Renderer::ndc_to_tex_coord(&Renderer::clamp_ndc(&end));

        let start: Vector2<i32> = Renderer::clamp_i32(
            &Vector2::new((start.x * width) as i32, (start.y * height) as i32),
            &Vector2::new(0, 0),
            &Vector2::new((width - 1.0) as i32, (height - 1.0) as i32),
        );
        let end: Vector2<i32> = Renderer::clamp_i32(
            &Vector2::new((end.x * width) as i32, (end.y * height) as i32),
            &Vector2::new(0, 0),
            &Vector2::new((width - 1.0) as i32, (height - 1.0) as i32),
        );

        let dx = (start.x - end.x).abs();
        let dy = (start.y - end.y).abs();
        let mut y = start.y;
        let mut e = -2 * dx;

        let increment: i32 = (|| {
            if start.y < end.y {
                1
            } else {
                -1
            }
        })();

        for x in start.x..end.x {
            self.set_color_by_whi_viewport(x as usize, y as usize, color);
            e += 2 * dy;
            if e > 0 {
                y += increment;
            }
            if e >= dx {
                e -= 2 * dx;
            }
        }
    }

    pub fn render_graphics_pipeline<T: ShaderVertexData>(
        &mut self,
        graphics_pipeline: &GraphicsPipeline<T>,
        vertex_buffer: &Vec<T>,
        index_buffer: Option<&Vec<u32>>,
    ) {
        let (width, height) = self.get_back_buffer_width_height();
        let (width, height) = (width as f32, height as f32);
        let shader = graphics_pipeline.shader;
        let triangle_count = {
            match index_buffer {
                Some(index_buffer) => {
                    assert_eq!(index_buffer.len() % 3, 0);
                    index_buffer.len() / 3
                }
                None => {
                    assert_eq!(vertex_buffer.len() % 3, 0);
                    vertex_buffer.len() / 3
                }
            }
        };

        for i in (0..triangle_count).step_by(1) {
            let (a, b, c) = {
                match index_buffer {
                    Some(index_buffer) => (
                        vertex_buffer
                            .get(*index_buffer.get(i * 3 + 0).unwrap() as usize)
                            .unwrap(),
                        vertex_buffer
                            .get(*index_buffer.get(i * 3 + 1).unwrap() as usize)
                            .unwrap(),
                        vertex_buffer
                            .get(*index_buffer.get(i * 3 + 2).unwrap() as usize)
                            .unwrap(),
                    ),
                    None => (
                        vertex_buffer.get(i * 3 + 0).unwrap(),
                        vertex_buffer.get(i * 3 + 1).unwrap(),
                        vertex_buffer.get(i * 3 + 2).unwrap(),
                    ),
                }
            };
            let a_rasterization_data = shader.vertex(a);
            let b_rasterization_data = shader.vertex(b);
            let c_rasterization_data = shader.vertex(c);
            assert_eq!(
                a_rasterization_data.extra_datas.len(),
                b_rasterization_data.extra_datas.len()
            );
            assert_eq!(
                b_rasterization_data.extra_datas.len(),
                c_rasterization_data.extra_datas.len()
            );

            let a = Self::divide_by_w(&a_rasterization_data.position);
            let b = Self::divide_by_w(&b_rasterization_data.position);
            let c = Self::divide_by_w(&c_rasterization_data.position);

            if !Self::is_valid_triangle(&a.xy(), &b.xy(), &c.xy()) {
                continue;
            }

            let bounding_box = crate::rect::Rect::bounding_box(&a.xy(), &b.xy(), &c.xy());
            let y_range_start = (bounding_box.y * height) as i32;
            let y_range_end = ((bounding_box.y + bounding_box.height) * height) as i32;
            let y_range = (y_range_start..y_range_end).map(|x| x as f32 / height);

            for y in y_range {
                let x_range_start = (bounding_box.x * width) as i32;
                let x_range_end = ((bounding_box.x + bounding_box.width) * width) as i32;
                let x_range = (x_range_start..x_range_end).map(|x| x as f32 / width);
                for x in x_range {
                    let test_result = BarycentricTestResult::test(&a.xy(), &b.xy(), &c.xy(), x, y);
                    if test_result.is_inside_triangle {
                        let mut data = RasterizationData {
                            position: Vector4::identity(),
                            extra_datas: vec![],
                        };

                        for (i, (a_extra_data, b_extra_data, c_extra_data)) in izip!(
                            &a_rasterization_data.extra_datas,
                            &b_rasterization_data.extra_datas,
                            &c_rasterization_data.extra_datas
                        )
                        .enumerate()
                        {
                            if let (
                                EShaderExtraData::Vec4(a_vec),
                                EShaderExtraData::Vec4(b_vec),
                                EShaderExtraData::Vec4(c_vec),
                            ) = (a_extra_data, b_extra_data, c_extra_data)
                            {
                                let interpolation_data =
                                    Self::project_correction_interpolation_vec4(
                                        &a_vec,
                                        &b_vec,
                                        &c_vec,
                                        a.w,
                                        b.w,
                                        c.w,
                                        &test_result,
                                    );
                                data.extra_datas
                                    .insert(i, EShaderExtraData::Vec4(interpolation_data));
                            } else if let (
                                EShaderExtraData::Vec3(a_vec),
                                EShaderExtraData::Vec3(b_vec),
                                EShaderExtraData::Vec3(c_vec),
                            ) = (a_extra_data, b_extra_data, c_extra_data)
                            {
                                let interpolation_data =
                                    Self::project_correction_interpolation_vec3(
                                        &a_vec,
                                        &b_vec,
                                        &c_vec,
                                        a.w,
                                        b.w,
                                        c.w,
                                        &test_result,
                                    );
                                data.extra_datas
                                    .insert(i, EShaderExtraData::Vec3(interpolation_data));
                            } else if let (
                                EShaderExtraData::Vec2(a_vec),
                                EShaderExtraData::Vec2(b_vec),
                                EShaderExtraData::Vec2(c_vec),
                            ) = (a_extra_data, b_extra_data, c_extra_data)
                            {
                                let interpolation_data =
                                    Self::project_correction_interpolation_vec2(
                                        &a_vec,
                                        &b_vec,
                                        &c_vec,
                                        a.w,
                                        b.w,
                                        c.w,
                                        &test_result,
                                    );
                                data.extra_datas
                                    .insert(i, EShaderExtraData::Vec2(interpolation_data));
                            } else if let (
                                EShaderExtraData::Vec1(a_vec),
                                EShaderExtraData::Vec1(b_vec),
                                EShaderExtraData::Vec1(c_vec),
                            ) = (a_extra_data, b_extra_data, c_extra_data)
                            {
                                let interpolation_data =
                                    Self::project_correction_interpolation_vec1(
                                        &a_vec,
                                        &b_vec,
                                        &c_vec,
                                        a.w,
                                        b.w,
                                        c.w,
                                        &test_result,
                                    );
                                data.extra_datas
                                    .insert(i, EShaderExtraData::Vec1(interpolation_data));
                            } else {
                                panic!("");
                            }
                        }
                        let color = shader.fragment(&data);
                        let z_at_screen_sapce = Vector3::new(a.z, b.z, c.z).dot(&test_result.weight());
                        self.set_color_depth_func(
                            &Vector3::new(x, y, z_at_screen_sapce),
                            &color,
                            &graphics_pipeline.depth_cmp_func,
                        );
                    }
                }
            }
        }
    }

    fn divide_by_w(vec4: &Vector4<f32>) -> Vector4<f32> {
        Vector4::new(vec4.x / vec4.w, vec4.y / vec4.w, vec4.z / vec4.w, vec4.z)
    }

    fn is_valid_triangle(a: &Vector2<f32>, b: &Vector2<f32>, c: &Vector2<f32>) -> bool {
        let a = Point2 { coords: *a };
        let b = Point2 { coords: *b };
        let c = Point2 { coords: *c };
        let d0 = nalgebra::distance(&a, &b);
        let d1 = nalgebra::distance(&b, &c);
        let d2 = nalgebra::distance(&c, &a);
        d0 + d1 > d2 && (d0 - d1).abs() < d2
    }

    // fn interpolation(weight: &Vector3<f32>, value: &Vector3<f32>) -> f32 {
    //     weight.dot(&value)
    // }

    // fn interpolation_vec4(
    //     weight: &Vector3<f32>,
    //     v0: &Vector4<f32>,
    //     v1: &Vector4<f32>,
    //     v2: &Vector4<f32>,
    // ) -> Vector4<f32> {
    //     Vector4::new(
    //         Self::interpolation(weight, &Vector3::new(v0.x, v1.x, v2.x)),
    //         Self::interpolation(weight, &Vector3::new(v0.y, v1.y, v2.y)),
    //         Self::interpolation(weight, &Vector3::new(v0.z, v1.z, v2.z)),
    //         Self::interpolation(weight, &Vector3::new(v0.w, v1.w, v2.w)),
    //     )
    // }

    // fn interpolation_vec3(
    //     weight: &Vector3<f32>,
    //     v0: &Vector3<f32>,
    //     v1: &Vector3<f32>,
    //     v2: &Vector3<f32>,
    // ) -> Vector3<f32> {
    //     Vector3::new(
    //         Self::interpolation(weight, &Vector3::new(v0.x, v1.x, v2.x)),
    //         Self::interpolation(weight, &Vector3::new(v0.y, v1.y, v2.y)),
    //         Self::interpolation(weight, &Vector3::new(v0.z, v1.z, v2.z)),
    //     )
    // }

    // fn interpolation_vec2(
    //     weight: &Vector3<f32>,
    //     v0: &Vector2<f32>,
    //     v1: &Vector2<f32>,
    //     v2: &Vector2<f32>,
    // ) -> Vector2<f32> {
    //     Vector2::new(
    //         Self::interpolation(weight, &Vector3::new(v0.x, v1.x, v2.x)),
    //         Self::interpolation(weight, &Vector3::new(v0.y, v1.y, v2.y)),
    //     )
    // }

    // fn get_weight_at_world_space(
    //     z0: f32,
    //     z1: f32,
    //     z2: f32,
    //     test_result_at_screen_sapce: &BarycentricTestResult,
    // ) -> BarycentricTestResult {
    //     if z0 == z1 && z1 == z2 {
    //         *test_result_at_screen_sapce
    //     } else {
    //         let z_at_world_space = 1.0
    //             / ((test_result_at_screen_sapce.w1 / z0)
    //                 + (test_result_at_screen_sapce.w2 / z1)
    //                 + (test_result_at_screen_sapce.w3 / z2));
    //         BarycentricTestResult {
    //             is_inside_triangle: false,
    //             w1: z_at_world_space * test_result_at_screen_sapce.w1 / z0,
    //             w2: z_at_world_space * test_result_at_screen_sapce.w2 / z1,
    //             w3: z_at_world_space * test_result_at_screen_sapce.w3 / z2,
    //         }
    //     }
    // }

    fn get_zp(
        z0: f32,
        z1: f32,
        z2: f32,
        test_result_at_screen_sapce: &BarycentricTestResult,
    ) -> (f32, f32, f32, f32) {
        let w1 = test_result_at_screen_sapce.w1 / z0;
        let w2 = test_result_at_screen_sapce.w2 / z1;
        let w3 = test_result_at_screen_sapce.w3 / z2;
        (w1 + w2 + w3, w1, w2, w3)
    }

    fn project_correction_interpolation_vec4(
        c0: &Vector4<f32>,
        c1: &Vector4<f32>,
        c2: &Vector4<f32>,
        z0: f32,
        z1: f32,
        z2: f32,
        test_result_at_screen_sapce: &BarycentricTestResult,
    ) -> Vector4<f32> {
        let (zp, w1, w2, w3) = Self::get_zp(z0, z1, z2, test_result_at_screen_sapce);
        (c0 * w1 + c1 * w2 + c2 * w3) / zp
    }

    fn project_correction_interpolation_vec3(
        c0: &Vector3<f32>,
        c1: &Vector3<f32>,
        c2: &Vector3<f32>,
        z0: f32,
        z1: f32,
        z2: f32,
        test_result_at_screen_sapce: &BarycentricTestResult,
    ) -> Vector3<f32> {
        let (zp, w1, w2, w3) = Self::get_zp(z0, z1, z2, test_result_at_screen_sapce);
        (c0 * w1 + c1 * w2 + c2 * w3) / zp
    }

    fn project_correction_interpolation_vec2(
        c0: &Vector2<f32>,
        c1: &Vector2<f32>,
        c2: &Vector2<f32>,
        z0: f32,
        z1: f32,
        z2: f32,
        test_result_at_screen_sapce: &BarycentricTestResult,
    ) -> Vector2<f32> {
        let (zp, w1, w2, w3) = Self::get_zp(z0, z1, z2, test_result_at_screen_sapce);
        (c0 * w1 + c1 * w2 + c2 * w3) / zp
    }

    fn project_correction_interpolation_vec1(
        c0: &Vector1<f32>,
        c1: &Vector1<f32>,
        c2: &Vector1<f32>,
        z0: f32,
        z1: f32,
        z2: f32,
        test_result_at_screen_sapce: &BarycentricTestResult,
    ) -> Vector1<f32> {
        let (zp, w1, w2, w3) = Self::get_zp(z0, z1, z2, test_result_at_screen_sapce);
        (c0 * w1 + c1 * w2 + c2 * w3) / zp
    }
}

#[cfg(test)]
pub mod renderer_test_case {
    use crate::renderer::*;
    use nalgebra::Vector2;

    #[test]
    pub fn test_ndc_to_tex_coord() {
        assert_eq!(
            Renderer::ndc_to_tex_coord(&Vector2::new(-1.0, -1.0)),
            Vector2::new(0.0, 1.0)
        );
        assert_eq!(
            Renderer::ndc_to_tex_coord(&Vector2::new(1.0, 1.0)),
            Vector2::new(1.0, 0.0)
        );
        assert_eq!(
            Renderer::ndc_to_tex_coord(&Vector2::new(-1.0, 1.0)),
            Vector2::new(0.0, 0.0)
        );
        assert_eq!(
            Renderer::ndc_to_tex_coord(&Vector2::new(1.0, -1.0)),
            Vector2::new(1.0, 1.0)
        );
        assert_eq!(
            Renderer::ndc_to_tex_coord(&Vector2::new(0.0, 0.0)),
            Vector2::new(0.5, 0.5)
        );
        assert_eq!(
            Renderer::ndc_to_tex_coord(&Vector2::new(0.5, 0.5)),
            Vector2::new(0.75, 0.25)
        );
    }
}
