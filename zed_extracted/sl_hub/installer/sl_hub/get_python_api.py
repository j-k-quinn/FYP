import os
import platform
import sys
import re
import requests
import argparse
from pathlib import Path
import subprocess
import sys

ZED_IOT_MAJOR = ""
ZED_IOT_MINOR = ""

CUDA_MAJOR_ZED = ""
CUDA_MINOR_ZED = ""

CUDA_STR = ""

CUDA_MAJOR = "?"
CUDA_MINOR = "?"

PYTHON_MAJOR = ""
PYTHON_MINOR = ""

OS_VERSION = ""
CUDA_INDEPENDANT_VERSION=False
ARCH_VERSION = platform.machine()

whl_platform_str = ""

cuda_path = "/usr/local/cuda"
base_URL = "https://download.stereolabs.com/iot/default/"

def pip_install(package, force_install=False):
    try:
        call_list=[sys.executable, "-m", "pip", "install"]
        if force_install:
            call_list.append("--ignore-installed")
        call_list.append(package)
        err = subprocess.check_call(call_list)
    except Exception as e:
        err = 1
        #print("DEBUG : Exception " + str(e))
    #print("DEBUG : " + package + " errcode " + str(err))
    return err

def check_valid_file(file_path):
    file_size = os.stat(file_path).st_size / 1000.
    # size > 150 Ko
    return file_size > 150

def check_zed_hub_version():
    global ZED_IOT_MAJOR
    global ZED_IOT_MINOR
    global CUDA_INDEPENDANT_VERSION

    CUDA_INDEPENDANT_VERSION = False

    file_path = "/usr/local/sl_hub/settings/version"
    with open(file_path, "r", encoding="utf-8") as myfile:
        data = myfile.read().split(".")

    if len(data) <= 2:
        print ("Invalid version found in /usr/local/sl_hub/settings/version")
        exit(1)

    major = data[0]
    minor = data[1]
    patch = data[2]

    ZED_IOT_MAJOR = major
    ZED_IOT_MINOR = minor

parser = argparse.ArgumentParser(description='Helper script to download and setup the ZED Python API')
parser.add_argument('--path', help='whl file destination path')
args = parser.parse_args()

arch = platform.architecture()[0]
if arch != "64bit":
    print("ERROR : Python 64bit must be used, found " + str(arch))
    sys.exit(1)

# If path empty, take pwd
dirname = args.path or os.getcwd()

# If no write access, download in home
if not (os.path.exists(dirname) and os.path.isdir(dirname) and os.access(dirname, os.W_OK)):
    dirname = str(Path.home())

print("-> Downloading to '" + str(dirname) + "'")

if "linux" in sys.platform:

    if "aarch64" in ARCH_VERSION:
        with open("/etc/nv_tegra_release", "r", encoding="utf-8") as myfile:
            data = myfile.read()
        number_extraction = re.findall(r'\d+', data)
        TEGRA_RELEASE_MAJOR = int(number_extraction[0])
        TEGRA_RELEASE_MINOR = int(number_extraction[1])
        TEGRA_RELEASE_PATCH = int(number_extraction[2])

        print("L4T " + str(TEGRA_RELEASE_MAJOR) + "." + str(TEGRA_RELEASE_MINOR))
        CUDA_STR = "l4t" + str(TEGRA_RELEASE_MAJOR)# + "." + str(TEGRA_RELEASE_MINOR)
        OS_VERSION = "l4t"
    else:
        with open("/etc/lsb-release", "r", encoding="utf-8") as myfile:
            data = myfile.read()
        p = re.compile("DISTRIB_RELEASE=(.*)")
        DISTRIB_RELEASE = p.search(data).group(1).split(".")[0]
        p = re.compile("DISTRIB_ID=(.*)")
        DISTRIB_ID = p.search(data).group(1).lower()
        OS_VERSION = DISTRIB_ID

    zedhub_path = "/usr/local/sl_hub"

    if not os.path.isdir(zedhub_path):
        print("Error: could not find sl_hub installation, please install it from Zed Hub.")
        sys.exit(1)

    check_zed_hub_version()

    whl_platform_str = "linux"


else:
    print ("Unknown system.platform: %s  Installation failed, see setup.py." % sys.platform)
    sys.exit(1)

PYTHON_MAJOR = platform.python_version().split(".")[0]
PYTHON_MINOR = platform.python_version().split(".")[1]

whl_python_version = "-cp" + str(PYTHON_MAJOR) + str(PYTHON_MINOR) + "-cp" + str(PYTHON_MAJOR) + str(PYTHON_MINOR)
if int(PYTHON_MINOR) < 8 :
    whl_python_version += "m"

disp_str = "Detected platform: \n\t " + str(OS_VERSION) + "\n\t Python " + str(PYTHON_MAJOR) + "." + str(PYTHON_MINOR)
if "aarch64" not in ARCH_VERSION and not CUDA_INDEPENDANT_VERSION:
    disp_str += "\n\t ZED IOT " + str(ZED_IOT_MAJOR) + "." + str(ZED_IOT_MINOR)
if "aarch64" not in ARCH_VERSION and not CUDA_INDEPENDANT_VERSION:
    disp_str += " (requiring CUDA " + str(CUDA_MAJOR_ZED) + "." + str(CUDA_MINOR_ZED) + ")"
print(disp_str)

whl_file = "pyzedhub-" + str(ZED_IOT_MAJOR) + "." + str(ZED_IOT_MINOR) + whl_python_version + "-" + whl_platform_str + "_" + str(ARCH_VERSION).lower() + ".whl"
whl_file_URL = base_URL + str(ZED_IOT_MAJOR) + "." + str(ZED_IOT_MINOR) + "/whl/" + OS_VERSION + "/" + whl_file

whl_file_path = os.path.join(dirname, whl_file)

print("-> Checking if " + whl_file_URL + " exists and is available")
r = requests.get(whl_file_URL, allow_redirects=True)
open(whl_file_path, 'wb').write(r.content)
# Warning doesn't handle missing remote file yet and will probably download an html

if check_valid_file(whl_file_path):
    # Internet is ok, file has been downloaded and is valid
    print("-> Found ! Downloading python package into " + whl_file_path)

    print("-> Installing necessary dependencies")
    err = 0
    if "aarch64" in ARCH_VERSION:
        # On jetson numpy is built from source and need other packages
        err = pip_install("wheel")

    if err != 0:
        print("ERROR : An error occurred, 'pip' failed to setup python dependencies packages (pyzedhub was NOT correctly setup)")
        sys.exit(1)

    # Install pyzedhub with its dependencies
    err_pyzed = pip_install(whl_file_path, force_install=True)
    if err_pyzed == 0:
        print("Done")
    else:
        print("ERROR : An error occurred, 'pip' failed to setup pyzed package (pyzedhub was NOT correctly setup)")
        sys.exit(1)

    if sys.platform == "win32" :
        print("Installing OpenGL dependencies required to run the samples")
        py_vers = int(PYTHON_MAJOR) *10 + int(PYTHON_MINOR)
        install_win_dep("PyOpenGL", py_vers)
        install_win_dep("PyOpenGL_accelerate", py_vers)

    print("  To install it later or on a different environment run : \n python -m pip install --ignore-installed "+ whl_file_path)
    sys.exit(0)
else:
    print("\nUnsupported platforms, no pyzedhub file available for this configuration\n Contact us at support@stereolabs.com to get support.")
    sys.exit(1)
