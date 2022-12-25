use std::time::{Duration, SystemTime};

pub struct Benchmark {
    start: SystemTime,
    end: SystemTime,
}

impl Benchmark {
    pub fn run() -> Benchmark {
        Benchmark {
            start: SystemTime::now(),
            end: SystemTime::now(),
        }
    }

    pub fn end(&mut self) -> &mut Self {
        self.end = SystemTime::now();
        self
    }

    pub fn duration_sec(&self) -> f32 {
        let duration = self.end.duration_since(self.start).unwrap();
        duration.as_secs_f32()
    }

    pub fn duration(&self) -> Duration {
        let duration = self.end.duration_since(self.start).unwrap();
        duration
    }

    pub fn print(&self, tag: Option<&str>) {
        match tag {
            Some(tag) => println!("[{}] Finished in {}s.", tag, self.duration_sec()),
            None => println!("Finished in {}s.", self.duration_sec()),
        }
    }
}
