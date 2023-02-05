use crate::{
    barycentric_test_result::BarycentricTestResult,
    depth_cmp::EDepthCmpFunc,
    frame_buffer::FrameBuffer,
    graphics_pipeline::{EFaceCullingMode, GraphicsPipeline},
    shader::{EShaderAttribute, RasterizationData, Shader, ShaderVertexData},
    texture::{Texture, TextureDescriptor},
};
use nalgebra::{Matrix3, Point2, SVector, Vector2, Vector3, Vector4};
use std::sync::{mpsc::channel, Arc, Mutex};

lazy_static! {
    static ref GLOBAL_RENDERER_THREAD_POOL: Mutex<rayon::ThreadPool> = Mutex::new(
        rayon::ThreadPoolBuilder::new()
            .num_threads(std::thread::available_parallelism().unwrap().get())
            .build()
            .unwrap(),
    );
}

struct RenderResult {
    position: Vector3<f32>,
    color: Vector4<f32>,
}

/**
 * TODO:
 * Viewport clipping
 * Early Depth Test
 * Frustum culling
 * Visibility and Occlusion Culling
 * Blending
 * Stencil Test
 * SIMD
 */
pub struct Renderer {
    pub(crate) frame_buffer: FrameBuffer,
}

impl Renderer {
    fn component_clamp<
        T: nalgebra::Scalar + std::cmp::PartialOrd + Copy + num_traits::Zero,
        const D: usize,
    >(
        val: &SVector<T, D>,
        min: &SVector<T, D>,
        max: &SVector<T, D>,
    ) -> SVector<T, D> {
        let mut output = SVector::<T, D>::zeros();
        for i in 0..D {
            *(&mut output.data.as_mut_slice()).get_mut(i).unwrap() = nalgebra::clamp(
                *val.get(i).unwrap(),
                *min.get(i).unwrap(),
                *max.get(i).unwrap(),
            );
        }
        output
    }

    fn clamp_ndc(val: &Vector2<f32>) -> Vector2<f32> {
        let x: f32 = nalgebra::clamp(val.x, -1.0, 1.0);
        let y: f32 = nalgebra::clamp(val.y, -1.0, 1.0);
        Vector2::new(x, y)
    }

    fn ndc_to_tex_coord(val: &Vector2<f32>) -> Vector2<f32> {
        let m1 = Matrix3::new_scaling(1.0 / 2.0);
        let m2 = Matrix3::new_translation(&Vector2::new(1.0 / 2.0, 1.0 / 2.0));
        let m3 = Matrix3::new_nonuniform_scaling(&Vector2::new(1.0, -1.0));
        let m4 = Matrix3::new_translation(&Vector2::new(0.0, 1.0));
        (m4 * m3 * m2 * m1 * Vector3::new(val.x, val.y, 1.0)).xy()
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

impl Renderer {
    pub fn from_texture_descriptor(descriptor: TextureDescriptor) -> Renderer {
        Renderer {
            frame_buffer: FrameBuffer::from_texture(Texture::from_descriptor(descriptor)),
        }
    }

    pub fn get_frame_buffer(&self) -> &FrameBuffer {
        &self.frame_buffer
    }

    pub fn get_back_buffer_width_height(&self) -> (usize, usize) {
        let texture = &self.frame_buffer.texture;
        let descriptor = &texture.descriptor;
        (descriptor.width, descriptor.height)
    }

    pub fn clear_color(&mut self, color: Option<&Vector4<f32>>) {
        let (width, height) = self.get_back_buffer_width_height();
        let texture = &mut self.frame_buffer.texture;
        let buffers = &mut texture.buffers;
        let buffer = buffers.get_mut(0).unwrap();
        match color {
            Some(color) => {
                for index in 0..height * width {
                    buffer[index * 4 + 0] = (color.x * 255.0) as u8;
                    buffer[index * 4 + 1] = (color.y * 255.0) as u8;
                    buffer[index * 4 + 2] = (color.z * 255.0) as u8;
                    buffer[index * 4 + 3] = (color.w * 255.0) as u8;
                }
            }
            None => buffer.fill(0),
        }
    }

    pub fn clear_depth(&mut self, value: Option<f32>) {
        match value {
            Some(value) => {
                self.frame_buffer.z_buffer.fill(value);
            }
            None => {
                self.frame_buffer.z_buffer.fill(FrameBuffer::DEFAULT_ZVALUE);
            }
        }
    }

    pub fn set_color_by_index(&mut self, index: usize, color: &Vector4<f32>) {
        let (width, height) = self.get_back_buffer_width_height();
        let texture = &mut self.frame_buffer.texture;
        let max_index = height * width;
        let buffers = &mut texture.buffers;
        let buffer = buffers.get_mut(0).unwrap();
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
        match self.ndc_to_viewport(point) {
            Some(viewport_pixel_index) => {
                self.set_color_by_whi_viewport(
                    viewport_pixel_index.x,
                    viewport_pixel_index.y,
                    color,
                );
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
        let (width, _) = self.get_back_buffer_width_height();

        match self.ndc_to_viewport(&point.xy()) {
            Some(viewport_pixel_index) => {
                let z_value = self
                    .frame_buffer
                    .z_buffer
                    .get_mut(viewport_pixel_index.y * width + viewport_pixel_index.x)
                    .unwrap();
                let is_pass = depth_cmp_func.get_func()(point.z, *z_value);
                if is_pass {
                    *z_value = point.z;
                    self.set_color_by_whi_viewport(
                        viewport_pixel_index.x,
                        viewport_pixel_index.y,
                        color,
                    );
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

        let start: Vector2<i32> = Renderer::component_clamp(
            &Vector2::new((start.x * width) as i32, (start.y * height) as i32),
            &Vector2::new(0, 0),
            &Vector2::new((width - 1.0) as i32, (height - 1.0) as i32),
        );
        let end: Vector2<i32> = Renderer::component_clamp(
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

    fn float_range_from(start: f32, end: f32, length: usize) -> Vec<f32> {
        let start = (start * length as f32) as i32;
        let end = (end * length as f32) as i32;
        (start..end).map(|x| x as f32 / length as f32).collect()
    }

    fn is_pass_face_culling(
        a: &Vector3<f32>,
        b: &Vector3<f32>,
        c: &Vector3<f32>,
        face_culling_mode: &EFaceCullingMode,
    ) -> bool {
        match face_culling_mode {
            EFaceCullingMode::None => true,
            EFaceCullingMode::Front => Self::face_culling(a, b, c),
            EFaceCullingMode::Back => Self::face_culling(a, b, c) == false,
        }
    }

    fn render_triangle<T: ShaderVertexData>(
        a: T,
        b: T,
        c: T,
        viewport: Vector2<usize>,
        shader: Arc<Mutex<dyn Shader<T>>>,
        face_culling_mode: EFaceCullingMode,
    ) -> Box<Vec<RenderResult>> {
        let mut render_result = Box::<Vec<RenderResult>>::new(Vec::new());
        let a_rasterization_data: RasterizationData;
        let b_rasterization_data: RasterizationData;
        let c_rasterization_data: RasterizationData;
        {
            let shader = shader.lock().unwrap();
            a_rasterization_data = shader.vertex(&a);
            b_rasterization_data = shader.vertex(&b);
            c_rasterization_data = shader.vertex(&c);
        }

        assert_eq!(
            a_rasterization_data.attributes.len(),
            b_rasterization_data.attributes.len()
        );
        assert_eq!(
            b_rasterization_data.attributes.len(),
            c_rasterization_data.attributes.len()
        );

        let a = Self::divide_by_w(&a_rasterization_data.position);
        let b = Self::divide_by_w(&b_rasterization_data.position);
        let c = Self::divide_by_w(&c_rasterization_data.position);

        if Self::is_pass_face_culling(&a.xyz(), &b.xyz(), &c.xyz(), &face_culling_mode)
            && Self::is_valid_triangle(&a.xy(), &b.xy(), &c.xy())
        {
            let bounding_box = crate::rect::Rect::bounding_box(&a.xy(), &b.xy(), &c.xy());
            let (width, height) = (viewport.x, viewport.y);
            for y in
                Self::float_range_from(bounding_box.y, bounding_box.y + bounding_box.height, height)
            {
                for x in Self::float_range_from(
                    bounding_box.x,
                    bounding_box.x + bounding_box.width,
                    width,
                ) {
                    let test_result = BarycentricTestResult::test(&a.xy(), &b.xy(), &c.xy(), x, y);
                    if test_result.is_inside_triangle {
                        let mut data = RasterizationData {
                            position: Vector4::identity(),
                            attributes: vec![],
                        };

                        for i in 0..a_rasterization_data.attributes.len() {
                            let a_extra_data = &a_rasterization_data.attributes[i];
                            let b_extra_data = &b_rasterization_data.attributes[i];
                            let c_extra_data = &c_rasterization_data.attributes[i];

                            if let (
                                EShaderAttribute::Vec4(a_vec),
                                EShaderAttribute::Vec4(b_vec),
                                EShaderAttribute::Vec4(c_vec),
                            ) = (a_extra_data, b_extra_data, c_extra_data)
                            {
                                let interpolation_data = Self::project_correction_interpolation::<4>(
                                    &a_vec,
                                    &b_vec,
                                    &c_vec,
                                    a.w,
                                    b.w,
                                    c.w,
                                    &test_result,
                                );
                                data.attributes
                                    .push(EShaderAttribute::Vec4(interpolation_data));
                            } else if let (
                                EShaderAttribute::Vec3(a_vec),
                                EShaderAttribute::Vec3(b_vec),
                                EShaderAttribute::Vec3(c_vec),
                            ) = (a_extra_data, b_extra_data, c_extra_data)
                            {
                                let interpolation_data = Self::project_correction_interpolation::<3>(
                                    &a_vec,
                                    &b_vec,
                                    &c_vec,
                                    a.w,
                                    b.w,
                                    c.w,
                                    &test_result,
                                );
                                data.attributes
                                    .push(EShaderAttribute::Vec3(interpolation_data));
                            } else if let (
                                EShaderAttribute::Vec2(a_vec),
                                EShaderAttribute::Vec2(b_vec),
                                EShaderAttribute::Vec2(c_vec),
                            ) = (a_extra_data, b_extra_data, c_extra_data)
                            {
                                let interpolation_data = Self::project_correction_interpolation::<2>(
                                    &a_vec,
                                    &b_vec,
                                    &c_vec,
                                    a.w,
                                    b.w,
                                    c.w,
                                    &test_result,
                                );
                                data.attributes
                                    .push(EShaderAttribute::Vec2(interpolation_data));
                            } else if let (
                                EShaderAttribute::Vec1(a_vec),
                                EShaderAttribute::Vec1(b_vec),
                                EShaderAttribute::Vec1(c_vec),
                            ) = (a_extra_data, b_extra_data, c_extra_data)
                            {
                                let interpolation_data = Self::project_correction_interpolation::<1>(
                                    &a_vec,
                                    &b_vec,
                                    &c_vec,
                                    a.w,
                                    b.w,
                                    c.w,
                                    &test_result,
                                );
                                data.attributes
                                    .push(EShaderAttribute::Vec1(interpolation_data));
                            } else {
                                panic!("");
                            }
                        }
                        let color: Vector4<f32>;
                        {
                            let shader = shader.lock().unwrap();
                            color = shader.fragment(&data);
                        }
                        let z_at_screen_sapce =
                            Vector3::new(a.z, b.z, c.z).dot(&test_result.weight());
                        render_result.push(RenderResult {
                            position: Vector3::new(x, y, z_at_screen_sapce),
                            color,
                        });
                    }
                }
            }
        }
        render_result
    }

    pub fn render_graphics_pipeline<T: ShaderVertexData>(
        &mut self,
        graphics_pipeline: &GraphicsPipeline<T>,
        vertex_buffer: &Vec<T>,
        index_buffer: Option<&Vec<u32>>,
    ) {
        let pool = GLOBAL_RENDERER_THREAD_POOL.lock().unwrap();
        pool.in_place_scope(|scope| {
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
            let (width, height) = self.get_back_buffer_width_height();
            let (sender, receiver) = channel();
            for i in 0..triangle_count {
                let sender = sender.clone();
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
                scope.spawn(move |_| {
                    let render_results = Self::render_triangle(
                        *a,
                        *b,
                        *c,
                        Vector2::new(width, height),
                        Arc::clone(&graphics_pipeline.shader),
                        graphics_pipeline.face_culling_mode,
                    );
                    sender.send(render_results).unwrap();
                });
            }
            for _ in 0..triangle_count {
                let render_results = receiver.recv().unwrap();
                for render_result in *render_results {
                    self.set_color_depth_func(
                        &render_result.position,
                        &render_result.color,
                        &graphics_pipeline.depth_cmp_func,
                    );
                }
            }
        });
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

    fn get_weight_at_world_space(
        z0: f32,
        z1: f32,
        z2: f32,
        test_result_at_screen_sapce: &BarycentricTestResult,
    ) -> (f32, f32, f32) {
        if (z0 == z1) && (z1 == z2) {
            let w1 = test_result_at_screen_sapce.w1;
            let w2 = test_result_at_screen_sapce.w2;
            let w3 = test_result_at_screen_sapce.w3;
            (w1, w2, w3)
        } else {
            let w1 = test_result_at_screen_sapce.w1 / z0;
            let w2 = test_result_at_screen_sapce.w2 / z1;
            let w3 = test_result_at_screen_sapce.w3 / z2;
            let zp = w1 + w2 + w3;
            (w1 / zp, w2 / zp, w3 / zp)
        }
    }

    fn project_correction_interpolation<const D: usize>(
        c0: &nalgebra::SVector<f32, D>,
        c1: &nalgebra::SVector<f32, D>,
        c2: &nalgebra::SVector<f32, D>,
        z0: f32,
        z1: f32,
        z2: f32,
        test_result_at_screen_sapce: &BarycentricTestResult,
    ) -> nalgebra::SVector<f32, D> {
        let (w1, w2, w3) = Self::get_weight_at_world_space(z0, z1, z2, test_result_at_screen_sapce);
        c0 * w1 + c1 * w2 + c2 * w3
    }

    fn face_culling(v1: &Vector3<f32>, v2: &Vector3<f32>, v3: &Vector3<f32>) -> bool {
        let tmp1 = v2 - v1;
        let tmp2 = v3 - v1;
        let normal = tmp1.cross(&tmp2).normalize();
        let view = Vector3::new(0.0, 0.0, 1.0);
        normal.dot(&view) < 0.0
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
