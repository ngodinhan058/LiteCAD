import sys

def count_types():
    counts = {}
    try:
        with open("d:\\LibreCAD-2.2.1.5\\debug_draw.txt", "r", encoding='utf-8', errors='ignore') as f:
            for line in f:
                if "drawEntityPlain: type=" in line:
                    parts = line.strip().split(", ")
                    if len(parts) >= 2:
                        t = parts[0].split("=")[1]
                        counts[t] = counts.get(t, 0) + 1
        
        with open("d:\\LibreCAD-2.2.1.5\\type_counts.txt", "w") as out:
            for k, v in counts.items():
                out.write(f"Type {k}: {v} times\n")
    except Exception as e:
        with open("d:\\LibreCAD-2.2.1.5\\type_counts.txt", "w") as out:
            out.write(f"Error: {e}")

if __name__ == "__main__":
    count_types()
