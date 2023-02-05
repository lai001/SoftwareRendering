use std::time::{Duration, SystemTime};

pub struct Benchmark {
    start: SystemTime,
}

impl Benchmark {
    pub fn run() -> Benchmark {
        Benchmark {
            start: SystemTime::now(),
        }
    }

    pub fn duration_sec(&self) -> f32 {
        self.duration().as_secs_f32()
    }

    pub fn duration(&self) -> Duration {
        let end = SystemTime::now();
        let duration = end.duration_since(self.start).unwrap();
        duration
    }

    pub fn print(&self, tag: Option<&str>) {
        match tag {
            Some(tag) => println!("[{}] Finished in {}s.", tag, self.duration_sec()),
            None => println!("Finished in {}s.", self.duration_sec()),
        }
    }
}
