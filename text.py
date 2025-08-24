import matplotlib.pyplot as plt

# Các thành phần khung điện văn
fields = [
    ("Start", "0x01"),
    ("Header", "3 bytes"),
    ("Data Start", "0x02"),
    ("Data (Value1 ... ValueN, delimiter=0x2C)", "variable"),
    ("Data End", "0x03"),
    ("Checksum", "2 bytes"),
    ("End", "0x0D0A")
]

# Vẽ sơ đồ
fig, ax = plt.subplots(figsize=(10, 2))
ax.axis("off")

# Vẽ các ô
x = 0
for name, val in fields:
    width = 1.5 if "Data" not in name else 4
    ax.add_patch(plt.Rectangle((x, 0), width, 1, fill=None, edgecolor="black"))
    ax.text(x + width/2, 0.5, f"{name}\n{val}", ha="center", va="center", fontsize=9)
    x += width

plt.show()
