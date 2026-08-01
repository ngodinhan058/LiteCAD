import sys

def analyze():
    unique_pairs = set()
    try:
        with open("d:\\LibreCAD-2.2.1.5\\debug_draw.txt", "r") as f:
            for line in f:
                if "drawEntityPlain:" in line:
                    parts = line.strip().split(", ")
                    if len(parts) >= 2:
                        type_str = parts[0].split("=")[-1]
                        parent_str = parts[1].split("=")[-1]
                        unique_pairs.add((type_str, parent_str))
        
        print("Unique pairs (type, parent_type):")
        for p in unique_pairs:
            print(f"Type: {p[0]}, Parent: {p[1]}")
    except Exception as e:
        print("Error:", e)

if __name__ == "__main__":
    analyze()
