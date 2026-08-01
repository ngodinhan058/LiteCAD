import subprocess

def run_git_diff():
    try:
        # Run git status
        result = subprocess.run(["git", "status"], cwd="d:\\LibreCAD-2.2.1.5", capture_output=True, text=True, check=True)
        with open("d:\\LibreCAD-2.2.1.5\\git_status.txt", "w") as f:
            f.write(result.stdout)
            
        # Run git diff
        result2 = subprocess.run(["git", "diff", "origin/master"], cwd="d:\\LibreCAD-2.2.1.5", capture_output=True, text=True, check=True)
        with open("d:\\LibreCAD-2.2.1.5\\git_diff.txt", "w") as f:
            f.write(result2.stdout)
            
    except Exception as e:
        with open("d:\\LibreCAD-2.2.1.5\\git_error.txt", "w") as f:
            f.write(str(e))

if __name__ == "__main__":
    run_git_diff()
