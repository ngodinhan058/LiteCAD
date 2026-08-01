import os
import time

def find_recent_files():
    target_dir = "d:\\LibreCAD-2.2.1.5\\librecad\\src"
    current_time = time.time()
    two_days_ago = current_time - (2 * 24 * 3600)
    
    modified_files = []
    for root, dirs, files in os.walk(target_dir):
        for file in files:
            filepath = os.path.join(root, file)
            mtime = os.path.getmtime(filepath)
            if mtime > two_days_ago:
                modified_files.append((filepath, mtime))
                
    modified_files.sort(key=lambda x: x[1], reverse=True)
    
    with open("d:\\LibreCAD-2.2.1.5\\recent_files.txt", "w") as f:
        for filepath, mtime in modified_files:
            f.write(f"{filepath}\n")

if __name__ == "__main__":
    find_recent_files()
