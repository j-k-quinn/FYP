include .env
export $(shell sed 's/=.*//' .env)

build:
	DOCKER_BUILDKIT=1 docker build \
		-t prl_p3at_ros_driver:latest \
		.

run:
	@docker stop p3at_ros_driver || true && docker rm p3at_ros_driver || true
	@sudo chmod 666 ${P3AT_USB_PORT}
	if [ -n "$$DISPLAY" ]; then xhost +si:localuser:root > /dev/null; fi
	docker run \
		--gpus all \
		--runtime=nvidia \
		-e NVIDIA_VISIBLE_DEVICES=all \
		-e NVIDIA_DRIVER_CAPABILITIES=all \
		-e DISPLAY=${DISPLAY} \
		-e QT_X11_NO_MITSHM=1 \
		-e P3AT_USB_PORT=${P3AT_USB_PORT} \
		-v /dev:/dev \
		-v ./rplidar_ros:/root/ros_ws/src/rplidar_ros \
		-v ./full_system_pkg/full_system:/root/ros_ws/src/full_system \
		-v ./p3dx_description:/root/ros_ws/src/pioneer_p3dx_model/p3dx_description \
		-v ./scripts:/root/ros_ws/src/scripts \
		-v ./zed-yolo-ros:/root/ros_ws/src/zed-yolo-ros \
		--privileged \
		--network host \
		--name p3at_ros_driver \
		-it \
		prl_p3at_ros_driver:latest
