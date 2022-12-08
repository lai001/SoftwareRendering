pub enum EDepthCmpFunc {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
}

impl EDepthCmpFunc {
    pub fn get_func(&self) -> fn(f32, f32) -> bool {
        match self {
            EDepthCmpFunc::Never => |_, _| false,
            EDepthCmpFunc::Less => |x, y| x < y,
            EDepthCmpFunc::Equal => |x, y| x == y,
            EDepthCmpFunc::LessEqual => |x, y| x <= y,
            EDepthCmpFunc::Greater => |x, y| x > y,
            EDepthCmpFunc::NotEqual => |x, y| x != y,
            EDepthCmpFunc::GreaterEqual => |x, y| x >= y,
            EDepthCmpFunc::Always => |_, _| true,
        }
    }
}
