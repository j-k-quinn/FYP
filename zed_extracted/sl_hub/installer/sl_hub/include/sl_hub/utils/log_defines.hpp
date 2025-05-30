#define LogDefines_hpp

#ifdef LogDefines_hpp
#define LOG_APP_DEPLOY_REQUEST "Requested: Deploy "
#define LOG_APP_DEPLOY_SUCCESS "Application deployed successfully on device."
#define LOG_APP_UPDATE_SUCCESS "Application updated successfully to "
#define LOG_APP_DEPLOY_FAIL "Cannot deploy application."
#define LOG_APP_DEPLOY_FAIL_MKDIR "An error occured while creating application folder."
#define LOG_APP_DEPLOY_FAIL_METADATA "Invalid metadata in application resources."
#define LOG_APP_DEPLOY_FAIL_DOWNLOAD "An error occured while downloading application resources."
#define LOG_APP_DEPLOY_FAIL_EXTRACT "An error occured while extracting application resources."
#define LOG_APP_DEPLOY_FAIL_UP "An error occured while executing docker-compose up."
#define LOG_APP_DEPLOY_FAIL_BUILD "An error occured while executing docker-compose build."
#define LOG_APP_DEPLOY_FAIL_PULL "An error occured while executing docker-compose pull."
#define LOG_APP_DEPLOY_FAIL_BUILD_PULL "An error occured while executing docker-compose."
#define LOG_APP_DEPLOY_FAIL_COMPOSE "Malformated or unexsting docker-compose.yml."
#define LOG_APP_REMOVE_REQUEST "Requested: Remove "
#define LOG_APP_REMOVE_SUCCESS "Application removed successfully."
#define LOG_APP_DISABLE_REQUEST "Requested: Disable "
#define LOG_APP_DISABLE_SUCCESS "Application disabled successfully."
#define LOG_APP_DISABLE_FAIL "Cannot disable application."
#define LOG_APP_DISABLE_FAIL_DOWN "An error occured while executing docker-compose down"
#define LOG_APP_REMOVE_FAIL "Cannot remove application."
#define LOG_APP_ENABLE_REQUEST "Requested: Enable "
#define LOG_APP_ENABLE_SUCCESS "Application running successfully."
#define LOG_APP_ENABLE_FAIL "Cannot run application."
#define LOG_APP_EXECUTION_FAIL "Application execution was interrupted."

#define LOG_SVO_DOWNLOAD_REQUEST "Requested: Download video from "
#define LOG_SVO_DOWNLOAD_SUCCESS "Video uploaded successfully"
#define LOG_SVO_DOWNLOAD_FAIL "Cannot upload video."
#define LOG_SVO_DOWNLOAD_FAIL_MERGE "An error occured while merging video on device."
#define LOG_SVO_DOWNLOAD_FAIL_DOWNLOAD "An error occured while downloading the video."

#define SL_IOT_CONFIGURATION_CLOUD_STREAM_WIDTH 1280
#define SL_IOT_CONFIGURATION_CLOUD_STREAM_HEIGHT 720
#define SL_IOT_CONFIGURATION_CLOUD_STREAM_BITRATE 1100
#define SL_IOT_CONFIGURATION_CLOUD_STREAM_FPS 15

#define SL_IOT_CONFIGURATION_PREVIEW_WIDTH 448
#define SL_IOT_CONFIGURATION_PREVIEW_HEIGHT 252

#define SL_IOT_CONFIGURATION_MP4_RECORDING_BITRATE 2200

#define SL_IOT_CONFIGURATION_MP4_RECORDING_WIDTH 1280
#define SL_IOT_CONFIGURATION_MP4_RECORDING_HEIGHT 720

#endif
