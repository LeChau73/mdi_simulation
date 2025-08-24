import argparse
import csv

def parse_telegram(hex_string, delimiter):
    # loại bỏ khoảng trắng, xuống dòng
    hex_string = "".join(hex_string.split())
    data = bytes.fromhex(hex_string)

    # tách theo delimiter
    fields = data.split(delimiter)

    results = []
    for field in fields:
        if not field:
            continue
        try:
            ascii_value = field.decode("ascii")
        except UnicodeDecodeError:
            ascii_value = field.hex()
        results.append((ascii_value, len(field)))
    return results

def main():
    parser = argparse.ArgumentParser(description="Parse telegram into fields")
    parser.add_argument("input_file", help="Input file chứa hex string (txt)")
    parser.add_argument("delimiter", help="Delimiter ở dạng hex, ví dụ 2c cho dấu phẩy ','")
    parser.add_argument("-o", "--output", default="output.csv", help="Output CSV file")

    args = parser.parse_args()

    # đọc hex từ file
    with open(args.input_file, "r", encoding="utf-8") as f:
        hex_string = f.read()

    delimiter = bytes.fromhex(args.delimiter)
    results = parse_telegram(hex_string, delimiter)

    with open(args.output, "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["Value", "ByteLen"])
        for val, blen in results:
            writer.writerow([val, blen])

    print(f"✅ Parsed {len(results)} fields. Saved to {args.output}")

if __name__ == "__main__":
    main()
