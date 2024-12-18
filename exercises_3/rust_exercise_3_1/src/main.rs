use libc::{LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_MAGIC2A,
           LINUX_REBOOT_MAGIC2B, LINUX_REBOOT_MAGIC2C, c_int};
use chrono::NaiveDate;

/// Pads the text left with zeros until it has 8 characters
fn pad_with_zeros(val: &str) -> String {
    let mut actual_val = String::with_capacity(8);
    let old_len = val.len();
    let diff = 8 - old_len;
    for _ in 0..diff {
        actual_val.push('0');
    }
    actual_val.push_str(val);
    return actual_val;
}

fn main() {
    let magic_numbers: [c_int; 5] = [LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
                                     LINUX_REBOOT_MAGIC2A, LINUX_REBOOT_MAGIC2B,
                                     LINUX_REBOOT_MAGIC2C];
    for mag_num in magic_numbers {
        println!("{:#x}", mag_num);
    }

    for mag_num in &magic_numbers[1..5] {
        // Hack needed as parse_from_str is wonky unless there are exactly 2
        // characters for day and month
        let val = pad_with_zeros(format!("{:x}", mag_num).as_str());
        let time_obj = NaiveDate::parse_from_str(val.as_str(), "%d%m%Y").unwrap();
        println!("{:?}", time_obj.format("%d-%m-%Y").to_string());
    }

    println!("\n---\nhttps://esc.sh/blog/linux-reboot-easter-egg/\n---\n");
}
