with open("hankaku.txt", "r") as txt, open("hankaku.c", "w") as c:
    c.write("char hankaku[4096] = {\n")
    for n in range(0x100):
        assert txt.readline() == f"char 0x{n:02x}\n"
        c.write("    ")
        for i in range(16):
            s = txt.readline().strip("\n")
            m = 0
            for ch in s:
                m <<= 1
                if ch == "*":
                    m += 1
            c.write(f"0x{m:02x},")
        c.write("\n")
        assert txt.readline() == "\n"
    assert txt.readline() == ""
    c.write("};")