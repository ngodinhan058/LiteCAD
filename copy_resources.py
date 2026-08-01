import shutil
import os

src_fonts = r"d:\LibreCAD-2.2.1.5\librecad\support\fonts"
dst_fonts = r"d:\LibreCAD-2.2.1.5\windows\resources\fonts"
src_patterns = r"d:\LibreCAD-2.2.1.5\librecad\support\patterns"
dst_patterns = r"d:\LibreCAD-2.2.1.5\windows\resources\patterns"
src_library = r"d:\LibreCAD-2.2.1.5\librecad\support\library"
dst_library = r"d:\LibreCAD-2.2.1.5\windows\resources\library"

if not os.path.exists(dst_fonts):
    shutil.copytree(src_fonts, dst_fonts)
if not os.path.exists(dst_patterns):
    shutil.copytree(src_patterns, dst_patterns)
if not os.path.exists(dst_library):
    shutil.copytree(src_library, dst_library)

print("Copied resources successfully.")
