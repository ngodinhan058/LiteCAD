import os
for root, dirs, files in os.walk('d:/LibreCAD-2.2.1.5'):
    for file in files:
        if file.endswith('.lff'):
            print(os.path.join(root, file))
