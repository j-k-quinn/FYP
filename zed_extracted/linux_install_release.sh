#!/usr/bin/env bash
REQUIRED_CUDA_VERSION=11.4
TARGET_PLATFORM=Tegra_L4T35.4
STATIC_LIBS_AVAILABLE=1

# If a command fail the script will exit
#set -e -x
SILENT=0
RUNTIME_ONLY=0
STATIC_LIBS=1
JETSON_NPVMODEL_INPUT=

WARNINGS=0

folder="."
installation_folder="/usr/local/zed"
installation_folder_hub="/usr/local/sl_hub"
is_jetson_installer=false

AI_INSTALLED=0
SAMPLES_INSTALLED=0
TOOLS_INSTALLED=0
PYTHON_INSTALLED=0
DOCKER_ENV=0
AI_DISABLED=0
PYTHON_DISABLED=0
SKIP_CUDA=0
SKIP_HUB=0

detected_ubuntu_release=""

dependencies=(
  libjpeg-turbo8
  libturbojpeg
  libusb-1.0-0
  libusb-1.0-0-dev
  libopenblas-dev
  libarchive-dev
  libv4l-0
  curl
  unzip
  zlib1g
  mesa-utils
)

dependencies_dev=(
  libpng-dev
)

dependencies_python=(
  python3-dev
  python3-pip
  python3-setuptools
)

dependencies_tools=(
  qtbase5-dev 
  qtchooser 
  qt5-qmake 
  qtbase5-dev-tools
  libqt5opengl5
  libqt5svg5
)

dependencies_samples=(
  libglew-dev
  freeglut3-dev
)

dependencies_docker=(
  lsb-release 
  wget 
  less 
  udev 
  sudo 
  apt-transport-https
)

# Input arg parsing
usage() {
    echo "USAGE: "
    echo -e "\t-h --help"
    echo -e "\t--silent"
    echo -e "\t--runtime_only"
    echo -e "\t--skip_tools"
    echo -e "\t--skip_cuda"
    echo -e "\t--skip_od_module"
    echo -e "\t--skip_python"
    #echo -e "\t--nvpmodel=0 (Jetson only)"
    echo -e "\t--skip_hub"
    echo ""
}

while [ "$1" != "" ]; do
    PARAM=`echo $1 | awk -F= '{print $1}'`
    VALUE=`echo $1 | awk -F= '{print $2}'`
    case $PARAM in
        -h | --help)
            usage
            exit
            ;;
        --silent | silent | -s)
            SILENT=1
            ;;
        --runtime_only | runtime_only | -rt)
            RUNTIME_ONLY=1
            STATIC_LIBS=O
            ;;
        --skip_tools | skip_tools | -st)
            RUNTIME_ONLY=1
            ;;
        --skip_od_module | skip_od_module | -sod)
            AI_DISABLED=1
            ;;
        --skip_python | skip_python | -spy)
            PYTHON_DISABLED=1
            ;;
        --skip_cuda | skip_cuda | no_cuda | --skip_cuda | -scu)
            SKIP_CUDA=1
            ;;
        --skip_hub | skip_hub | -shub)
            SKIP_HUB=1
            ;;
        #--nvpmodel | nvpmodel | -m)
        #    nvpmodel=$VALUE
        #    ;;
        --static_libs | static_libs | -sl)
            STATIC_LIBS=$VALUE
            ;;
        *)
            echo "WARNING: unknown parameter \"$PARAM\" (ignored)"
            usage
            ;;
    esac
    shift
done

if [ "$STATIC_LIBS_AVAILABLE" == 0 ]; then
    STATIC_LIBS=O
fi


NONE='\x1b[m'
BOLD='\x1b[1m'
RED='\x1b[31m'
ITALIC='\x1b[3m'

display_warning() {
    echo -e "\n${BOLD}${1}${NONE}\n" >&2
}

asksure() {
    if [ $SILENT == 1 ]; then
        return 0
    fi
    read -r -p "$1 [Y/n] ?" response
    response=${response,,} # tolower
    if [[ $response =~ ^(yes|y| ) ]] || [[ -z $response ]]; then
        retval=0
    else
        retval=1
    fi
    return $retval
}

download_fct() {
    url=$1
    dest_file=$2

    if [ $SILENT == 1 ]; then
        wget_options=( -q )
        echo "Downloading '$dest_file'..."
    else
        wget_options=( -q --show-progress --progress=bar:force:noscroll)
    fi

    wget "$url" -O "$dest_file" "${wget_options[@]}"
}

create_swap() {
    SWAPDIRECTORY="/mnt"
    SWAPSIZE=2
    FREE=$(df $PWD | awk '/[0-9]%/{print $(NF-2)}')
    echo "** Free space : $FREE **"
    if [[ $FREE -lt 2000000 ]]; then 
        return 1
    fi

    sudo fallocate -l $SWAPSIZE"G" $SWAPDIRECTORY"/sl_swapfile"
    cd $SWAPDIRECTORY
    # Change permissions so that only root can use it
    sudo chmod 600 sl_swapfile
    sudo mkswap sl_swapfile
    sudo swapon sl_swapfile
    return 0
}

remove_swap() {
    SWAPDIRECTORY="/mnt"
    cd $SWAPDIRECTORY
    echo "Waiting for swap release..."
    sudo swapoff -v sl_swapfile
    sudo rm sl_swapfile
    echo "Swap release done!"
    return 0
}

install_cuda_desktop() {
    if [ "$is_jetson_installer" = true ] ; then
        echo "Incompatible with Jetson installer"
        WARNINGS=$(($WARNINGS + 1))
    else
        # Check if there's an nvidia graphic card https://docs.nvidia.com/cuda/cuda-installation-guide-linux/index.html#verify-you-have-cuda-enabled-system
        nvidia_devices=$(lspci | grep -i nvidia | wc -l)
        if [ "$nvidia_devices" -eq "0" ]; then
            if asksure "No NVIDIA graphics cards found in the system, it is mandatory to run the ZED SDK, Continue anyway"; then
                echo "Installing..."
                WARNINGS=$(($WARNINGS + 1))
            else
                echo "Exiting"
                exit 1
            fi
        fi

        ubuntu_year="$(lsb_release -r | egrep -o [0-9][0-9] | head -1)"
        ubuntu_month="$(lsb_release -r | egrep -o [0-9][0-9] | tail -1)"
        ubuntu_version="${ubuntu_year}${ubuntu_month}"
        cuda_version_array=(${REQUIRED_CUDA_VERSION//./ })
        cuda_major=${cuda_version_array[0]}
        cuda_minor=${cuda_version_array[1]}

        apt_repo=$(find /etc/apt/ -name *.list | xargs cat | grep  ^[[:space:]]*deb | grep developer.download.nvidia.com/compute/cuda/repos/)
        if [[ -z "$apt_repo" ]]; then # Repo not configured
            # https://developer.nvidia.com/cuda-downloads?target_os=Linux&target_arch=x86_64&target_distro=Ubuntu&target_version=1604&target_type=debnetwork
            # https://forums.developer.nvidia.com/t/notice-cuda-linux-repository-key-rotation/212772
            sudo apt-key del 7fa2af80
            wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu${ubuntu_version}/x86_64/cuda-keyring_1.0-1_all.deb
            sudo dpkg -i cuda-keyring_1.0-1_all.deb
            rm cuda-keyring_1.0-1_all.deb
        fi
        sudo apt-get update
        sudo apt-get -y install cuda-${cuda_major}-${cuda_minor}
    fi
}

check_install_hub() {
    if asksure "Do you want to install ZED Hub module ?" ; then
        SKIP_HUB=0
    else
        SKIP_HUB=1
    fi
}

check_cuda_version() {
    found_cuda_version=
    # Check only if the variable has been set
    if [ -n "$REQUIRED_CUDA_VERSION" ]; then
        echo "Checking CUDA version..."
        
        # Check for cuda compiler first
        nvcc_output=""
        if ! [ -x "$(command -v nvcc)" ]; then # nvcc not found
            if [ -x "$(command -v /usr/local/cuda/bin/nvcc)" ]; then # /usr/local/cuda/bin/nvcc found
                nvcc_output="$(/usr/local/cuda/bin/nvcc --version 2>/dev/null)"
            fi
        else # nvcc found
            nvcc_output="$(nvcc --version 2>/dev/null)"
        fi

        if [ -z "$nvcc_output" ]; then # nvcc_output empty
            CUDA_VERSION_FILE=/usr/local/cuda/version.txt
            if [ -f "$CUDA_VERSION_FILE" ]; then
                found_cuda_version="$(cat /usr/local/cuda/version.txt | egrep -o [0-9]+.[0-9]+ | head -1)"
            fi
        else
            # Extract version from output
            found_cuda_version="$(echo $nvcc_output | egrep -o 'V[0-9]+.[0-9]')"
            found_cuda_version=${found_cuda_version:1} # cut the 'V'
        fi

        if [ -z "$found_cuda_version" ]; then # nvcc_output empty
            display_warning "ERROR : the installer failed to detect the CUDA version (CUDA $REQUIRED_CUDA_VERSION required), make sure it's installed"
            
            if [ "$is_jetson_installer" = false ] ; then
                if asksure "Do you want to install CUDA $REQUIRED_CUDA_VERSION automatically" ; then
                    install_cuda_desktop
                else
                    if asksure "The ZED SDK will NOT work without CUDA, Continue "; then
                        echo "Installing..."
                        WARNINGS=$(($WARNINGS + 1))
                    else
                        echo "Exiting"
                        exit 1
                    fi
                fi
            fi
        else
            cuda_version_required_array=(${REQUIRED_CUDA_VERSION//./ })
            cuda_major_required=${cuda_version_required_array[0]}
            cuda_minor_required=${cuda_version_required_array[1]}

            cuda_version_detected_array=(${found_cuda_version//./ })
            cuda_major_detected=${cuda_version_detected_array[0]}
            cuda_minor_detected=${cuda_version_detected_array[1]}

            must_install_cuda="true"
            # CUDA 11 is compatible for all minor version
            if [ "$cuda_major_detected" == "$cuda_major_required" ]; then
                if [ "$cuda_major_detected" -ge 11 ]; then
                    must_install_cuda="false"
                elif [ "$cuda_minor_detected" -eq "$cuda_minor_required" ]; then
                    must_install_cuda="false"
                fi
            fi

            # Test the version number
            if [ "$must_install_cuda" = "true" ]; then
                display_warning "ERROR : Found CUDA $found_cuda_version but the ZED SDK requires $REQUIRED_CUDA_VERSION"
                if asksure "Do you want to install CUDA $REQUIRED_CUDA_VERSION automatically" ; then
                    install_cuda_desktop
                else
                    if asksure "The ZED will NOT work with this CUDA version ($found_cuda_version), Continue "; then
                        echo "Installing..."
                        WARNINGS=$(($WARNINGS + 1))
                    else
                        echo "Exiting"
                        exit 1
                    fi
                fi
            else
                echo "OK: Found CUDA $found_cuda_version"
            fi
        fi
    fi

    if [ "$is_jetson_installer" = false ] ; then
        # Checking nvidia driver

        # Check if nvidia-smi is there
        if ! command -v nvidia-smi &> /dev/null
        then
            echo "nvidia-smi could not be found, skipping nvidia driver check"
            WARNINGS=$(($WARNINGS + 1))
        else
            # Check that it's working
            out=$(nvidia-smi)
            need_reboot_str="NVML: Driver/library version mismatch"
            if [[ "$out" == *"$need_reboot_str"* ]]; then
                echo "Driver recently setup, reboot your computer to use CUDA"
                WARNINGS=$(($WARNINGS + 1))
            else
                # Normal operation
                out=$(nvidia-smi -q -u -x)
                cuda_version_runtime=$(echo $out | grep "cuda_version" | sed -e 's/.*<cuda_version>\(.*\)<\/cuda_version>.*/\1/')
                nvidia_driver_version=$(echo $out | grep "driver_version" | sed -e 's/.*<driver_version>\(.*\)<\/driver_version>.*/\1/')

                cuda_version_runtime_arr=($(echo $cuda_version_runtime | grep -o -E '[0-9]+'))
                cuda_version_runtime_major=${cuda_version_runtime_arr[0]}
                cuda_version_runtime_minor=${cuda_version_runtime_arr[1]}

                nvidia_driver_version_arr=($(echo $nvidia_driver_version | grep -o -E '[0-9]+'))
                nvidia_driver_version_major=${nvidia_driver_version_arr[0]}
                nvidia_driver_version_minor=${nvidia_driver_version_arr[1]}

                required_cuda_version_array=(${REQUIRED_CUDA_VERSION//./ })
                required_cuda_major=${required_cuda_version_array[0]}
                required_cuda_minor=${required_cuda_version_array[1]}

                check_driver="true"
                if [ "$cuda_major_detected" == "$cuda_major_required" ]; then
                    if [ "$cuda_major_detected" -ge 11 ]; then
                        check_driver="false"
                    elif [ "$cuda_minor_detected" -eq "$cuda_minor_required" ]; then
                        check_driver="false"
                    fi
                fi

                if [ "$check_driver" = "true" ]; then
                    if [ $cuda_version_runtime_major -lt $required_cuda_major ] || ( [ $cuda_version_runtime_major -eq $required_cuda_major ] && [ $cuda_version_runtime_minor -lt $required_cuda_minor ] ); then
                        echo "WARNING : Detected Nvidia driver $nvidia_driver_version_major.$nvidia_driver_version_minor may be too old to run CUDA $required_cuda_major.$required_cuda_minor (limited to CUDA $cuda_version_runtime_major.$cuda_version_runtime_minor)"
                        WARNINGS=$(($WARNINGS + 1))
                        if asksure "CUDA applications (including the ZED SDK) may not work with this Nvidia driver, Continue "; then
                            echo "Installing..."
                            WARNINGS=$(($WARNINGS + 1))
                        else
                            echo "Exiting"
                            exit 1
                        fi
                    else
                        echo "Detected Nvidia driver $nvidia_driver_version_major.$nvidia_driver_version_minor, capable of running CUDA $cuda_version_runtime_major.$cuda_version_runtime_minor (need $required_cuda_major.$required_cuda_minor)"
                    fi
                fi
            fi
        fi

    fi

}

check_platform() {
    # Determine OS platform https://askubuntu.com/a/459425/609717
    UNAME=$(uname | tr "[:upper:]" "[:lower:]")
    # If Linux, try to determine specific distribution
    if [ "$UNAME" == "linux" ]; then
        # If available, use LSB to identify distribution
        if [ -f /etc/lsb-release -o -d /etc/lsb-release.d ]; then
            DISTRO=$(lsb_release -i | cut -d: -f2 | sed s/'^\t'//)
            # Otherwise, use release info file
        else
            DISTRO=$(ls -d /etc/[A-Za-z]*[_-][rv]e[lr]* | grep -v "lsb" | cut -d'/' -f3 | cut -d'-' -f1 | cut -d'_' -f1)
        fi
    else # Check if 32bits or non Linux system
        display_warning "This ZED SDK installer is only compatible with Linux 64bits"
        exit 1
    fi

    # Check Jetson
    detected_arch=$(uname -m)
    if [ "$detected_arch" == "armv7l" ]; then
        echo "*******************************************************************************"
        echo "** WARNING ** : This version does not support the jetson TK1 **"
        echo "*******************************************************************************"
        exit 1
    fi

    if [ "$detected_arch" != "x86_64" ]; then
        is_jetson_installer=true;
    fi
    
    detected_ubuntu_release=$(lsb_release -r | egrep -o [0-9][0-9].[0-9][0-9])

    if [ "$DISTRO" != "Ubuntu" ]; then
        if asksure "WARNING : This ZED SDK installer is only compatible with Ubuntu LTS, other version of Linux might work but without any guarantee or support, Continue "; then
            echo "Installing..."
        else
            echo "Exiting"
            exit 1
        fi
    elif [ "$is_jetson_installer" = true ]; then # JetPack/L4T detection
        # Get the jetpack version https://github.com/dusty-nv/jetson-inference/blob/7e81381a96c1ac5f57f1728afbfdec7f1bfeffc2/tools/install-pytorch.sh#L296
        JETSON_L4T_STRING=$(head -n 1 /etc/nv_tegra_release)
        if [[ $JETSON_L4T_STRING == *"release"* ]]; then
            JETSON_L4T_RELEASE=$(echo $JETSON_L4T_STRING | cut -f 2 -d ' ' | grep -Po '(?<=R)[^;]+')
            JETSON_L4T_REVISION=$(echo $JETSON_L4T_STRING | cut -f 2 -d ',' | grep -Po '(?<=REVISION: )[^;]+' | cut -f 1 -d '.')
        else
            JETSON_L4T_STRING=$(dpkg-query --showformat='${Version}' --show nvidia-l4t-core)
            JETSON_L4T_ARRAY=$(echo $JETSON_L4T_STRING | cut -f 1 -d '-')
            JETSON_L4T_RELEASE=$(echo $JETSON_L4T_ARRAY | cut -f 1 -d '.')
            JETSON_L4T_REVISION=$(echo $JETSON_L4T_ARRAY | cut -f 2 -d '.')
        fi

        if [[ "$TARGET_PLATFORM" == *"JP"* ]]; then
            if [ "$JETSON_L4T_RELEASE" == "32" ]; then
                JETSON_JETPACK_MAJOR_DETECTED="4"
            fi
            JETSON_JETPACK_MINOR_DETECTED="$JETSON_L4T_REVISION"
            if [ "$TARGET_PLATFORM" == "Tegra_JP${JETSON_JETPACK_MAJOR_DETECTED}${JETSON_JETPACK_MINOR_DETECTED}" ]; then
                echo "JetPack version ${JETSON_JETPACK_MAJOR_DETECTED}${JETSON_JETPACK_MINOR_DETECTED} detected. OK"
            else
                echo "Detected Tegra_JP${JETSON_JETPACK_MAJOR_DETECTED}${JETSON_JETPACK_MINOR_DETECTED}, required exact $TARGET_PLATFORM"

                if asksure "WARNING : possibly unsupported JetPack version, Continue "; then
                    echo "Installing..."
                else
                    echo "Exiting"
                    exit 1
                fi
            fi
        else
            if [ "$TARGET_PLATFORM" == "Tegra_L4T${JETSON_L4T_RELEASE}.${JETSON_L4T_REVISION}" ]; then
                echo "L4T version ${JETSON_L4T_RELEASE}.${JETSON_L4T_REVISION} detected. OK"
            else
                echo "Detected Tegra_L4T${JETSON_L4T_RELEASE}.${JETSON_L4T_REVISION}, required exact $TARGET_PLATFORM"

                if asksure "WARNING : possibly unsupported Linux 4 Tegra version, Continue "; then
                    echo "Installing..."
                else
                    echo "Exiting"
                    exit 1
                fi
            fi
        fi
    else
        ubuntu_version="$(lsb_release -r | egrep -o [0-9][0-9] | head -1)"

        detected_platform="${DISTRO}${ubuntu_version}"
        if [ "$TARGET_PLATFORM" == "$detected_platform" ]; then
            echo "Ubuntu version $detected_ubuntu_release detected. OK"
        else
            echo "Detected $detected_platform, required exact $TARGET_PLATFORM"

            if asksure "WARNING : possibly unsupported OS version, Continue "; then
                echo "Installing..."
            else
                echo "Exiting"
                exit 1
            fi
        fi
    fi

    

    # Regardless of the platform, add specific packages
    if [ "$detected_ubuntu_release" == "20.04" ]; then
        dependencies_dev+=('libturbojpeg0-dev')
    fi
    if [ "$detected_ubuntu_release" == "18.04" ]; then
        dependencies+=('libpng16-16')
        dependencies_dev+=('libturbojpeg0-dev')
    fi
    if [ "$detected_ubuntu_release" == "16.04" ]; then
        dependencies+=('libpng12-0')
        dependencies_dev+=('libjpeg-turbo8-dev')
    fi

    # Check if it's the good arch for the system by checking the folder name (if the ${uname -m} match the system)
    uname_platform="$(uname -m)"
    file_platform="$(file ${folder}/lib/libsl_zed.so)"
    if [[ "${file_platform//_/-}" != *"${uname_platform//_/-}"* ]]; then
        case ${uname_platform} in
            "i686")
                display_warning "The ZED SDK is not compatible with Linux 32 bits"
            exit 1 ;;
            "aarch64")
                display_warning "This installer is not compatible with ARM processor, for the Jetson Tegra check the special installers available at stereolabs.com/developers"
            exit 1 ;;
            "x86-64")
                display_warning "This installer is not compatible with x86_64 processor, for the Desktop check the installer available at stereolabs.com/developers"
            exit 1 ;;
        esac
    fi

    if [[ $EUID -ne 0 ]]; then
        DOCKER_ENV=0
    else
        # We're assuming 'root' user means 'inside docker', not sudo
        home_path=$HOME
        if [[ ! $home_path == *"/home/"* ]]; then
            DOCKER_ENV=1
        fi
    fi
}

enable_maxperf_jetson() {
    # This script is useless in a docker image
    if [ $DOCKER_ENV == 0 ]; then
        if [ "$is_jetson_installer" = true ] ; then

            # Remove old script installed by previous versions
            sudo rm "/etc/init.d/tegra_maxperf" -f
            sudo rm "/run/systemd/generator.late/tegra_maxperf.service" -f
            sudo rm "/usr/local/zed/tools/jetson_clocks" -f
            service_enabled=$(systemctl list-unit-files --state=enabled | grep -c jetson_clocks)
            if [ $service_enabled -eq 1 ] ; then
                sudo systemctl disable jetson_clocks.service
                sudo rm "/etc/systemd/system/jetson_clocks.service" -f
            fi
            
            nvpmodel_mode=0
            nvpmodel_model_input_set=0

            # MAX N nvpmodel
            nvpmodel_maxn_mode=0
            if [ -e "/proc/device-tree/compatible" ]; then
                if [ -e "/proc/device-tree/model" ]; then
                    machine="$(tr -d '\0' < /proc/device-tree/model)"
                fi
                if [[ $machine == *"Xavier NX"* ]]; then
                    nvpmodel_maxn_mode=2
                fi
            fi

            if ! [ -z "$1" ] && [[ "$1" =~ [0-9] ]] ; then
                # Valid input nvpmodel
                nvpmodel_mode=$1
                nvpmodel_model_input_set=1
            else
                # default max perf 
                nvpmodel_mode=$nvpmodel_maxn_mode
            fi

            if [ $nvpmodel_model_input_set -eq 0 ] ; then
                if asksure "Do you want to enable maximum performance mode (recommended)? It provides optimal performance but increases power draw. "; then
                    echo "$machine detected"
                    sudo nvpmodel -m $nvpmodel_mode
                    if [ "${nvpmodel_mode}" -eq "${nvpmodel_maxn_mode}" ]; then 
                        sudo jetson_clocks
                    fi
                    sudo nvpmodel -q
                fi
            else
                sudo nvpmodel -m $nvpmodel_mode
            fi
        fi
    fi
}


check_packages() {
    #https://askubuntu.com/a/995110
    local apt_options="$1"
    shift
    local packages=("$@") # Rebuild the array with rest of arguments

    run_install() {
        ## Prompt the user 
        if asksure "Do you want to auto-install dependencies (recommended) ? following packet will be installed via the package manager : ${packages[*]}"; then
            if [ "$USER" = "root" ]; then
                # Probably docker: we may need to update the pkg list (without sudo)
                apt-get update
            fi
            sudo apt-get install -y --no-install-recommends "${apt_options}" ${packages[@]}
        fi
    }

    dpkg -s "${packages[@]}" >/dev/null 2>&1 || run_install
}

# Check if AI module is available for this platform
AI_LIBRARY_INSTALLER=${folder}/lib/libsl_ai.so
if [ ! -f "$AI_LIBRARY_INSTALLER" ]; then
    AI_DISABLED=1
fi

# Make sure we're not root
if [ "$(id -u)" == "0" ]; then
    if [ $SILENT != 1 ]; then
        display_warning "This script shouldn't be run as root, the root permission will be asked when needed (to install the dependencies, and the SDK into /usr/local/zed)"
        exit 1
    else # Install as root anyway
        USER=root
        # Probably docker environment : install the missing packages # check_packages "" "${dependencies_docker[@]}"
        # apt-get update && apt-get install --no-install-recommends lsb-release wget less udev sudo apt-transport-https -y
        check_packages "" "${dependencies_docker[@]}"
    fi
fi

command -v sudo >/dev/null 2>&1 || { "apt-get" install -y --no-install-recommends "sudo";}
command -v less >/dev/null 2>&1 || { sudo "apt-get" install -y --no-install-recommends "less";}
command -v file >/dev/null 2>&1 || { sudo "apt-get" install -y --no-install-recommends "file";}

# Check Jetson system
check_platform
    
# Display EULA with confirmation asking
if [ $SILENT == 0 ]; then
    less doc/license/LICENSE.txt
    if asksure "To continue you have to accept the EULA. Accept "; then
        echo "Installing..."
    else
        echo "Exiting"
        exit 1
    fi
fi

echo "Installation path: ${installation_folder}"

# Get resources and conf files from previous installation if present
installation_folder_old="/tmp/zed_previous"
if [ -d "${installation_folder}" ]; then
    if [ -d "${installation_folder_old}" ]; then
        sudo rm -fr "${installation_folder_old}"
    fi

    mkdir -p "${installation_folder_old}"

    cp -R "${installation_folder}/settings" "${installation_folder_old}" 2>/dev/null || :
    cp -R "${installation_folder}/resources" "${installation_folder_old}" 2>/dev/null || :

    sudo rm -rf "${installation_folder}" 2>/dev/null || :
fi
sudo mkdir -p "${installation_folder}"
sudo chown "${USER}" -R "${installation_folder}"
if [ -d "${installation_folder_old}" ]; then
    cp -r "${installation_folder_old}/settings" "${installation_folder}/" 2>/dev/null || :
    cp -r "${installation_folder_old}/resources" "${installation_folder}/" 2>/dev/null || :
    sudo rm -fr "${installation_folder_old}"
fi

# Check CUDA version and display the appropriate message
if [ $SKIP_CUDA == 0 ]; then
    check_cuda_version
else
    echo "Skipping CUDA version check"
fi

# Install ZED Hub ?
if [ $SKIP_HUB == 0 ]; then
    check_install_hub
else
    echo "Skipping Hub installation"
fi

# Install tools
if [ $RUNTIME_ONLY == 0 ]; then
    cp -r "${folder}/tools" "${installation_folder}/"
    
    # Creating symlinks
    tools_exe=$(find "${installation_folder}/tools/" -executable -type f)
    tools_exe_arr=($tools_exe)
    for tool_exe in "${tools_exe_arr[@]}"; do
        sudo ln -s "$tool_exe" "/usr/local/bin/$(basename $tool_exe)" > /dev/null 2>&1
    done

    TOOLS_INSTALLED=1
fi

if [ $STATIC_LIBS_AVAILABLE == 1 ]; then
    if asksure "Do you want to also install the static version of the ZED SDK (AI module will still require libsl_ai.so)"; then
        STATIC_LIBS=1
    else
        STATIC_LIBS=0
    fi
fi

## AI Module
if [ $AI_DISABLED == 0 ] ; then
    if [ "$is_jetson_installer" = true ] ; then 
        ai_module_text="Do you want to install the AI module (required for Object detection and Neural Depth, recommended)"
    else
        # Look into the libs
        ## TRT embedded libs version check
        #trt_version_raw=$(nm -D "${folder}/dependencies/ai_dependencies/lib/libnvinfer.so" | grep tensorrt_version)
        #trt_version_arr_raw=($trt_version_raw)
        #trt_version_raw2=$(echo ${trt_version_arr_raw[2]} | grep -o -E '[0-9]+')
        #trt_version_arr=($trt_version_raw2)
        #trt_version="${trt_version_arr[0]}.${trt_version_arr[1]}"
        ## cuDNN embedded libs version check, works only for v7
        #cudnn_version_raw=$(strings "${folder}/dependencies/ai_dependencies/lib/libcudnn.so" | grep cudnn_version | grep -o -E '[0-9]+')
        #cudnn_version_arr=($cudnn_version_raw)
        #cudnn_version="${cudnn_version_arr[0]}.${cudnn_version_arr[1]}"
        
        # Versions from filename
        nvinfer_lib=$(ls "${folder}/dependencies/ai_dependencies/lib/libnvinfer.so".*.*.*)
        trt_version_arr=($(echo $(basename -- "$nvinfer_lib") | grep -o -E '[0-9]+'))
        trt_version="${trt_version_arr[0]}.${trt_version_arr[1]}"

        cudnn_lib=$(ls "${folder}/dependencies/ai_dependencies/lib/libcudnn.so".*.*.*)
        cudnn_version_arr=($(echo $(basename -- "$cudnn_lib") | grep -o -E '[0-9]+'))
        cudnn_version="${cudnn_version_arr[0]}.${cudnn_version_arr[1]}"

        ai_module_text="Do you want to install the AI module (required for Object detection and Neural Depth, recommended), cuDNN ${cudnn_version} and TensorRT ${trt_version} will be installed"
    fi
    if asksure "${ai_module_text}"; then
        mkdir -p "${installation_folder}/resources"
        cp -r "${folder}/resources/"* "${installation_folder}/resources/" 2>/dev/null
        AI_INSTALLED=1
    fi
fi

## sl_hub
if [ $SKIP_HUB == 0 ]; then
    sudo mkdir -p "${installation_folder_hub}"
    sudo chown "${USER}" -R "${installation_folder_hub}"
    cp -r "${folder}/sl_hub/installer/sl_hub/." "${installation_folder_hub}"
fi

# Enable MaxN jetson
#enable_maxperf_jetson "$JETSON_NPVMODEL_INPUT"

mkdir -p "${installation_folder}/lib/"
mkdir -p "${installation_folder}/include"

# Install lib_deps
if [ "$is_jetson_installer" = true ] ; then 
    # For Jetson JP43
    v4l_symlink="/usr/lib/aarch64-linux-gnu/libv4l2.so"
    if [ ! -f "$v4l_symlink" ]; then
        sudo ln -s "${v4l_symlink}.0" ${v4l_symlink}
    fi

    # Rename stub folder to avoid issues with the tools on L4T 35.1
    sudo mv /usr/local/cuda-11.4/targets/aarch64-linux/lib/stubs/ /usr/local/cuda-11.4/targets/aarch64-linux/lib/stubs_ 2>/dev/null || true 
fi
# Install lib
cp -r "${folder}/lib/"*.so* "${installation_folder}/lib/"
if [ $STATIC_LIBS == 1 ]; then
    cp -r "${folder}/lib/"*.a* "${installation_folder}/lib/"
fi

#if [ $RUNTIME_ONLY == 0 ]; then
    # Install include
    cp -r "${folder}/include/"* "${installation_folder}/include/"
#fi

# Cmake find package
cp -r "${folder}/zed-config"* "${installation_folder}/"

# get_python_api.py
cp -r "${folder}/get_python_api.py" "${installation_folder}/"

mkdir -p "${installation_folder}/settings"

# Install samples
if [ $RUNTIME_ONLY == 0 ]; then
    if asksure "Install samples (recommended)"; then
        if [ $SILENT != 1 ]; then
            read -e -p "Installation path: " -i "${installation_folder}/samples/" samplePath
        else
            samplePath="${installation_folder}/samples/"
        fi
        mkdir -p "${samplePath}"
        cp -r "${folder}/samples/"* "${samplePath}/"
        SAMPLES_INSTALLED=1
    fi

    # Settings folder with granted access for the user (to update calib file)
    mkdir -p "${installation_folder}/firmware"
    cp -R "${folder}/firmware/"* "${installation_folder}/firmware/"

    # Doc folder with granted access for the user
    cp -r "${folder}/doc"* "${installation_folder}/"
fi

# Install dependencies
if [ $RUNTIME_ONLY == 0 ] || [ $SAMPLES_INSTALLED == 1 ]; then dependencies=("${dependencies[@]}" "${dependencies_dev[@]}"); fi
if [ $PYTHON_DISABLED == 0 ]; then dependencies=("${dependencies[@]}" "${dependencies_python[@]}"); fi
if [ $SAMPLES_INSTALLED == 1 ]; then dependencies=("${dependencies[@]}" "${dependencies_samples[@]}"); fi
if [ $TOOLS_INSTALLED == 1 ]; then dependencies=("${dependencies[@]}" "${dependencies_tools[@]}"); fi

# If AI is enabled we install those dependencies regardless of the apt dependencies
# CUDNN + nvinfer
if [ "$is_jetson_installer" = false ] && [ $AI_INSTALLED == 1 ]; then
    sudo cp -Rf "${folder}/dependencies/ai_dependencies/lib/"*.so* "/usr/local/cuda/lib64/"
    #if [ $STATIC_LIBS == 1 ]; then
    #    sudo cp -Rf "${folder}/dependencies/ai_dependencies_static/lib/"*.a* "/usr/local/cuda/lib64/"
    #fi
    if [ $RUNTIME_ONLY == 0 ]; then
        sudo cp -Rf "${folder}/dependencies/ai_dependencies/include/"* "/usr/local/cuda/include/"
        sudo chmod a+r -R "/usr/local/cuda/include/"
    fi

    if [ $SKIP_CUDA == 0 ]; then
        # https://community.stereolabs.com/t/installer-causes-docker-layer-to-duplicate-cuda-libraries-even-with-skip-cuda/2665
        # Ensure the libs are accessible if cuda is installed by script
        sudo chmod a+rx -R "/usr/local/cuda/lib64/"*
    fi

    # Symlink for nvinfer lib (for TRT 8.4.1 and newer)
    nvinfer_lib=$(ls "${folder}/dependencies/ai_dependencies/lib/libnvinfer.so".*.*.*)
    trt_version_arr=($(echo $(basename -- "$nvinfer_lib") | grep -o -E '[0-9]+'))
    trt_version="${trt_version_arr[0]}.${trt_version_arr[1]}.${trt_version_arr[2]}"
    libnvinfer_builder_path=$(ls /usr/local/cuda/lib64/libnvinfer_builder_resource.so.${trt_version}*)
    libnvinfer_builder_name="$(basename -- $libnvinfer_builder_path)"
    # if file exists and symlink doesn't, create it
    if [ -f "$libnvinfer_builder_path" ] && [ ! -f "/usr/lib/x86_64-linux-gnu/${libnvinfer_builder_name}" ]; then
        sudo ln -s "${libnvinfer_builder_path}" /usr/lib/x86_64-linux-gnu/
    fi
fi

#if asksure "Do you want to auto-install dependencies (recommended) ? following packet will be installed via the package manager : ${dependencies[*]}"; then
if [ $SILENT != 1 ]; then
    #sudo "apt-get" install -y --no-install-recommends "${dependencies[@]}"
    check_packages "" "${dependencies[@]}"
    if [ $? -eq 0 ]; then
        echo "Dependencies installation complete"
    else
        display_warning "WARNING! Dependencies installation failed, the installer requires an internet connection" 
        WARNINGS=$(($WARNINGS + 1))
    fi
else
    #sudo "apt-get" install --yes --force-yes --no-install-recommends "${dependencies[@]}"
    check_packages "--force-yes" "${dependencies[@]}"
fi
#fi


## Python API
if [ $PYTHON_DISABLED == 0 ] ; then
    if asksure "Do you want to install the Python API (recommended)"; then
        default_python="python3"
        if [ $SILENT != 1 ]; then
            read -e -p "Please specify your python executable: " -i "${default_python}" python3_exe
        fi
        if [ -z "$python3_exe" ];then
            python3_exe=${default_python}
        fi
        ${python3_exe} -m pip install numpy #opencv-python pyopengl
        if ${python3_exe} "${installation_folder}/get_python_api.py" ; then
            PYTHON_INSTALLED=1
        else
            PYTHON_INSTALLED=0
        fi

        if [ $SKIP_HUB == 0 ]; then
            if ${python3_exe} "${installation_folder_hub}/get_python_api.py" ; then
                PYTHON_IOT_INSTALLED=1
            else
                PYTHON_IOT_INSTALLED=0
            fi
        else
            PYTHON_IOT_INSTALLED=2
        fi

        if [[ ${PYTHON_INSTALLED} == 1 ]] && [[ ${PYTHON_IOT_INSTALLED} != 0 ]]; then
            echo "The ZED Python API was installed for '${python3_exe}'"
        elif [[ ${PYTHON_INSTALLED} == 1 ]] && [[ ${PYTHON_IOT_INSTALLED} == 0 ]]; then
            display_warning "WARNING! Python API is only partially installed. ZED Hub module installation failed." 
            WARNINGS=$(($WARNINGS + 1))
        else
            display_warning "WARNING! Python API failed to install" 
            echo "The ZED Python API needs to be resetup to be available (using 'python /usr/local/zed/get_python_api.py' and 'python /usr/local/sl_hub/get_python_api.py')" 
            WARNINGS=$(($WARNINGS + 1))
        fi
    fi
fi

if cmp --silent -- "${folder}/99-slabs.rules" "/etc/udev/rules.d/99-slabs.rules"; then
    :
else
    # Copy udev rules for HID
    sudo cp -r "${folder}/99-slabs.rules" "/etc/udev/rules.d/"
    sudo chmod 644 "/etc/udev/rules.d/99-slabs.rules"
    sudo udevadm control --reload-rules && sudo udevadm trigger
fi

# Update ld_library
sudo cp -r "${folder}/zed.conf" "/etc/ld.so.conf.d/"
sudo ldconfig

# Install Daemon service
if [ "$is_jetson_installer" = true ] ; then
## modify argus daemon to avoid timeout when using multiple cameras
sudo sed -i '/^\[Service\]$/ a Environment="enableCamInfiniteTimeout=1"' /etc/systemd/system/nvargus-daemon.service

## Copy/apply the zed-x daemon
##sudo mkdir "/usr/local/zed/bin/"
##sudo cp "${folder}/bin/ZEDX_Daemon" "/usr/local/zed/bin/"
##sudo cp -r "${folder}/zed_x_daemon.service" "/etc/systemd/system/"
##sudo systemctl enable zed_x_daemon.service     

# Install GMSL drivers
mkdir -p "${installation_folder}/drivers"
cp -R "${folder}/drivers/"* "${installation_folder}/drivers/"
fi

# Add user to video group see https://github.com/stereolabs/zed-ros-wrapper/issues/82#issuecomment-280826440
sudo usermod -a -G video "$(whoami)"

if [ $AI_INSTALLED == 1 ] && [ $TOOLS_INSTALLED == 1 ]; then
    if [ $DOCKER_ENV == 0 ]; then
        # If jetson and the RAM is <= 4GB : need to create swap file to optimize the AI model, otherwise handled at runtime
        system_memory_gb=$(expr $(cat /proc/meminfo | grep MemTotal | egrep -o [0-9]+) / 1000000)
        system_swap_memory_gb=$(expr $(cat /proc/meminfo | grep SwapTotal | egrep -o [0-9]+) / 1000000)
        # We need at least 2G of swap
        if [ "$is_jetson_installer" = true ] && [ $system_memory_gb -le 4 ] && [ $system_swap_memory_gb -lt 2 ]; then
            if create_swap;  then
                # Download optimized AI model
                /usr/local/zed/tools/ZED_Diagnostic -aid
                remove_swap
            else
                echo "Insufficient storage available to create swap file. Make sure you have at least 2Gb space left on your device."
                WARNINGS=$(($WARNINGS + 1))
            fi
        fi
    fi
fi

if [ $AI_INSTALLED == 1 ] && [ $TOOLS_INSTALLED == 1 ] && [ $SILENT != 1 ]; then
    if asksure "Do you want to run the ZED Diagnostic to download all AI models"; then
        /usr/local/zed/tools/ZED_Diagnostic -aid
        if asksure "Do you want to run the ZED Diagnostic to optimize all AI models, it may take a very long time, up to multiple hours but will be done only once. Otherwise it will be optimized just in time when running the ZED SDK"; then
            /usr/local/zed/tools/ZED_Diagnostic -aio
        fi
    fi
fi


echo -e "${NONE}"

if [ $SILENT != 1 ] && [ "$is_jetson_installer" = true ] ; then
    echo "For GMSL support, please navigate to https://www.stereolabs.com/docs/get-started-with-zed-x/ where you will find compatible drivers and installation instructions."
    display_warning "Please note that installing the 'libv4l-dev' apt package at any point on jetson will break the hardware encoding/decoding support"
fi

if [ $WARNINGS -eq 0 ]; then
    echo "ZED SDK installation complete !"
else
    display_warning "ZED SDK installation complete, with $WARNINGS warning(s)"
fi
