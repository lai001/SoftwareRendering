use std::error::Error;
use std::fmt;

#[derive(Debug)]
pub struct RendererError {
    pub reason: String,
}

impl fmt::Display for RendererError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", &self.reason)
    }
}

impl Error for RendererError {}
