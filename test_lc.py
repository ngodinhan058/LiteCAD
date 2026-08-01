import subprocess  
import os  
exe_path = r'd:\LibreCAD-2.2.1.5\windows\LibreCAD.exe'  
cwd = r'd:\LibreCAD-2.2.1.5'  
with open(r'd:\LibreCAD-2.2.1.5\lc_debug.log', 'w') as f:  
    try:  
        process = subprocess.Popen([exe_path, r'windows\temp_conversion.dxf'], cwd=cwd, stdout=f, stderr=subprocess.STDOUT)  
        try:  
            process.wait(timeout=5)  
        except subprocess.TimeoutExpired:  
            process.terminate()  
            process.wait()  
    except Exception as e:  
        f.write(str(e)) 
