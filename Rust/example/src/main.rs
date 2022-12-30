use nalgebra::Isometry3;
use nalgebra::Matrix4;
use nalgebra::Perspective3;
use nalgebra::Point3;
use nalgebra::Vector2;
use nalgebra::Vector3;
use nalgebra::Vector4;
use sr::benchmark::Benchmark;
use sr::color::preset_color;
use sr::depth_cmp::EDepthCmpFunc;
use sr::frame_buffer::*;
use sr::graphics_pipeline::*;
use sr::renderer::*;
use sr::texture::*;
use sr_example::camera::Camera;
use sr_example::image_shader::*;
use sr_example::model_shader::*;
use sr_example::triangle_shader::*;

fn save_file_to_disk(texture: &Texture, frame_index: u32) {
    let descriptor = &texture.descriptor;
    let write_buffer = texture.get_buffers().get(0).unwrap().as_slice();

    let path = std::path::Path::new(std::env::current_dir().unwrap().to_str().unwrap())
        .join("target")
        .join("output")
        .join(format!("image{frame_index}.png"));
    let path = path.to_str().unwrap();
    {
        let path = std::path::Path::new(path);
        let prefix = path.parent().unwrap();
        std::fs::create_dir_all(prefix).unwrap();
    }
    image::save_buffer(
        path,
        write_buffer,
        descriptor.width.try_into().unwrap(),
        descriptor.height.try_into().unwrap(),
        image::ColorType::Rgba8,
    )
    .expect("Failed to write file to disk");
    println!("Write to {}\n", path);
}

fn test_draw_line(renderer: &mut Renderer) {
    renderer.draw_line2d(
        &Vector2::new(-1.0, 1.0),
        &Vector2::new(1.0, -1.0),
        &Vector4::new(1.0, 0.0, 0.0, 1.0),
    );
}

fn test_draw_image(renderer: &mut Renderer, input_texture: &Texture) {
    let vertex_buffer = vec![
        ImageShaderVertex {
            position: Vector2::new(-1.0, 1.0),
            uv: Vector2::new(0.0, 0.0),
        },
        ImageShaderVertex {
            position: Vector2::new(1.0, 1.0),
            uv: Vector2::new(1.0, 0.0),
        },
        ImageShaderVertex {
            position: Vector2::new(-1.0, -1.0),
            uv: Vector2::new(0.0, 1.0),
        },
    ];
    let shader = ImageShader {
        texture: &input_texture,
    };
    let mut graphics_pipeline = GraphicsPipeline::from_shader(&shader);
    graphics_pipeline.face_culling_mode = EFaceCullingMode::None;
    renderer.render_graphics_pipeline(&graphics_pipeline, &vertex_buffer, None);
}

fn test_draw_triangle(renderer: &mut Renderer) {
    let vertex_buffer = vec![
        TriangleShaderVertex {
            position: Vector2::new(1.0, 1.0),
            color: preset_color::red(),
        },
        TriangleShaderVertex {
            position: Vector2::new(1.0, -1.0),
            color: preset_color::green(),
        },
        TriangleShaderVertex {
            position: Vector2::new(-1.0, -1.0),
            color: preset_color::blue(),
        },
    ];

    let shader = TriangleShader {};
    let mut graphics_pipeline = GraphicsPipeline::from_shader(&shader);
    graphics_pipeline.face_culling_mode = EFaceCullingMode::None;
    graphics_pipeline.depth_cmp_func = EDepthCmpFunc::Always;
    renderer.render_graphics_pipeline(
        &graphics_pipeline,
        &vertex_buffer,
        Some(&vec![0 as u32, 1, 2]),
    );
}

fn test_draw_cube(renderer: &mut Renderer, input_texture: &Texture, time: f32, frame_index: u32) {
    let vertex_buffer = vec![
        ModelShaderVertex {
            position: Vector3::new(-1.0, 1.0, -1.0),
            vertext_color: preset_color::red(),
            uv: Vector2::new(0.0, 0.0),
        },
        ModelShaderVertex {
            position: Vector3::new(1.0, 1.0, -1.0),
            vertext_color: preset_color::green(),
            uv: Vector2::new(1.0, 0.0),
        },
        ModelShaderVertex {
            position: Vector3::new(1.0, -1.0, -1.0),
            vertext_color: preset_color::blue(),
            uv: Vector2::new(1.0, 1.0),
        },
        ModelShaderVertex {
            position: Vector3::new(-1.0, -1.0, -1.0),
            vertext_color: preset_color::black(),
            uv: Vector2::new(0.0, 1.0),
        },
        ModelShaderVertex {
            position: Vector3::new(-1.0, 1.0, 1.0),
            vertext_color: preset_color::white(),
            uv: Vector2::new(0.0, 0.0),
        },
        ModelShaderVertex {
            position: Vector3::new(1.0, 1.0, 1.0),
            vertext_color: preset_color::red(),
            uv: Vector2::new(1.0, 0.0),
        },
        ModelShaderVertex {
            position: Vector3::new(1.0, -1.0, 1.0),
            vertext_color: preset_color::green(),
            uv: Vector2::new(1.0, 1.0),
        },
        ModelShaderVertex {
            position: Vector3::new(-1.0, -1.0, 1.0),
            vertext_color: preset_color::blue(),
            uv: Vector2::new(0.0, 1.0),
        },
    ];
    let rotation = time * 30.0 * 3.0;
    let scale = 0.5_f32;

    let projection = Perspective3::new(1.0 / 1.0, 39.5978_f32.to_radians(), 0.01, 1000.0);
    let eye = Point3::new(0.0, 0.0, -3.2);
    let target = Point3::new(0.0, 0.0, 0.0);
    let view = Isometry3::look_at_rh(&eye, &target, &Vector3::y());

    let model_matrix: Matrix4<f32> = Matrix4::new_translation(&Vector3::new(0.0, 0.0, 0.0))
        * Matrix4::new_rotation(Vector3::new(
            0.0,
            rotation.to_radians(),
            rotation.to_radians(),
        ))
        * Matrix4::new_scaling(scale);

    let shader = ModelShader {
        texture: &input_texture,
        model_matrix: model_matrix,
        view_matrix: view.to_matrix(),
        projection_matrix: *projection.as_matrix(),
    };

    let graphics_pipeline = GraphicsPipeline::from_shader(&shader);
    let index_buffer: Vec<u32> = vec![
        0, 1, 3, //
        1, 2, 3, //
        4, 5, 7, //
        5, 6, 7, //
        0, 4, 7, //
        0, 3, 7, //
        1, 5, 6, //
        1, 6, 2, //
        4, 5, 0, //
        5, 1, 0, //
        7, 6, 3, //
        6, 2, 3, //
    ];
    renderer.render_graphics_pipeline(&graphics_pipeline, &vertex_buffer, Some(&index_buffer));
}

fn test_draw_scene(
    renderer: &mut Renderer,
    scene: &russimp::scene::Scene,
    input_texture: &Texture,
    time: f32,
    frame_index: u32,
) {
    let mut vertex_buffer: Vec<ModelShaderVertex> = vec![];
    let mut index_buffer: Vec<u32> = vec![];

    let mesh = scene.meshes.get(0).unwrap();

    for face in &mesh.faces {
        let indices = &face.0;
        for index in indices {
            index_buffer.insert(index_buffer.len(), *index);
        }
        assert_eq!(indices.len(), 3);

        if let (Some(a), Some(b), Some(c), Some(uv_map)) = (
            mesh.vertices.get(*indices.get(0).unwrap() as usize),
            mesh.vertices.get(*indices.get(1).unwrap() as usize),
            mesh.vertices.get(*indices.get(2).unwrap() as usize),
            mesh.texture_coords.get(0).unwrap(),
        ) {
            let a_texture_coord = uv_map.get(*indices.get(0).unwrap() as usize).unwrap();
            let b_texture_coord = uv_map.get(*indices.get(1).unwrap() as usize).unwrap();
            let c_texture_coord = uv_map.get(*indices.get(2).unwrap() as usize).unwrap();

            let a_vertex = ModelShaderVertex {
                position: Vector3::new(a.x, a.y, a.z),
                vertext_color: preset_color::white(),
                uv: Vector2::new(a_texture_coord.x, a_texture_coord.y),
            };
            let b_vertex = ModelShaderVertex {
                position: Vector3::new(b.x, b.y, b.z),
                vertext_color: preset_color::white(),
                uv: Vector2::new(b_texture_coord.x, b_texture_coord.y),
            };
            let c_vertex = ModelShaderVertex {
                position: Vector3::new(c.x, c.y, c.z),
                vertext_color: preset_color::white(),
                uv: Vector2::new(c_texture_coord.x, c_texture_coord.y),
            };
            vertex_buffer.insert(vertex_buffer.len(), a_vertex);
            vertex_buffer.insert(vertex_buffer.len(), b_vertex);
            vertex_buffer.insert(vertex_buffer.len(), c_vertex);
        }
    }

    let rotation = time * 30.0 * 3.0;
    let scale = 0.5_f32;

    let aspect = {
        let (width, height) = renderer.get_back_buffer_width_height();
        width as f32 / height as f32
    };
    let mut camera = Camera::new(aspect, 39.5978_f32.to_radians(), 0.01, 1000.0);
    camera.set_eye(&Vector3::new(0.0, 0.0, -3.0));

    let model_matrix: Matrix4<f32> = Matrix4::new_translation(&Vector3::new(0.0, 0.0, 0.0))
        * Matrix4::new_rotation(Vector3::new(
            rotation.to_radians(),
            rotation.to_radians(),
            rotation.to_radians(),
        ))
        * Matrix4::new_scaling(scale);

    let shader = ModelShader {
        texture: &input_texture,
        model_matrix: model_matrix,
        view_matrix: *camera.get_view(),
        projection_matrix: *camera.get_projection(),
    };
    let graphics_pipeline = GraphicsPipeline::from_shader(&shader);
    renderer.render_graphics_pipeline(&graphics_pipeline, &vertex_buffer, Some(&index_buffer));
}

fn main() {
    let delta: f32 = 1.0 / 30.0;
    let mut frame_index: u32 = 0;
    let mut time: f32 = frame_index as f32 * delta;
    let descriptor = TextureDescriptor {
        width: 800,
        height: 600,
        array_size: 1,
        format: ETextureFormat::R8g8b8a8Unorm,
        r#type: ETextureType::Dim2D,
    };
    let mut frame_buffer = FrameBuffer::new(Texture::new(descriptor));
    let mut renderer = Renderer::new(&mut frame_buffer);

    let scene = russimp::scene::Scene::from_file(
        "../../Resource/box_with_texutre.fbx",
        vec![
            russimp::scene::PostProcess::Triangulate,
            russimp::scene::PostProcess::MakeLeftHanded,
        ],
    );
    let input_texture = Texture::from_file(&"../../Resource/ColorGrid.png".to_string(), true);
    if let (Ok(input_texture), Ok(scene)) = (input_texture, scene) {
        loop {
            if frame_index >= 60 {
                break;
            }
            let mut benchmark = Benchmark::run();
            renderer.clean_color(None);
            renderer.clean_depth();
            // test_draw_image(&mut renderer, &input_texture);
            // test_draw_triangle(&mut renderer);
            // test_draw_line(&mut renderer);
            // test_draw_cube(&mut renderer, &input_texture, time, frame_index);
            test_draw_scene(&mut renderer, &scene, &input_texture, time, frame_index);
            let clone_texture = renderer
                .get_frame_buffer()
                .get_color_attachment()
                .borrow()
                .to_owned();
            std::thread::spawn(move || {
                save_file_to_disk(&clone_texture, frame_index);
            });
            time += delta;
            frame_index += 1;
            benchmark.end().print(None);
        }
    } else {
        panic!("");
    }
}
