
import re
import argparse
import csv

def parse_hex_file(input_file, delimiter_pattern, output_file):
    with open(input_file, "r") as f:
        hex_data = f.read().strip()

    # Chuyển hex -> bytes
    raw_bytes = bytes.fromhex(hex_data)

    # Decode thành ASCII (nếu có ký tự không decode được thì bỏ qua)
    ascii_str = raw_bytes.decode("ascii", errors="ignore")

    # Split theo regex delimiter
    fields = re.split(delimiter_pattern, ascii_str)

    # Ghi ra CSV
    with open(output_file, "w", newline="", encoding="utf-8") as csvfile:
        writer = csv.writer(csvfile)
        for i, field in enumerate(fields, start=1):
            writer.writerow([f"Field{i}", field, len(field.encode("ascii"))])

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Hex frame parser with regex delimiter")
    parser.add_argument("input_file", help="Input hex data file")
    parser.add_argument("-d", "--delimiter", required=True, help="Regex delimiter pattern, e.g. ' {2,4}' for INVOS5100C")
    parser.add_argument("-o", "--output", required=True, help="Output CSV file")
    args = parser.parse_args()

    parse_hex_file(args.input_file, args.delimiter, args.output)
