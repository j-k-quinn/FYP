#ifndef PERFORMANCE_TIMER_H
#define PERFORMANCE_TIMER_H

#include <unistd.h>
#include <sys/vfs.h>
#include <sl_hub/HubClient.hpp>


namespace perf {
    class PerformanceMeasureObject {
        public:
            PerformanceMeasureObject() {}
            PerformanceMeasureObject(std::string _name): name(_name) {}
            
            inline void startTimer()
            {
                start = std::chrono::high_resolution_clock::now();
                duration = 0;
            }

            inline void stopTimer()
            {
                duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000.f;
            }

            inline float getTime()
            {       
                auto tmp = duration;
                duration = 0;
                return tmp;
            }

            inline void setTime(float time_)
            {
                duration = time_;
            }

            std::string name;
        
        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> start;
            float duration = 0;
    };

    enum class MEASUREMENT {
        UPDATE,                         // Total duration of the update method
        RETRIEVE_CAMERA,            // Duration of the check of the p_zed pointer and retrieval of the registered_camera
        INIT_WEBRTC_STREAM,         // Duration of the initialization the webrtc_communicator
        INIT_RTSP_STREAM,           // Duration of the initialization the rtsp_streamer
        INIT_VIDEO_STREAM,          // Duration of the initialization of the streamer (HLS?)
        RETRIEVE_IMAGE,             // Duration of the:
                                    //      - setFrom for sl::Mat if given as input
                                    //      - retrieveImage if sl::Mat not given as input
        RESIZE_IMAGE,               // Duration of the resize of the sl::Mat before ingest
        INGEST_FRAME,               // Duration of the HLS or WebRTC update with new frame
        MJPEG_ADD,                  // Duration of the creation of JPEG file
        MP4_WRITE,                  // Duration of the creation of TS file
        REFRESH_RECORDER,           // Duration of the recoding of SVO (and MP4?)
        APPLY_CAMERA_PARAMETERS,    // Duration of the update of camera parameters (not implemented)
        APPLY_INIT_PARAMETERS,      // Duration of the update of sl::InitParameters
        REPORTS,                     // Duration of twin reports (camera info, streaming status)

        INIT_INTERNAL_ZMQ_CONNECT,   // During init, duration ZMQ connection (discussion with edge_hub_client)
        INIT_INTERNAL_MQTT_CONNECT,   // During init, duration MQTT connection (discussion with edge_hub_client)
        INIT_INTERNAL_TWIN_RETRIEVAL, // During init, duration twin retrieval
        INIT_INTERNAL_APP_RETRIEVAL,  // During init, duration of app twin and stream list retrieval
        INIT_INTERNAL_REMOTE_FUNCTION,// During init, duration of remote function subscription
        INIT_INTERNAL_VIDEO,          // During init, duration of video initialization
        INIT_INTERNAL_CALLBACK_SUBSCRIPTION   // During init, duration of other callbacks subscription
    };

    class PerformanceMeasurer {
        public:
            PerformanceMeasurer()
            {
                is_performance_measure_activated = isInit = false;
            }

            ~PerformanceMeasurer()
            {
                timers.clear();
                if (performance_file.is_open()) 
                    performance_file.close();
            }

            bool isEnabled(){
                return is_performance_measure_activated;
            }

            void enableMeasure(bool enable)
            {
                if (enable)
                {
                    is_performance_measure_activated = true;
                    if (!isInit)
                    {
                        timers[perf::MEASUREMENT::UPDATE] = perf::PerformanceMeasureObject("update");
                        timers[perf::MEASUREMENT::RETRIEVE_CAMERA] = perf::PerformanceMeasureObject("retrieve_camera");
                        timers[perf::MEASUREMENT::INIT_WEBRTC_STREAM] = perf::PerformanceMeasureObject("init_webrtc_stream");
                        timers[perf::MEASUREMENT::INIT_RTSP_STREAM] = perf::PerformanceMeasureObject("init_rtsp_stream");
                        timers[perf::MEASUREMENT::INIT_VIDEO_STREAM] = perf::PerformanceMeasureObject("init_video_stream");
                        timers[perf::MEASUREMENT::RETRIEVE_IMAGE] = perf::PerformanceMeasureObject("retrieve_image");
                        timers[perf::MEASUREMENT::RESIZE_IMAGE] = perf::PerformanceMeasureObject("resize_image");
                        timers[perf::MEASUREMENT::INGEST_FRAME] = perf::PerformanceMeasureObject("ingest_frame");
                        timers[perf::MEASUREMENT::MJPEG_ADD] = perf::PerformanceMeasureObject("mjpeg_add");
                        timers[perf::MEASUREMENT::MP4_WRITE] = perf::PerformanceMeasureObject("mp4_write");
                        timers[perf::MEASUREMENT::REFRESH_RECORDER] = perf::PerformanceMeasureObject("refresh_recorder");
                        timers[perf::MEASUREMENT::APPLY_CAMERA_PARAMETERS] = perf::PerformanceMeasureObject("apply_camera_params");
                        timers[perf::MEASUREMENT::APPLY_INIT_PARAMETERS] = perf::PerformanceMeasureObject("apply_init_params");
                        timers[perf::MEASUREMENT::REPORTS] = perf::PerformanceMeasureObject("reports");
                        timers[perf::MEASUREMENT::INIT_INTERNAL_MQTT_CONNECT] = perf::PerformanceMeasureObject("init_internal_mqtt_connect");
                        timers[perf::MEASUREMENT::INIT_INTERNAL_TWIN_RETRIEVAL] = perf::PerformanceMeasureObject("init_internal_twin_retrieval");
                        timers[perf::MEASUREMENT::INIT_INTERNAL_APP_RETRIEVAL] = perf::PerformanceMeasureObject("init_internal_app_retrieval");
                        timers[perf::MEASUREMENT::INIT_INTERNAL_REMOTE_FUNCTION] = perf::PerformanceMeasureObject("init_internal_remote_function");
                        timers[perf::MEASUREMENT::INIT_INTERNAL_VIDEO] = perf::PerformanceMeasureObject("init_internal_video");
                        timers[perf::MEASUREMENT::INIT_INTERNAL_CALLBACK_SUBSCRIPTION] = perf::PerformanceMeasureObject("init_internal_callback_subscription");

                        isInit = true;
                    }
                }
                else if(is_performance_measure_activated)
                {
                    stopMeasurements();
                }
            }

            void stopMeasurements()
            {
                is_performance_measure_activated = false;
                if (performance_file.is_open()) 
                    performance_file.close();
                timers.clear();
            }

            inline bool check(perf::MEASUREMENT measure)
            {
                return (timers.count(measure) == 1);
            }

            inline void startMeasure(perf::MEASUREMENT measure)
            {
                if(is_performance_measure_activated && check(measure))
                    timers[measure].startTimer();
            }

            inline void stopMeasure(perf::MEASUREMENT measure)
            {
                if (is_performance_measure_activated && check(measure))
                    timers[measure].stopTimer();        
            }

            inline void setTime(perf::MEASUREMENT measure, float value)
            {
                if (is_performance_measure_activated && check(measure))
                    timers[measure].setTime(value);
            }

            inline float getTime(perf::MEASUREMENT measure)
            {
                if (is_performance_measure_activated && check(measure))
                    return timers[measure].getTime();
                else
                    return 0.f;
            }

            inline void addStatus(const sl_hub::STATUS_CODE &status_code)
            {
                if (is_performance_measure_activated) status = sl_hub::toString(status_code);
            }

            void saveMeasures()
            {
                if (is_performance_measure_activated)
                {
                    if (!performance_file.is_open())
                    {
                        // Create File CSV Header
                        performance_file.open(file_path);
                        for (auto& it : timers)
                        {
                            performance_file << it.second.name << ";";
                        }
                        performance_file << "status" << std::endl;
                    }

                    if(performance_file.is_open())
                    {
                        for (auto& it : timers)
                        {
                            performance_file << it.second.getTime() << ";";
                        }
                        performance_file << status << std::endl;
                        status = "";
                    }
                }
            }
        private:
            bool is_performance_measure_activated;
            bool isInit;
            std::string status;
            
            std::string file_path = "/tmp/zed_iot_perf.csv";
            std::ofstream performance_file;
            std::map<perf::MEASUREMENT, perf::PerformanceMeasureObject> timers;
    };
}

#endif /* PERFORMANCE_TIMER_H */
