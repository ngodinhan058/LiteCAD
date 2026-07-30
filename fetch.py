import urllib.request
url = 'https://raw.githubusercontent.com/LibreCAD/LibreCAD/2.2.1/libraries/jwwlib/src/dl_jww.cpp'
print("Downloading from", url)
try:
    urllib.request.urlretrieve(url, 'official_dl_jww.cpp')
    print("Downloaded successfully to official_dl_jww.cpp")
except Exception as e:
    print("Error:", e)
