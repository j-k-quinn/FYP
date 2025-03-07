include .env
export $(shell sed 's/=.*//' .env)

build-base:
	DOCKER_BUILDKIT=1 docker build -t my-base-image -f Dockerfile.base .

build: 
#	@cp /etc/nv_tegra_release .nv_tegra_release
	DOCKER_BUILDKIT=1 docker build \
		-t prl_p3at_ros_driver:latest \
		-f Dockerfile .
#	@rm -Rf .nv_tegra_release

run:
	@docker stop p3at_ros_driver || true && docker rm p3at_ros_driver || true
	@sudo chmod 666 ${P3AT_USB_PORT}
	xhost +si:localuser:root > /dev/null
	docker run \
		--gpus all \
		--runtime=nvidia \
		-it \
		-e NVIDIA_VISIBLE_DEVICES=all \
		-e NVIDIA_DRIVER_CAPABILITIES=all \
		-e "DISPLAY" \
		-e "QT_X11_NO_MITSHM=1" \
		-e "XAUTHORITY=${XAUTH}" \
		-e ROS_MASTER_URI \
		-e ROS_IP \
		-e DISPLAY=${DISPLAY} \
 		-e QT_X11_NO_MITSHM=1 \
 		-e P3AT_USB_PORT=${P3AT_USB_PORT} \
		-v /dev:/dev \
		-v ./rplidar_ros:/root/ros_ws/src/rplidar_ros \
		-v ./p3dx_description:/root/ros_ws/src/pioneer_p3dx_model/p3dx_description \
		-v ~/.Xauthority:/root/.Xauthority:rw \
		-v "/tmp/.X11-unix:/tmp/.X11-unix:rw" \
		--network host \
		--privileged \
		--name p3at_ros_driver \
		--runtime=nvidia \
		prl_p3at_ros_driver:latest

push:
	docker push personalroboticsimperial/prl:jetson2004-cuda114-noetic-zed40


# run:
# 	@docker stop p3at_ros_driver || true && docker rm p3at_ros_driver || true
# 	@sudo chmod 666 ${P3AT_USB_PORT}
# 	xhost +si:localuser:root > /dev/null
# 	docker run \
# 		--gpus all \
# 		--runtime=nvidia \
# 		-e NVIDIA_VISIBLE_DEVICES=all \
# 		-e NVIDIA_DRIVER_CAPABILITIES=all \
# 		-e DISPLAY=${DISPLAY} \
# 		-e QT_X11_NO_MITSHM=1 \
# 		-e P3AT_USB_PORT=${P3AT_USB_PORT} \
# 		-v /dev:/dev \
# 		-v ./rplidar_ros:/root/ros_ws/src/rplidar_ros \
# 		-v ./p3dx_description:/root/ros_ws/src/pioneer_p3dx_model/p3dx_description \
# 		--privileged \
# 		--network host \
# 		--name p3at_ros_driver \
# 		-it \
# 		prl_p3at_ros_driver:latest
