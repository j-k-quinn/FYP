#ifndef HubClient_hpp
#define HubClient_hpp

// IoT includes
#include <sl_hub/utils/json.hpp>
#include <sl_hub/utils/utils.hpp>
#include <memory>

// For ZED SDK Interoperability
#include <sl/Camera.hpp>
#include <sl_hub/utils/log_defines.hpp>

#ifdef _WIN32
#if SL_IO_DEF
#define SL_IOT_EXPORT __declspec(dllexport)
#else
#define SL_IOT_EXPORT __declspec(dllimport)
#endif
#else
#define SL_IOT_EXPORT __attribute__((visibility("default")))
#endif

// Stereolabs, sl_hub namespace
namespace sl_hub
{

    /**
@api {enum} LOG_LEVEL 1
@apiGroup 5.Enumerates
@apiDescription Represents the available level of logs.
- `DEBUG` log level for debugging
- `INFO` log level for an info
- `STATUS` log level for a status
- `WARNING` log level for a warning
- `ERROR` log level for an error
- `SUCCESS` log level for a success
- `DISABLED` level where no log will be sent
@apiExample Usage
HubClient::sendLog("Sample log", LOG_LEVEL::INFO);
 */
    enum class LOG_LEVEL
    {
        DEBUG = 0,
        INFO = 1,
        STATUS = 2,
        WARNING = 3,
        ERROR = 4,
        SUCCESS = 5,
        DISABLED = 6,
    };

    /**
@api {function} toString 2
@apiGroup 5.Enumerates
@apiDescription Converts the given enumerated value into readable text.
@apiParam {LOG_LEVEL} log_level The log level to convert to string
@apiSuccess {std::string} String that reflects the enumerated value.
@apiError {std::string} String containing "Unknown" value.
@apiExample Usage
std::cout << toString(LOG_LEVEL::INFO) << std::endl;
std::cout << LOG_LEVEL::INFO << std::endl;
 */
    std::string /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ toString(LOG_LEVEL log_level);

    inline /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ ::std::ostream &operator<<(std::ostream &os, const LOG_LEVEL &log_level)
    {
        return os << toString(log_level);
    }

    /**
@api {enum} STATUS_CODE 3
@apiGroup 5.Enumerates
@apiDescription Represent the status code that can be returned by functions.
A successful function will return `SUCCESS`.
- `NOT_CONNECTED`  The device is not connected to ZED Hub currently. Returned by the isConnected() method.
- `CANNOT_CREATE_HW_ENCODER`  The device does not have enough hardware encoders. Note that for each camera, the streaming takes one encoder and the recordings take two.
- `INVALID_SYSTEM_CLOCK` The device has an invalid system clock, which can mess with SSL certificates.
- `INVALID_TARGET`  A publish or a subscribe was called with an invalid `TARGET`.
- `NO_RECORDINGS_FOUND`  No recordings were found on the device.
- `CANNOT_LOAD_CONFIGURATION`  Cannot get the local device configuration.
- `CANNOT_CONNECT`  Cannot connect to ZED Hub.
- `FAILURE`  A function has failed.
- `INVALID_FUNCTION_CALL`  A function was called with invalid parameters.
- `INVALID_TOPIC`  A publish on an invalid topic was requested.
- `REST_REQUESTS_DISABLED`  A REST Request was called but the feature is disabled.
- `REQUEST_FAILED`  A REST Request has failed.
- `EMPTY_KEY`  A device configuration read or write was called with an empty key.
- `EMPTY_JSON`  A publish on a topic with an empty JSON is called.
- `NOT_INITIALIZED`  A function was used before HubClient::connect has been successfully called.
- `UNEXPECTED_INIT_TOKEN`  HubClient was initialized with the wrong token (application token for applications or device token for Edge Agent)
- `CANNOT_OPEN_FILE`  Error while opening a file. 
- `SUCCESS`  No Error, Success.
@apiExample Usage
STATUS_CODE status = HubClient::isInitialized();
std::cout << status << std::endl;
 */
    enum class STATUS_CODE
    {
        NOT_CONNECTED = -17,
        CANNOT_CREATE_HW_ENCODER = -16,
        INVALID_SYSTEM_CLOCK = -15,
        INVALID_TARGET = -14,
        NO_RECORDINGS_FOUND = -13,
        CANNOT_LOAD_CONFIGURATION = -12,
        CANNOT_CONNECT = -11,
        FAILURE = -10,
        INVALID_FUNCTION_CALL = -9,
        INVALID_TOPIC = -8,
        REST_REQUESTS_DISABLED = -7,
        REQUEST_FAILED = -6,
        EMPTY_KEY = -5,
        EMPTY_JSON = -4,
        NOT_INITIALIZED = -3,
        UNEXPECTED_INIT_TOKEN = -2,
        CANNOT_OPEN_FILE = -1,
        SUCCESS = 0
    };

/**
@api {function} toString 4
@apiGroup 5.Enumerates
@apiDescription Converts the given enumerated value into readable text.
@apiParam {STATUS_CODE} status The log level to convert to string
@apiSuccess {std::string} String that reflects the enumerated value.
@apiError {std::string} String containing "Unknown" value.
@apiExample Usage
std::cout << toString(STATUS_CODE::SUCCESS) << std::endl;
std::cout << STATUS_CODE::SUCCESS << std::endl;
 */
    std::string /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ toString(STATUS_CODE status);

    inline /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ ::std::ostream &operator<<(::std::ostream &os, const STATUS_CODE &status)
    {
        return os << toString(status);
    }

    /**
 * @api {enum} REST_REQUEST_TYPE 5
 * @apiGroup 5.Enumerates
 * @apiDescription Specify the REST request to perform.
 * - `GET` Defines a GET request.
 * - `POST` Defines a POST request.
 * - `PUT` Defines a PUT request. 
 * - `DELETE` Defines a DELETE request. 
 * @apiExample Usage
 * // Get the device information through a REST request
 * std::string workspace_url;
 * std::string device_id;
 * getWorkspaceUrl(workspace_url);
 * getDeviceId(device_id);
 * std::string endpoint = workspace_url + "/devices/" + device_id;
 * json result;
 * if (HubClient::performRESTRequest(result, REST_REQUEST_TYPE::GET, endpoint) == STATUS_CODE::SUCCESS)
 *     std::cout << "Device Information as REST response: " << result.dump(1) << std::endl;
 */
    enum class REST_REQUEST_TYPE
    {
        GET = 0,
        POST = 1,
        PUT = 2,
        DELETE = 3
    };

    /**
 * @api {function} toString 6
 * @apiGroup 5.Enumerates
 * @apiDescription Converts the given enumerated value into readable text.
 * @apiParam {REST_REQUEST_TYPE} type The rest request type to convert to string.
 * @apiSuccess {std::string} String that reflects the enumerated value.
 * @apiError {std::string} String containing "Unknown" value.
 * @apiExample Usage
 * std::cout << toString(REST_REQUEST_TYPE::GET) << std::endl;
 * std::cout << REST_REQUEST_TYPE::GET << std::endl;
 */
    std::string /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ toString(REST_REQUEST_TYPE type);

    inline /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ ::std::ostream &operator<<(std::ostream &os, const REST_REQUEST_TYPE &type)
    {
        return os << toString(type);
    }

    /**
@api {enum} TARGET 7
@apiGroup 5.Enumerates
@apiDescription Specify the communication target.
This is used in the methods `publishOnTopic` and `subscribeToTopic` to specify the target recipient of the messages.
`TARGET::LOCAL_DEVICE` refers to localhost, and `TARGET::WORKSPACE` refers to any device on the same local network.
- `LOCAL_DEVICE` Target recipients are all applications that run on the current device.
- `WORKSPACE` Target recipients are all devices in the current workspace.
@apiExample Usage
// Subscribe to the "myLocalData" topic on the local device only
HubClient::subscribeToTopic("myLocalData", onNewMessage, TARGET::LOCAL_DEVICE);
 */
    enum class TARGET
    {
        LOCAL_DEVICE = 0,
        WORKSPACE = 1
    };

    /**
@api {function} toString 8
@apiGroup 5.Enumerates
@apiDescription Converts the given enumerated value into readable text.
@apiParam {TARGET} type The rest request type to convert to string.
@apiSuccess {std::string} String that reflects the enumerated value.
@apiError {std::string} String containing "Unknown" value.
@apiExample Usage
std::cout << toString(TARGET::WORKSPACE) << std::endl;
std::cout << TARGET::WORKSPACE << std::endl;
 */
    std::string /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ toString(TARGET type);

    inline /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ ::std::ostream &operator<<(std::ostream &os, const TARGET &type)
    {
        return os << toString(type);
    }

    /**
@api {enum} CALLBACK_TYPE 8
@apiGroup 5.Enumerates
@apiDescription Defines the callback type for `CallbackParameters`.
- `ON_CONFIGURATION_UPDATE` Callback type for a configuration setting update.
- `ON_REMOTE_CALL` Callback type for a remote function call.
- `ON_RESTART_CALL` Callback type for a restart request.
- `ON_PARAMETER_UPDATE` Callback type for a parameter update.
@apiExample Usage
// Set a remote callback
HubClient::setRemoteCallback("functionCall", CALLBACK_TYPE::ON_REMOTE_CALL);
 */
    enum class CALLBACK_TYPE
    {
        ON_CONFIGURATION_UPDATE = 0,
        ON_REMOTE_CALL = 1,
        ON_RESTART_CALL = 2,
        ON_PARAMETER_UPDATE = 3
    };

    /**
@api {enum} PARAMETER_TYPE 9
@apiGroup 5.Enumerates
@apiDescription Defines the parameter type for `CALLBACK_TYPE::ON_PARAMETER_UPDATE` `CallbackParameters`.
- `DEVICE` The parameter is a device parameter (in device twin).
- `APPLICATION` The parameter is an application parameter (in application twin).
@apiExample Usage
// Set a remote callback for the application parameter "alarm"
HubClient::setParameterCallback("alarmUpdate", "alarm", CALLBACK_TYPE::ON_PARAMETER_UPDATE, PARAMETER_TYPE::APPLICATION);
 */

    enum class PARAMETER_TYPE
    {
        DEVICE = 0,
        APPLICATION = 1
    };

    /**
@api {function} toString 10
@apiGroup 5.Enumerates
@apiDescription Converts the given enumerated value into readable text.
@apiParam {CALLBACK_TYPE} type The rest request type to convert to string.
@apiSuccess {std::string} String that reflects the enumerated value.
@apiError {std::string} String containing "Unknown" value.
@apiExample Usage
std::cout << toString(CALLBACK_TYPE::ON_REMOTE_CALL) << std::endl;
std::cout << CALLBACK_TYPE::ON_REMOTE_CALL << std::endl;
 */
    std::string /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ toString(CALLBACK_TYPE type);

    inline /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ ::std::ostream &operator<<(std::ostream &os, const CALLBACK_TYPE &type)
    {
        return os << toString(type);
    }

    ///@cond SHOWHIDDEN
    class HubClientHandler;
    class HubClient;
    class ConfigurationManager;
    ///@endcond

    /**
 * @api {class} FunctionEvent 1
 * @apiGroup 2.FunctionEvent
 * 
 * @apiDescription This is used in the callback function as a parameter.
 * @apiParam {int} status Status of the event.
 * @apiParam {json} result Result sent from the cloud. Can be empty.
 * @apiExample Usage
 * // Define a callback with a FunctionEvent as a parameter
 * void remoteCallback(FunctionEvent& event) {
 *    // Perform action on an event
 *    ...
 * }
 */
    class /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ FunctionEvent
    {
        friend HubClient;
        friend HubClientHandler;

    public:
        int status = -1;
        json result;

        /**
     * @api {function} getFunctionName 2
     * @apiGroup 2.FunctionEvent
     * 
     * @apiDescription Get the registered function name.
     * @apiSuccess {std::string} Function name as std::string.
     * @apiExample Usage
     * // Define a callback with a FunctionEvent as a parameter
     * void remoteCallback(FunctionEvent& event) {
     *    auto function_name = event.getFunctionName();
     * }
    */
        inline std::string getFunctionName() const
        {
            return function_name;
        }

        /**
     * @api {function} getEventParameters 3
     * @apiGroup 2.FunctionEvent
     * 
     * @apiDescription Get event parameters as JSON of the function event.
     * @apiSuccess {json} Event parameters as JSON.
     * @apiExample Usage
     * // Define a callback with a FunctionEvent as a parameter
     * void remoteCallback(FunctionEvent& event) {
     *    auto event_params = event.getEventParameters();
     * }
    */
        inline json getEventParameters() const
        {
            return parameters;
        }

        inline void setEventParameters(json params)
        {
            parameters = params;
        }

        /**
     * @api {function} getEventParameterValue 4
     * @apiGroup 2.FunctionEvent
     * 
     * @apiDescription Parse the event parameters and extract a value for parameter_key.
     * @apiParam {std::string} parameter_key The name of the target parameter.
     * @apiParam {T#} parameter_value A reference to a variable to store the value in.
     * @apiSuccess {int} An int set as 0.
     * @apiError {int} An non-zero value.
     * @apiExample Usage
     * // Define a callback with a FunctionEvent as a parameter
     * void remoteCallback(FunctionEvent& event) {
     *    int value;
     *    auto status_code = event.getEventParameterValue("key", &value);
     * }
    */
        template <typename T>
        int getEventParameterValue(const std::string& parameter_key, T &parameter_value);

        /**
     * @api {struct} FunctionParameterDescription 5
     * @apiGroup 2.FunctionEvent
     * 
     * @apiDescription Defines a function parameter as a name and a default value.
     * @apiParam {std::string} name Name of the parameter.
     * @apiParam {std::string} default_value Default value of the parameter as std::string.
     * @apiExample Usage
     * FunctionParameterDescription param("unit", "meters");
    */
        struct FunctionParameterDescription
        {
            std::string name;
            std::string default_value;

            FunctionParameterDescription(const std::string& name_, const std::string& default_value_ = "")
            {
                name = name_;
                default_value = default_value_;
            }
        };

        /**
     \brief generate a parameter description
     \param parameters: vector of function parameters
     */

        /**
     * @api {function} generateParametersDescription 6
     * @apiGroup 2.FunctionEvent
     * 
     * @apiDescription Generate a parameter description
     * @apiParam {std::vector[FunctionParameterDescription]} parameters Vector of function parameters
     * @apiSuccess {json} A json description of provided parameters
     * @apiExample Usage
     * FunctionEvent event = FunctionEvent();
     * std::vector<FunctionParameterDescription> parameters;
     * parameters.push_back(FunctionParameterDescription("unit", "meters"));
     * json result = event.generateParametersDescription(parameters);
     * std::cout << result.dump() << std::endl;
    */
        static json generateParametersDescription(const std::vector<FunctionParameterDescription>& parameters);

    private:
        std::string function_call_id; // unique ID generated and transmitted by the cloud
        std::string function_name;    // name of the function
        json parameters;
        // Format the response by concatenating the result JSON with function_name + function_call_id + status code
        void getFormattedResponse(json &response);
    };

    /**
 * @api {class} CallbackParameters 1
 * @apiGroup 3.CallbackParameters
 * 
 * @apiDescription Creates a callback "package" (containing function name, parameters, ...) that can be registered using `HubClient::registerFunction`.
 * @apiExample Usage
 * // Setup a callback function in case of a device parameter change
 * CallbackParameters callback_param;
 * // Use "|" if you want your callback to be set on several parameters
 * callback_param.setParameterCallback("onParamChange", "camera_resolution|camera_fps|camera_image_flip", CALLBACK_TYPE::ON_PARAMETER_UPDATE);
 * // Register your function
 * HubClient::registerFunction(onInitParamUpdate, callback_param);
 */
    class /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ CallbackParameters
    {
        friend HubClient;

    public:
        CallbackParameters(){

        };

        /**
     * @api {function} setConfigurationCallback 2
     * @apiGroup 3.CallbackParameters
     *
     * @apiDescription Generates configuration callback parameters (for device twin update).
     * @apiParam {std::string} function_name The name of the function callback.
     * @apiParam {std::string} setting_key The device twin key that will activate the callback on update.
     * @apiParam {callback_type} [callback_type] allowed callback_type for this function is `CALLBACK_TYPE::ON_CONFIGURATION_UPDATE`
     * @apiParam {std::vector[FunctionParameterDescription]} [parameters] Vector of function parameters.
     * @apiExample Usage
     * // Setup a callback function in case of a device twin configuration change
     * CallbackParameters callback_param;
     * callback_param.setConfigurationCallback("onHostSoftwareUpdate", "host.software", CALLBACK_TYPE::ON_CONFIGURATION_UPDATE);
     * // Register your function
     * HubClient::registerFunction(onHostSoftwareUpdate, callback_param);
    */
        void setConfigurationCallback(const std::string& function_name, const std::string& setting_key,
                                      CALLBACK_TYPE callback_type = CALLBACK_TYPE::ON_CONFIGURATION_UPDATE,
                                      const std::vector<FunctionEvent::FunctionParameterDescription>& fct_parameters = {});

        /**
      @api {function} setRemoteCallback 3
      @apiGroup 3.CallbackParameters
      
      @apiDescription Generates remote function callback parameters (for remote function calls).
      @apiParam {std::string} function_name The name of the function callback.
      @apiParam {callback_type} [callback_type] allowed callback_type for this function is `CALLBACK_TYPE::ON_REMOTE_CALL`
      @apiParam {std::vector[FunctionParameterDescription]} [parameters] Vector of function parameters.
      @apiExample Usage
      // Set a remote callback for the function "functionCall"
      CallbackParameters callback_param;
      callback_param.setRemoteCallback("functionCall", CALLBACK_TYPE::ON_REMOTE_CALL);
      // Register your function
      HubClient::registerFunction(onFunctionCall, callback_param);
    */
        void setRemoteCallback(const std::string& function_name, CALLBACK_TYPE callback_type = CALLBACK_TYPE::ON_REMOTE_CALL,
                               const std::vector<FunctionEvent::FunctionParameterDescription>& fct_parameters = {});

        /**
      @api {function} setParameterCallback 4
      @apiGroup 3.CallbackParameters
      
      @apiDescription Generates parameter callback parameters (for device or application requested parameters update).
      @apiParam {std::string} function_name The name of the function callback.
      @apiParam {std::string} function_name The name of the requested device or application parameter that will activate the callback on update. Use | between each "key" if you want your callback to be set on several parameters.
     For example, parameters = "camera_resolution|camera_fps|camera_image_flip": callback will be triggered when one of the 3 parameters changes.
      @apiParam {callback_type} [callback_type] allowed callback_type for this function is `CALLBACK_TYPE::ON_PARAMETER_UPDATE`
      @apiParam {parameter_type} [parameter_type] The requested parameter type that can be `PARAMETER_TYPE::APPLICATION` or `PARAMETER_TYPE::DEVICE` (default: `DEVICE`).
      @apiParam {std::vector[FunctionParameterDescription]} [parameters] Vector of function parameters.
      @apiExample Usage
      // Setup a callback function in case of an application parameter change
      CallbackParameters callback_param;
      // Use "|" if you want your callback to be set on several parameters
      callback_param.setParameterCallback("onParamChange", "first_param|second_param", CALLBACK_TYPE::ON_PARAMETER_UPDATE, PARAMETER_TYPE::APPLICATION);
      // Register your function
      HubClient::registerFunction(onParamUpdate, callback_param);
    */
        void setParameterCallback(const std::string& function_name, const std::string& parameters,
                                  CALLBACK_TYPE callback_type = CALLBACK_TYPE::ON_PARAMETER_UPDATE,
                                  PARAMETER_TYPE parameter_type = PARAMETER_TYPE::DEVICE,
                                  const std::vector<FunctionEvent::FunctionParameterDescription>& fct_parameters = {});

        /**
      @api {function} reset 5
      @apiGroup 3.CallbackParameters
      
      @apiDescription Resets the CallbackParameters structure to reuse it.
      @apiExample Usage
      // Setup a callback function in case of an application parameter change
      CallbackParameters callback_param;
      callback_param.setParameterCallback("onFirstParamChange", "first_param", CALLBACK_TYPE::ON_PARAMETER_UPDATE, PARAMETER_TYPE::APPLICATION);
      // Register your function
      HubClient::registerFunction(onFirstParamUpdate, callback_param);
      // Reset the CallbackParameters structure
      callback_param.reset();
      callback_param.setParameterCallback("onSecondParamChange", "second_param", CALLBACK_TYPE::ON_PARAMETER_UPDATE, PARAMETER_TYPE::APPLICATION);
      // Register your function
      HubClient::registerFunction(onSecondParamUpdate, callback_param);
    */
        void reset();

    private:
        bool init = false;
        std::string function_name;
        CALLBACK_TYPE callback_type;
        PARAMETER_TYPE parameter_type = PARAMETER_TYPE::DEVICE;
        std::vector<FunctionEvent::FunctionParameterDescription> fct_parameters;
        std::string setting_key;
    };

    /**
 * @api {struct} LogParameters 2
 * @apiGroup 4.DataParameters
 * 
 * @apiDescription A structure that defines parameters for logs that can be used in the `HubClient::sendLog` function.
 * @apiParam {unsigned-long-long} timestamp Defines the timestamp of the log in milliseconds. 0 gives the current timestamp when the log is sent. 
 * @apiParam {int} retention Log retention in days. 0 gives the default retention for logs.
 * @apiParam {std::string} reference Gives a specific id to the log to be used to send data to this log.
 * @apiParam {std::string} reference_to The log will be incremented to the log that was sent with this value as `reference`.
 * @apiExample Usage
 * LogParameters log_params;
 * HubClient::sendLog("Info log sent!", LOG_LEVEL::INFO, "label", log_params);
*/
    struct /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ LogParameters
    {
        unsigned long long timestamp = 0;
        int retention = 0;
        std::string reference = "";
        std::string reference_to = "";
        LogParameters(unsigned long long timestamp_ = 0,
                      int retention_ = 0,
                      const std::string&  reference_ = "",
                      const std::string&  reference_to_ = "")
            : timestamp(timestamp_), retention(retention_), reference(reference_), reference_to(reference_to_)
        {
        }
    };

    /**
 * @api {struct} TelemetryParameters 3
 * @apiGroup 4.DataParameters
 * 
 * @apiDescription A structure that defines parameters for telemetry that can be used in the `HubClient::sendTelemetry` function.
 * @apiParam {unsigned-long-long} timestamp Defines the timestamp of the telemetry in milliseconds. 0 gives the current timestamp when the telemetry is sent.
 * @apiParam {int} retention Telemetry retention in days. 0 gives the default retention for telemetry.
 * @apiParam {std::string} [tags] Tags to add to the telemetry.
 * @apiExample Usage
 * ts_start = getCurrentTimestamp();
 * sl_hub::json telemetry_data;
 * // Do some stuff with telemetry_data during a certain amount of time
 * TelemetryParameters telemetry_params;
 * telemetry_params.timestamp = ts_start;
 * // Send the telemetry with the right timestamp
 * HubClient::sendTelemetry("telem", telemetry_data, telemetry_params);
*/
    struct /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ TelemetryParameters
    {
        unsigned long long timestamp = 0;
        int retention = 0;
        std::string tags = "";
        TelemetryParameters(unsigned long long timestamp_ = 0,
                            int retention_ = 0,
                            const std::string& tags_ = "")
            : timestamp(timestamp_), retention(retention_), tags(tags_)
        {
        }
    };

    /**
 * @api {struct} EventParameters 4
 * @apiGroup 4.DataParameters
 * 
 * @apiDescription A structure that defines parameters for events that can be used in the `HubClient::startVideoEvent` function.
 * @apiParam {unsigned-long-long} timestamp Defines the timestamp of the event in milliseconds. 0 gives the current timestamp when the event is sent.
 * @apiParam {std::string} reference Gives a specific id to the event to be used to send data to this event and update it.
 * @apiParam {std::string} reference_to The data of this event will be incremented to the event that was sent with this value as `reference`. The `duration` attribute will be added by the cloud to the event data calculated from the new timestamp.
 * @apiParam {int} retention Event retention in days. 0 gives the default retention for events.
 * @apiExample Usage
 * // Create a ZED Object
 * std::shared_ptr<sl::Camera> p_zed;
 * p_zed.reset(new sl::Camera());
 * // Init sl_hub
 * STATUS_CODE status_hub = HubClient::connect(app_name);
 * if (status_hub != STATUS_CODE::SUCCESS) {
 *     std::cout << "Status: " << status_hub << std::endl;
 *     exit(1);
 * }
 * // Open the ZED Camera
 * sl::ERROR_CODE errZed = p_zed->open();
 * if (errZed != ERROR_CODE::SUCCESS) {
 *     HubClient::sendLog("Camera initialization error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
 *     exit(1);
 * }
 * // Register the ZED Camera
 * status_hub = HubClient::registerCamera(p_zed);
 * if (status_hub != STATUS_CODE::SUCCESS) {
 *     std::cout << "Registration: " << status_hub << std::endl;
 *     exit(1);
 * }
 * // Enable Object Detection
 * p_zed->enableObjectDetection();
 * // Main Loop
 * sl::Mat depth_image;
 * sl::Objects objects;
 * bool same_event = false;
 * std::string curr_event = "";
 * unsigned long long curr_ts = 0;
 * unsigned long long last_ts = 0;
 * while (true) {
 *     if (p_zed->grab() == sl::ERROR_CODE::SUCCESS) {
 *         // Use zed results
 *         curr_ts = getCurrentTimestamp();
 *         p_zed->retrieveObjects(objects);
 *         // If object list size is >= 5 send an event
 *         if (!same_event && objects.object_list.size() >= 5) {
 *             sl_hub::json value;
 *             value["nb_persons"] = objects.object_list.size();
 *             EventParameters event_params;
 *             // Give a unique id to the event
 *             curr_event = random_string();
 *             event_params.reference = curr_event;
 *             HubClient::startVideoEvent("crowd", value, event_params);
 *             same_event = true;
 *             last_ts = getCurrentTimestamp();
 *         }
 *         // If object list size is still >= 5 after the event, update the event
 *         if (same_event && curr_ts > last_ts + 5000 && objects.object_list.size() >= 5) {
 *             sl_hub::json value["nb_persons"] = objects.object_list.size();
 *             // Give the current event id
 *             EventParameters event_params;
 *             event_params.reference_to = curr_event
 *             HubClient::startVideoEvent("crowd", value, event_params);
 *             same_event = true;
 *             last_ts = getCurrentTimestamp();
 *         }
 *         // If object list size is not >= 5 anymore after 5 seconds, finish the event
 *         if (same_event && curr_ts > last_ts + 5000 && objects.object_list.size() < 5) {
 *              same_event = false;
 *              curr_event = "";
 *         }
 *         HubClient::update();
 *         // Live/Playback view will be the depth image.
 *     }
 * }
 */

    struct /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ EventParameters
    {
        unsigned long long timestamp = 0;
        int retention = 0;
        std::string reference = "";
        std::string tags = "";
        std::shared_ptr<sl::Camera> target_camera;
        EventParameters(unsigned long long timestamp_ = 0,
                        int retention_ = 0,
                        const std::string &reference_ = "",
                        const std::string &tags_ = "")
            : timestamp(timestamp_), retention(retention_), reference(reference_), tags(tags_)
        {
        }
    };

    struct /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ CustomConfigurationParameters
    {
        int custom_stream_width;
        int custom_stream_height;
        int custom_stream_bitrate; // In kBits/s, ex: 2200 means 2200KBits/s
        int custom_preview_image_with;
        int custom_preview_image_height;
    };

    /**
     * @api {struct} WebRTCConfigurationParameters 5
     * @apiGroup 4.DataParameters
     *
     * @apiDescription A structure that defines parameters to establish a WebRTC connection, used in the `HubClient::setWebRTCConfig` function.
     * @apiParam {std::string} signaling_host Defines the hostname of a signaling server.
     * @apiParam {int} signaling_port Defines the port of a signaling server.
     * @apiParam {std::string} stun_host Defines the hostname of a STUN/TURN server.
     * @apiParam {int} stun_port Defines the port of a STUN/TURN server.
     * @apiParam {std::string} stun_username Defines a username used to connect to the STUN/TURN server.
     * @apiParam {std::string} stun_credential Defines credentials used to connect to the STUN/TURN server.
     * @apiParam {std::string} data_provider_id Defines the id of the device for the signaling server.
     * @apiExample Usage
     * // Create a ZED Object
     * std::shared_ptr<sl::Camera> p_zed;
     * p_zed.reset(new sl::Camera());
     * // Init sl_hub
     * STATUS_CODE status_hub = HubClient::connect(app_name);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Status: " << status_hub << std::endl;
     *     exit(1);
     * }
     * // Set a custom WebRTC Configuration
     * sl_hub::WebRTCConfigurationParameters config_params;
     * config_params.signaling_host = "localhost";
     * config_params.signaling_port = 8000;
     * config_params.stun_host = "localhost";
     * config_params.stun_port = 9000;
     * config_params.stun_username = "username";
     * config_params.stun_credential = "pwd";
     * config_params.data_provider_id = "sender";
     * status_hub = HubClient::setWebRTCConfig(config_params);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Status: " << status_hub << std::endl;
     *     exit(1);
     * }
     */
    struct /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ WebRTCConfigurationParameters
    {
        std::string signaling_host;
        int signaling_port;
        std::string stun_host;
        int stun_port;
        std::string stun_username;
        std::string stun_credential;
        std::string data_provider_id;
    };

    struct /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ DataProvider
    {
        std::string stream_id;
        std::string label;
        std::string device_id;
        std::string application_id;
    };

        /**
    * @api {struct} UpdateParameters 1
    * @apiGroup 4.DataParameters

    * @apiDescription A structure that defines parameters used in the `HubClient::update` function.
    * @apiParam {bool} enable_recording Enable the recordings (default: `true`).
    * @apiParam {bool} enable_zed_sdk_streaming Enable ZED SDK streaming (default: `false`).
    * @apiParam {bool} enable_rtsp_streaming Enable RTSP streaming (default: `true`). RTSP stream will be available at rtsp://<IP>:<port>/channel
    * @apiParam {bool} rtsp_streaming_port TCP port for RTSP traffic (default: 8554).
    * @apiParam {string} rtsp_streaming_channel Channel name for the RTSP streaming (if not set, the channel will be set as the serial number of the camera or the custom stream id).
    * @apiParam {int} recording_bitrate Bitrate of the recordings in Kbit/s (default: `2200`).
    * @apiParam {int} streaming_width With of the streaming in pixel (default: `1280`).
    * @apiParam {int} streaming_height Height of the streaming in pixel (default: `720`).
    * @apiParam {int} streaming_bitrate Streaming bitrate in Kbit/s (default: `1100`).
    * @apiParam {int} streaming_fps Streaming FPS (default: `15`).

    * @apiExample Usage
    * UpdateParameters update_params;
    * update_params.enable_rtsp_streaming = true;
    * HubClient::update(update_params);
    * HubClient::register(p_zed, update_params);
    */
    struct /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ UpdateParameters
    {
        // Recording
        bool enable_recording = true;

        int jpg_recordings_width = SL_IOT_CONFIGURATION_PREVIEW_WIDTH;
        int jpg_recordings_height = SL_IOT_CONFIGURATION_PREVIEW_HEIGHT;

        int mp4_recordings_width = SL_IOT_CONFIGURATION_MP4_RECORDING_WIDTH;
        int mp4_recordings_height = SL_IOT_CONFIGURATION_MP4_RECORDING_HEIGHT;
        int mp4_recordings_bitrate = SL_IOT_CONFIGURATION_MP4_RECORDING_BITRATE; // Kbit/s

         // Hub streaming
        bool enable_streaming = true;
        // MJPEG (video wall)
        bool enable_mjpeg_streaming = true;
        // RTP Streaming
        bool enable_zed_sdk_streaming = false;
        int zed_sdk_streaming_port = 30000;
        // RTSP Streaming on rtsp://<ip>:<port>/channel
        bool enable_rtsp_streaming = true;
        int rtsp_streaming_port = 8554;
        std::string rtsp_streaming_channel = "";

        // Streaming parameters
        int streaming_width = SL_IOT_CONFIGURATION_CLOUD_STREAM_WIDTH;
        int streaming_height = SL_IOT_CONFIGURATION_CLOUD_STREAM_HEIGHT;
        int streaming_bitrate = SL_IOT_CONFIGURATION_CLOUD_STREAM_BITRATE;
        int streaming_fps = SL_IOT_CONFIGURATION_CLOUD_STREAM_FPS;
    };

    /**
 * @api {class} HubClient 1
 * @apiGroup 1.HubClient
 * 
 * @apiDescription Provides the main interface to connect your program to the cloud.
 * 
 * This class can be used program-wide with the static functions it provides.
 * 
 * An initial configuration is required using the `connect` function.
 * This initialization is available in two ways:
 * - Without a previously defined sl::Camera object: No video-related functions will be available, but Telemetry/Logs/Callbacks will still be available.
 * - With a previously defined sl::Camera object: All video-related functions will be available.
 * 
 * > This class is implemented as a singleton, which means that the constructor isn't available and only the static functions should be used.
 * 
 */
    class /*@cond SHOWHIDDEN*/ SL_IOT_EXPORT /*@endcond*/ HubClient
    {
        friend HubClientHandler;

    public:
        // The HubClient object is a singleton and therefore cannot be copied. You can use its static functions in a program-wide manner.
        HubClient(const HubClient &) = delete;

        // The HubClient object is a singleton and therefore cannot be copied. You can use its static functions in a program-wide manner.
        HubClient &operator=(const HubClient &) = delete;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////// MAIN  ///////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /**
     * @api {static function} connect 100
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Init function for an application that doesn't use the ZED Camera.
     * 
     * > This function does not require a sl::Camera object. Some functions (related to video) will not be available if this function is used without registering a camera with registerCamera.
     * @apiParam {std::string} app_id An identifier for your application
     * @apiParam {bool} [verbose] Set to `false` by default, whether the `HubClient` object will print verbose info in the console
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Init sl_hub
     * STATUS_CODE status_hub = HubClient::connect(app_name);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Status: " << status_hub << std::endl;
     * }
    */
        static STATUS_CODE connect(const std::string &app_id, bool verbose = false);

        static STATUS_CODE enableOfflineMode(const std::string &app_id, bool verbose = false);

        /**
     * @api {static function} disconnect 102
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Disconnect function that properly closes `HubClient` threads and connection.
     * 
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Init sl_hub
     * STATUS_CODE status_hub = HubClient::connect(app_name);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Status: " << status_hub << std::endl;
     * }
     * status_hub = HubClient::disconnect();
    */
        static STATUS_CODE disconnect();

        /**
     * @api {static function} registerCamera 103
     * @apiGroup 1.HubClient
     * 
     * @apiDescription ZED camera registration function for an application.
     *
     * > The ZED camera must be opened before being registered.
     * @apiParam {std::shared_ptr[sl::Camera]} camera Shared pointer of an opened ZED camera object. ZED SDK feature.
     * @apiParam {UpdateParameters} parameters Defines streaming and recording parameters. See `UpdateParameters` for more details.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Create a ZED Object
     * std::shared_ptr<sl::Camera> p_zed;
     * p_zed.reset(new sl::Camera());
     * // Init sl_hub
     * STATUS_CODE status_hub = HubClient::connect(app_name);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Status: " << status_hub << std::endl;
     *     exit(1);
     * }
     * // Open the ZED Camera
     * sl::ERROR_CODE errZed = p_zed->open();
     * if (errZed != ERROR_CODE::SUCCESS) {
     *     HubClient::sendLog("Camera initialization error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
     *     exit(1);
     * }
     * // Register the ZED Camera
     * UpdateParameters update_params;
     * status_hub = HubClient::registerCamera(p_zed, update_params);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Registration: " << status_hub << std::endl;
     *     exit(1);
     * }
    */
        static STATUS_CODE registerCamera(const std::shared_ptr<sl::Camera>& camera, const UpdateParameters& parameters = UpdateParameters());

        /**
     * @api {static function} registerCustomStream 104
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Custom stream registration function for an application.
     * 
     * > This function allows streaming image data from any source to ZED Hub.
     *
     * @apiParam {std::string} stream_id  Define the identifier of the custom stream. Cannot be a integer.
     * @apiParam {UpdateParameters} parameters Defines streaming and recording parameters. See `UpdateParameters` for more details.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Init sl_hub
     * STATUS_CODE status_hub = HubClient::connect(app_name);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Status: " << status_hub << std::endl;
     *     exit(1);
     * }
     * // Register a custom stream
     * UpdateParameters update_params;
     * status_hub = HubClient::registerCamera("my_stream", update_params);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Registration: " << status_hub << std::endl;
     *     exit(1);
     * }
    */
        static STATUS_CODE registerCustomStream(const std::string& stream_id, const UpdateParameters& parameters = UpdateParameters());

        /**
     * @api {static function} isInitialized 199
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Returns the status of `HubClient`'s initialization.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Init sl_hub
     * STATUS_CODE status_hub = HubClient::connect(app_name);
     * if (HubClient::isInitialized() != STATUS_CODE::SUCCESS) {
     *     exit(1);
     * }
     */
        static STATUS_CODE isInitialized();

        /**
     * @api {static function} isConnected 101
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Returns the status of `HubClient`'s connection to ZED Hub.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Init sl_hub
     * STATUS_CODE status_hub = HubClient::connect(app_name);
     * if (HubClient::isConnected() != STATUS_CODE::SUCCESS) {
     *     exit(1);
     * }
     */
        static STATUS_CODE isConnected();


        /**
     * @api {static function} update 105
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Function called in the main loop that updates the video features, live stream and recording, of the `sl_hub` library. Commonly called after each `p_zed->grab`.
     * > This function is mandatory for the video module: if it is not called, the live stream and SVO/MP4 recording won't be available.
     * > HubClient::connect must be called before using this function.
     * @apiParam {std::shared_ptr[sl::Camera]} [camera] Shared pointer of the ZED camera to update features from. ZED SDK feature.
     * If not filled, the `update` will loop on all registered ZED cameras and use the left RGB image.
     * @apiParam {sl::Mat} [image] Image from the SDK to update features. If not filled, the `update` will use the left RGB image.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Create a ZED Object
     * std::shared_ptr<sl::Camera> p_zed;
     * p_zed.reset(new sl::Camera());
     * // Init sl_hub
     * STATUS_CODE status_hub = HubClient::connect(app_name);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Status: " << status_hub << std::endl;
     *     exit(1);
     * }
     * // Open the ZED Camera
     * sl::ERROR_CODE errZed = p_zed->open();
     * if (errZed != ERROR_CODE::SUCCESS) {
     *     HubClient::sendLog("Camera initialization error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
     *     exit(1);
     * }
     * // Register the ZED Camera
     * status_hub = HubClient::registerCamera(p_zed);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Registration: " << status_hub << std::endl;
     *     exit(1);
     * }
     * // Main Loop
     * while (true) {
     *      if (p_zed->grab() == sl::ERROR_CODE::SUCCESS) {
     *         // Use ZED results
     *         // An empty update will use the left image of the registered ZED camera
     *         // HubClient::update();
     *         // Otherwise, you can retrieve a matrix, play with it, and send it to Hub Client
     *         p_zed->retrieveImage(depth, VIEW::DEPTH);
     *         HubClient::update(p_zed, depth);
     *     }
     * }
     */
        static STATUS_CODE update(const std::shared_ptr<sl::Camera> &_zed = std::shared_ptr<sl::Camera>(), sl::Mat _image = sl::Mat());

        /**
     * @api {static function} update 106
     * @apiGroup 1.HubClient
     *
     * @apiDescription Function called in the main loop that updates the video features, live stream and recording, of the `sl_hub` library. Commonly called after each `p_zed->grab`.
     * > This function is mandatory for the video module: if it is not called, the live stream and SVO/MP4 recording won't be available.
     *
     * > HubClient::connect must be called before using this function.
     * > This method can only be used when one camera is registered.
     * > If multiple cameras are registered, please use HubClient::update(std::shared_ptr\<sl::Camera\>, sl::Mat)
     * @apiParam {sl::Mat} image Image from the SDK to update features. If not filled, the `update` will use the left RGB image.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Create a ZED Object
     * std::shared_ptr<sl::Camera> p_zed;
     * p_zed.reset(new sl::Camera());
     * // Init sl_hub
     * STATUS_CODE status_hub = HubClient::connect(app_name);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Status: " << status_hub << std::endl;
     *     exit(1);
     * }
     * // Open the ZED Camera
     * sl::ERROR_CODE errZed = p_zed->open();
     * if (errZed != ERROR_CODE::SUCCESS) {
     *     HubClient::sendLog("Camera initialization error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
     *     exit(1);
     * }
     * // Register the ZED Camera
     * status_hub = HubClient::registerCamera(p_zed);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Registration: " << status_hub << std::endl;
     *     exit(1);
     * }
     * // Main Loop
     * while (true) {
     *      if (p_zed->grab() == sl::ERROR_CODE::SUCCESS) {
     *         // Use ZED results
     *         // An empty update will use the left image of the registered ZED camera
     *         // HubClient::update();
     *         // Otherwise, you can retrieve a matrix, update it, and send it to Hub Client
     *         p_zed->retrieveImage(depth, VIEW::DEPTH);
     *         HubClient::update(depth);
     *     }
     * }
     */
        static STATUS_CODE update(const sl::Mat& _image);

        /**
        * @api {static function} update 110
        * @apiGroup 1.HubClient
        *
        * @apiDescription Function called in the main loop that updates the video features of a custom stream for live streaming.
        * > This function is mandatory for the video module: if it is not called, the live stream won't be available.
        * > HubClient::connect must be called before using this function.
        * @apiParam {std::string} stream_id User defined stream id. This requires to be a unique id and should correspond with a registered stream from registerCustomStream().
        * @apiParam {sl::Mat} image Image to update stream with.
        * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
        * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
        * @apiExample Usage
        * // Create a ZED Object
        * std::shared_ptr<sl::Camera> p_zed;
        * p_zed.reset(new sl::Camera());
        * // Init sl_hub
        * STATUS_CODE status_hub = HubClient::connect(app_name);
        * if (status_hub != STATUS_CODE::SUCCESS) {
        *     std::cout << "Status: " << status_hub << std::endl;
        *     exit(1);
        * }
        * // Open the ZED Camera
        * sl::ERROR_CODE errZed = p_zed->open();
        * if (errZed != ERROR_CODE::SUCCESS) {
        *     HubClient::sendLog("Camera initialization error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
        *     exit(1);
        * }
        * // Register the custom stream
        * status_hub = HubClient::registerCustomStream("my_stream");
        * if (status_hub != STATUS_CODE::SUCCESS) {
        *     std::cout << "Registration: " << status_hub << std::endl;
        *     exit(1);
        * }
        * // Main Loop
        * while (true) {
        *      if (p_zed->grab() == sl::ERROR_CODE::SUCCESS) {
        *         // Use ZED results to update the custom stream
        *         p_zed->retrieveImage(depth, VIEW::DEPTH);
        *         HubClient::update("my_stream", depth);
        *     }
        * }
        */
        static STATUS_CODE update(const std::string& stream_id, const sl::Mat& _image);

        /**
         * @api {static function} update 107
         * @apiGroup 1.HubClient
         * 
         * @apiDescription Function called in the main loop that updates the objects' metadata of the `sl_hub` library. Commonly called after each `p_zed->grab`.
         * > HubClient::connect must be called before using this function.
         * @apiParam {std::shared_ptr[sl::Camera]} camera Shared pointer of the ZED camera to update features from. ZED SDK feature.
         * @apiParam {sl::Objects} objects Objects from the SDK to update features.
         * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
         * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
         * @apiExample Usage
         * // Create a ZED Object
         * std::shared_ptr<sl::Camera> p_zed;
         * p_zed.reset(new sl::Camera());
         * // Init sl_hub
         * STATUS_CODE status_hub = HubClient::connect(app_name);
         * if (status_hub != STATUS_CODE::SUCCESS) {
         *     std::cout << "Status: " << status_hub << std::endl;
         *     exit(1);
         * }
         * // Open the ZED Camera
         * sl::ERROR_CODE errZed = p_zed->open();
         * if (errZed != ERROR_CODE::SUCCESS) {
         *     HubClient::sendLog("Camera initialization error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
         *     exit(1);
         * }
         * // Register the ZED Camera
         * status_hub = HubClient::registerCamera(p_zed);
         * if (status_hub != STATUS_CODE::SUCCESS) {
         *     std::cout << "Registration: " << status_hub << std::endl;
         *     exit(1);
         * }
         * // Enable Position Tracking
         * sl::PositionalTrackingParameters track_params;
         * errZed = p_zed->enablePositionalTracking(track_params);
         * if (errZed != ERROR_CODE::SUCCESS) {
         *     HubClient::sendLog("Enable position tracking error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
         *     exit(1);
         * }
         * // Enable the Objects detection module
         * sl::ObjectDetectionParameters obj_det_params;
         * errZed = p_zed->enableObjectDetection(obj_det_params);
         * if (errZed != ERROR_CODE::SUCCESS) {
         *     HubClient::sendLog("Enable objects detection error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
         *     exit(1);
         * }
         * // Object Detection runtime parameters
         * ObjectDetectionRuntimeParameters objectTracker_parameters_rt;
         * objectTracker_parameters_rt.detection_confidence_threshold = 50;
         * // Main Loop
         * sl::Objects objects;
         * while (true)
         * {
         *      if (p_zed->grab() == sl::ERROR_CODE::SUCCESS) {
         *         // Retrieve Objects
         *         p_zed->retrieveObjects(objects, objectTracker_parameters_rt);
         *         // Update
         *         HubClient::update();
         *         HubClient::update(p_zed, objects);
         *     }
         * }
         */
         static STATUS_CODE update(const std::shared_ptr<sl::Camera> &_zed, const sl::Objects &objects);
         
         /**
         * @api {static function} update 108
         * @apiGroup 1.HubClient
         * 
         * @apiDescription Function called in the main loop that updates the bodies' metadata of the `sl_hub` library. Commonly called after each `p_zed->grab`.
         * > HubClient::connect must be called before using this function.
         * @apiParam {std::shared_ptr[sl::Camera]} camera Shared pointer of the ZED camera to update features from. ZED SDK feature.
         * @apiParam {sl::Bodies} bodies Bodies from the SDK to update features.
         * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
         * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
         * @apiExample Usage
         * // Create a ZED Object
         * std::shared_ptr<sl::Camera> p_zed;
         * p_zed.reset(new sl::Camera());
         * // Init sl_hub
         * STATUS_CODE status_hub = HubClient::connect(app_name);
         * if (status_hub != STATUS_CODE::SUCCESS)
         * {
         *     std::cout << "Status: " << status_hub << std::endl;
         *     exit(1);
         * }
         * // Open the ZED Camera
         * sl::ERROR_CODE errZed = p_zed->open();
         * if (errZed != ERROR_CODE::SUCCESS)
         * {
         *     HubClient::sendLog("Camera initialization error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
         *     exit(1);
         * }
         * // Register the ZED Camera
         * status_hub = HubClient::registerCamera(p_zed);
         * if (status_hub != STATUS_CODE::SUCCESS)
         * {
         *     std::cout << "Registration: " << status_hub << std::endl;
         *     exit(1);
         * }
         * // Enable Position Tracking
         * sl::PositionalTrackingParameters track_params;
         * errZed = p_zed->enablePositionalTracking(track_params);
         * if (errZed != ERROR_CODE::SUCCESS)
         * {
         *     HubClient::sendLog("Enable position tracking error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
         *     exit(1);
         * }
         * // Enable the Body tracking module
         * sl::PositionalTrackingParameters track_params;
         * errZed = p_zed->enableBodyTracking(body_track_params);
         * if (errZed != ERROR_CODE::SUCCESS)
         * {
         *     HubClient::sendLog("Enable body tracking error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
         *     exit(1);
         * }
         * // Body Tracking runtime parameters
         * BodyTrackingRuntimeParameters bt_rt_params;
         * bt_rt_params.detection_confidence_threshold = 50;
         * // Main Loop
         * sl::Bodies bodies;
         * while (true)
         * {
         *     if (p_zed->grab() == sl::ERROR_CODE::SUCCESS)
         *     {
         *         // Retrieve Bodies
         *         p_zed->retrieveBodies(bodies, bt_rt_params);
         *         // Update
         *         HubClient::update();
         *         HubClient::update(p_zed, bodies);
         *     }
         * }
         */         
         static STATUS_CODE update(const std::shared_ptr<sl::Camera> &_zed, const sl::Bodies &bodies);

         /**
         * @api {static function} update 109
         * @apiGroup 1.HubClient
         * 
         * @apiDescription Function called in the main loop that updates the mapping metadata of the `sl_hub` library. Commonly called after each `p_zed->grab`.
         * > HubClient::connect must be called before using this function.
         * @apiParam {std::shared_ptr[sl::Camera]} camera Shared pointer of the ZED camera to update features from. ZED SDK feature.
         * @apiParam {sl::Mesh-or-sl::FusedPointCloud} map Mesh or point cloud from the SDK to update features.
         * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
         * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
         * @apiExample Usage
         * // Create a ZED Object
         * std::shared_ptr<sl::Camera> p_zed;
         * p_zed.reset(new sl::Camera());
         * // Init sl_hub
         * STATUS_CODE status_hub = HubClient::connect(app_name);
         * if (status_hub != STATUS_CODE::SUCCESS) {
         *     std::cout << "Status: " << status_hub << std::endl;
         *     exit(1);
         * }
         * // Open the ZED Camera
         * sl::ERROR_CODE errZed = p_zed->open();
         * if (errZed != ERROR_CODE::SUCCESS) {
         *     HubClient::sendLog("Camera initialization error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
         *     exit(1);
         * }
         * // Register the ZED Camera
         * status_hub = HubClient::registerCamera(p_zed);
         * if (status_hub != STATUS_CODE::SUCCESS) {
         *     std::cout << "Registration: " << status_hub << std::endl;
         *     exit(1);
         * }
         * // Enable Position Tracking
         * sl::PositionalTrackingParameters track_params;
         * errZed = p_zed->enablePositionalTracking(track_params);
         * if (errZed != ERROR_CODE::SUCCESS) {
         *     HubClient::sendLog("Enable position tracking error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
         *     exit(1);
         * }
         * // Enable Spatial Mapping
         * sl::SpatialMappingParameters spatial_mapping_parameters;
         * spatial_mapping_parameters.use_chunk_only = true;
         * spatial_mapping_parameters.map_type = SpatialMappingParameters::SPATIAL_MAP_TYPE::MESH;
         * errZed = p_zed->enableSpatialMapping(spatial_mapping_parameters);
         * if (errZed != ERROR_CODE::SUCCESS) {
         *     HubClient::sendLog("Enable spatial mapping error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
         *     exit(1);
         * }
         * // Main Loop
         * sl::Mesh mesh;
         * chrono::high_resolution_clock::time_point ts_last_mapping_request = chrono::high_resolution_clock::now();
         * chrono::high_resolution_clock::time_point ts_last_mapping_retrieve = chrono::high_resolution_clock::now();
         * while (true)
         * {
         *      if (p_zed->grab() == sl::ERROR_CODE::SUCCESS) {
         *         // Compute elapsed time since the last call of Camera::requestSpatialMapAsync()
         *         auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - ts_last_mapping_request).count();
         *         if (duration > 500)
         *         {
         *            p_zed->requestSpatialMapAsync();
         *            ts_last_mapping_request = chrono::high_resolution_clock::now();
         *         }
         *         if (p_zed->getSpatialMapRequestStatusAsync() == ERROR_CODE::SUCCESS && ts_last_mapping_retrieve < ts_last_mapping_request)
         *         {
         *            p_zed->retrieveSpatialMapAsync(mesh);
         *            ts_last_mapping_retrieve = chrono::high_resolution_clock::now();
         *            HubClient::update(p_zed, mesh);
         *         }
         *         // Update
         *         HubClient::update();
         *     }
         * }
         */
         static STATUS_CODE update(const std::shared_ptr<sl::Camera> &_zed, const sl::Mesh &mesh);

         static STATUS_CODE update(const std::shared_ptr<sl::Camera> &_zed, const sl::FusedPointCloud &pc);

        /**
     * @api {static function} setWebRTCConfig 200
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Function that sets a specific configuration for the WebRTC video streaming and data communication feature of `sl_hub`.
     * > This function can be useful if you want to use a specific signaling or stun server.
     * @apiParam {WebRTCConfigurationParameters} parameters structure with server URLs and other information.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Create a ZED Object
     * std::shared_ptr<sl::Camera> p_zed;
     * p_zed.reset(new sl::Camera());
     * // Init sl_hub
     * STATUS_CODE status_hub = HubClient::connect(app_name);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Status: " << status_hub << std::endl;
     *     exit(1);
     * }
     * // Set a custom WebRTC Configuration
     * sl_hub::WebRTCConfigurationParameters config_params;
     * config_params.signaling_host = "localhost";
     * config_params.signaling_port = 8000;
     * config_params.stun_host = "localhost";
     * config_params.stun_port = 9000;
     * config_params.stun_username = "username";
     * config_params.stun_credential = "pwd";
     * config_params.data_provider_id = "sender";
     * status_hub = HubClient::setWebRTCConfig(config_params);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Status: " << status_hub << std::endl;
     *     exit(1);
     * }
     */
        static STATUS_CODE setWebRTCConfig(const WebRTCConfigurationParameters parameters);

        /**
     * @api {static function} setPeerName 201
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Function that sets the name of the peer client that will use WebRTC video streaming, and data communication features of `sl_hub`.
     * > This name will be used by the `connectToPeer` and `sendDataToPeer` functions to send data to your application.
     * @apiParam {std::string} peer_name string containing the name of your peer client.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // TODO
     */ 
        static STATUS_CODE setPeerName(const std::string& peer_name);

        /**
     * @api {static function} connectToPeer 202
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Function that connects to another peer to exchange data.
     * @apiParam {std::string} peer_name string containing the name of your peer client.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // TODO
     */ 
        static STATUS_CODE connectToPeer(const std::string& peer_name);

        /**
     * @api {static function} addPeerDataCallback 203
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Function that adds a callback when receiving data with a certain label from another peer.
     * > The callback is an std::function with an int& status argument for the status code number, a std::string& answer argument with the message answer and std::string data argument with the content of the data in itself.
     * @apiParam {std::string} data_label string containing the label of the data that triggers the callback.
     * @apiParam {std::function[void]} callback with the function that will be called.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // TODO
     */ 
        static STATUS_CODE addPeerDataCallback(const std::string& data_label, const std::function<void(int& status, std::string& answer, std::string data)> &callback);

        // This one is used for Python wrapper only.
        static STATUS_CODE addPeerDataCallback_python(void *python_callback, const std::string& data_label, const std::function<void(void *, int& status, std::string& answer, std::string data)>& cython_callback);

        /**
     * @api {static function} sendDataToPeer 204
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Function that sends data to a peer client.
     * > This function can only be used with peer clients available in the workspace your device was registered on, using names set with the `setPeerName` function.
     * @apiParam {std::string} peer_name string containing the name of the peer client that will receive the data.
     * @apiParam {std::string} data_label string containing the label of the data that will be sent.
     * @apiParam {std::string} message string with the content of the data sent.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // TODO
     */ 
        static STATUS_CODE sendDataToPeer(const std::string& peer_name, const std::string& data_label, const std::string& message);

        /**
     * @api {static function} sendDataToPeers 205
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Function that sends data to all peer clients connected to the application's peer client.
     * > This function should be used when you want to stream data to many users.
     * @apiParam {std::string} data_label string containing the label of the data that will be sent.
     * @apiParam {std::string} message string with the content of the data sent.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // TODO
     */ 
        static STATUS_CODE sendDataToPeers(const std::string& data_label, const std::string& message);

        /**
     * @api {static function} getWorkspacePeers 206
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Function that returns an array of all the peer clients available in the workspace.
     * @apiSuccess {STATUS_CODE} an array of peer clients.
     * @apiError {std::vector[DataProvider]} Empty array.
     * @apiExample Usage
     * // TODO
     */ 
        static std::vector<DataProvider> getWorkspacePeers();

        /**
     * @api {static function} startVideoEvent 330
     * @apiGroup 1.HubClient
     *
     * @apiDescription Starts a video event.
     *
     * The video event data will be stored on the cloud and can then be queried alongside the associated video if it's available on the device.
     *
     * Video events are data produced or calculated, associated with a recorded video on a device, that need to be reported on the cloud for further process.
     *
     * It's a combination of a label (i.e. a key), a format-free JSON value, a recorded video and internal metadata.
     * > HubClient::connect must be called before using this function.
     *
     * > This method can only be used when one camera is registered.
     * > If multiple cameras are registered, please use HubClient::startVideoEvent(std::shared_ptr<sl::Camera>, std::string, sl_hub::json, sl_hub::EventParameters)
     * @apiParam {std::string} label Label that defines the event. This label can later be queried on the cloud for data filtering for example.
     * @apiParam {json} value Format-free JSON that contains the event metadata value.
     * @apiParam {EventParameters} [event_params] Parameters for events. See `EventParameters` for more details.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Create a ZED Object
     * std::shared_ptr<sl::Camera> p_zed;
     * p_zed.reset(new sl::Camera());
     * // Init sl_hub
     * STATUS_CODE status_hub = HubClient::connect(app_name);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Status: " << status_hub << std::endl;
     *     exit(1);
     * }
     * // Open the ZED Camera
     * sl::ERROR_CODE errZed = p_zed->open();
     * if (errZed != ERROR_CODE::SUCCESS) {
     *     HubClient::sendLog("Camera initialization error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
     *     exit(1);
     * }
     * // Register the ZED Camera
     * status_hub = HubClient::registerCamera(p_zed);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Registration: " << status_hub << std::endl;
     *     exit(1);
     * }
     * // Enable Object Detection
     * p_zed->enableObjectDetection();
     * // Main Loop
     * sl::Mat depth_image;
     * sl::Objects objects;
     * while (true) {
     *     if (p_zed->grab() == sl::ERROR_CODE::SUCCESS) {
     *         // Use zed results
     *         p_zed->retrieveObjects(objects);
     *         if (objects.object_list.size() >= 5) {
     *             sl_hub::json value;
     *             value["nb_persons"] = objects.object_list.size();
     *             HubClient::startVideoEvent("crowd", value);
     *         }
     *         HubClient::update();
     *     }
     * }
     */

        static STATUS_CODE startVideoEvent(const std::string& label,
                                           const sl_hub::json& value,
                                           const EventParameters& event_params = EventParameters());
        /**
     * @api {static function} startVideoEvent 331
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Starts a video event.
     * 
     * The video event data will be stored on the cloud and can then be queried alongside the associated video if it's available on the device.
     * 
     * Video events are data produced or calculated, associated with a recorded video on a device, that need to be reported on the cloud for further process.
     *
     * It's a combination of a label (i.e. a key), a format-free JSON value, a recorded video and internal metadata.
     * > HubClient::connect must be called before using this function.
     * @apiParam {std::shared_ptr[sl::Camera]} target_camera Shared pointer of the target ZED camera. ZED SDK feature.
     * @apiParam {std::string} label Label that defines the event. This label can later be queried on the cloud for data filtering for example.
     * @apiParam {json} value Format-free JSON that contains the event metadata value.
     * @apiParam {EventParameters} [event_params] Parameters for events. See `EventParameters` for more details.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Create a ZED Object
     * std::shared_ptr<sl::Camera> p_zed;
     * p_zed.reset(new sl::Camera());
     * // Init sl_hub
     * STATUS_CODE status_hub = HubClient::connect(app_name);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Status: " << status_hub << std::endl;
     *     exit(1);
     * }
     * // Open the ZED Camera
     * sl::ERROR_CODE errZed = p_zed->open();
     * if (errZed != ERROR_CODE::SUCCESS) {
     *     HubClient::sendLog("Camera initialization error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
     *     exit(1);
     * }
     * // Register the ZED Camera
     * status_hub = HubClient::registerCamera(p_zed);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Registration: " << status_hub << std::endl;
     *     exit(1);
     * }
     * // Enable Object Detection
     * p_zed->enableObjectDetection();
     * // Main Loop
     * sl::Mat depth_image;
     * sl::Objects objects;
     * while (true) {
     *     if (p_zed->grab() == sl::ERROR_CODE::SUCCESS) {
     *         // Use zed results
     *         p_zed->retrieveObjects(objects);
     *         if (objects.object_list.size() >= 5) {
     *             sl_hub::json value;
     *             value["nb_persons"] = objects.object_list.size();
     *             HubClient::startVideoEvent(p_zed, "crowd", value);
     *         }
     *         HubClient::update();
     *     }
     * }
     */

        static STATUS_CODE startVideoEvent(const std::shared_ptr<sl::Camera>& target_camera,
                                           const std::string& label,
                                           const sl_hub::json& value,
                                           const EventParameters& event_params = EventParameters());

        /**
         * @api {static function} updateVideoEvent 332
         * @apiGroup 1.HubClient
         *
         * @apiDescription Update a video event.
         *
         * > HubClient::startVideoEvent must be called before using this function.
         *
         * > This method can only be used when one camera is registered.
         * > If multiple cameras are registered, please use HubClient::updateVideoEvent(std::shared_ptr<sl::Camera>, std::string, sl_hub::json, sl_hub::EventParameters)
         * @apiParam {std::string} label Label that defines the event. This label can later be queried on the cloud for data filtering for example.
         * @apiParam {json} value Format-free JSON that contains the event metadata value.
         * @apiParam {EventParameters} [event_params] Parameters for events. See `EventParameters` for more details.
         * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
         * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
         * @apiExample Usage
         * // Create a ZED Object
         * std::shared_ptr<sl::Camera> p_zed;
         * p_zed.reset(new sl::Camera());
         * // Init sl_hub
         * STATUS_CODE status_hub = HubClient::connect(app_name);
         * if (status_hub != STATUS_CODE::SUCCESS) {
         *     std::cout << "Status: " << status_hub << std::endl;
         *     exit(1);
         * }
         * // Open the ZED Camera
         * sl::ERROR_CODE errZed = p_zed->open();
         * if (errZed != ERROR_CODE::SUCCESS) {
         *     HubClient::sendLog("Camera initialization error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
         *     exit(1);
         * }
         * // Register the ZED Camera
         * status_hub = HubClient::registerCamera(p_zed);
         * if (status_hub != STATUS_CODE::SUCCESS) {
         *     std::cout << "Registration: " << status_hub << std::endl;
         *     exit(1);
         * }
         * // Enable Object Detection
         * p_zed->enableObjectDetection();
         * // Main Loop
         * sl::Mat depth_image;
         * sl::Objects objects;
         * bool event_started = false;
         * while (true) {
         *     if (p_zed->grab() == sl::ERROR_CODE::SUCCESS) {
         *         // Use zed results
         *         p_zed->retrieveObjects(objects);
         *         if (objects.object_list.size() >= 5) {
         *             sl_hub::json value;
         *             value["nb_persons"] = objects.object_list.size();
         *             if (event_started) {
         *                 HubClient::updateVideoEvent("crowd", value);
         *             } else {
         *                 HubClient::startVideoEvent("crowd", value);
         *                 event_started = true;
         *             }
         *         } else {
         *             event_started = false;
         *         }
         *         HubClient::update();
         *     }
         * }
         */
        static STATUS_CODE updateVideoEvent(const std::string& label,
                                            const sl_hub::json& value,
                                            const EventParameters& event_params = EventParameters());

        /**
         * @api {static function} updateVideoEvent 333
         * @apiGroup 1.HubClient
         * 
         * @apiDescription Update a video event.
         *
         * > HubClient::startVideoEvent must be called before using this function.
         * @apiParam {std::shared_ptr[sl::Camera]} target_camera Shared pointer of the target ZED camera. ZED SDK feature.
         * @apiParam {std::string} label Label that defines the event. This label can later be queried on the cloud for data filtering for example.
         * @apiParam {json} value Format-free JSON that contains the event metadata value.
         * @apiParam {EventParameters} [event_params] Parameters for events. See `EventParameters` for more details.
         * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
         * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
         * @apiExample Usage
         * // Create a ZED Object
         * std::shared_ptr<sl::Camera> p_zed;
         * p_zed.reset(new sl::Camera());
         * // Init sl_hub
         * STATUS_CODE status_hub = HubClient::connect(app_name);
         * if (status_hub != STATUS_CODE::SUCCESS) {
         *     std::cout << "Status: " << status_hub << std::endl;
         *     exit(1);
         * }
         * // Open the ZED Camera
         * sl::ERROR_CODE errZed = p_zed->open();
         * if (errZed != ERROR_CODE::SUCCESS) {
         *     HubClient::sendLog("Camera initialization error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
         *     exit(1);
         * }
         * // Register the ZED Camera
         * status_hub = HubClient::registerCamera(p_zed);
         * if (status_hub != STATUS_CODE::SUCCESS) {
         *     std::cout << "Registration: " << status_hub << std::endl;
         *     exit(1);
         * }
         * // Enable Object Detection
         * p_zed->enableObjectDetection();
         * // Main Loop
         * sl::Mat depth_image;
         * sl::Objects objects;
         * bool event_started = false;
         * while (true) {
         *     if (p_zed->grab() == sl::ERROR_CODE::SUCCESS) {
         *         // Use zed results
         *         p_zed->retrieveObjects(objects);
         *         if (objects.object_list.size() >= 5) {
         *             sl_hub::json value;
         *             value["nb_persons"] = objects.object_list.size();
         *             if (event_started) {
         *                 HubClient::updateVideoEvent(p_zed, "crowd", value);
         *             } else {
         *                 HubClient::startVideoEvent(p_zed, "crowd", value);
         *                 event_started = true;
         *             }
         *         } else {
         *             event_started = false; 
         *         }
         *         HubClient::update();
         *     }
         * }
         */
        static STATUS_CODE updateVideoEvent(const std::shared_ptr<sl::Camera>& target_camera,
                                            const std::string& label, const sl_hub::json& value,
                                            const EventParameters& event_params = EventParameters());

        /**
     * @api {static function} purgeVideoStream 334
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Purge the HLS (streaming protocol) live playlist and remove any remaining segment. This can be used when changing camera settings of ZED camera (brightness, contrast, ...).
     * > HubClient::connect must be called before using this function.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * HubClient::purgeVideoStream();
     * HubClient::update();
     */
        static STATUS_CODE purgeVideoStream();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////// TELEMETRY /////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        ///

        /**
     * @api {static function} sendTelemetry 310
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Sends a single telemetry to the cloud. The telemetry will be stored on the cloud and can then be queried.
     * 
     * Telemetry are data produced or calculated that are meant to be reported to the cloud for further processing.
     * 
     * Combination of a label (i.e. a key), a format-free JSON value and internal metadata.
     * > HubClient::connect must be called before using this function.
     * @apiParam {std::string} label Label that defines the telemetry. This label can later be queried on the cloud for data filtering for example.
     * @apiParam {json} value Format-free JSON that contains the telemetry metadata values.
     * @apiParam {TelemetryParameters} [telemetry_parameters] Parameters for events. See `TelemetryParameters` for more details.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Create a ZED Object
     * std::shared_ptr<sl::Camera> p_zed;
     * p_zed.reset(new sl::Camera());
     * // Init sl_hub
     * STATUS_CODE status_hub = HubClient::connect(app_name);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Status: " << status_hub << std::endl;
     *     exit(1);
     * }
     * // Open the ZED Camera
     * sl::ERROR_CODE errZed = p_zed->open();
     * if (errZed != ERROR_CODE::SUCCESS) {
     *     HubClient::sendLog("Camera initialization error: " + std::string(toString(errZed)), LOG_LEVEL::ERROR);
     *     exit(1);
     * }
     * // Register the ZED Camera
     * status_hub = HubClient::registerCamera(p_zed);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Registration: " << status_hub << std::endl;
     *     exit(1);
     * }
     * std::string label= "environment";
     * json value;
     * value["temperature"] = 21.0; // °C
     * value["humidity"] = 91;      // %
     * HubClient::sendTelemetry(label, value);
     */
        static STATUS_CODE sendTelemetry(const std::string& label, const json& value, const TelemetryParameters& telemetry_parameters = TelemetryParameters());

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////// LOCAL COMMUNICATION FUNCTIONS ////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /**
     * @api {static function} publishOnTopic 320
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Publishes a message (JSON format) on the specified topic.
     * > HubClient::connect must be called before using this function.
     * @apiParam {std::string} topic Defines the topic name.
     * @apiParam {json} value Message to be sent in a JSON format.
     * @apiParam {TARGET} [target] Defines the recipient of the message.
     * 
     * If `TARGET::LOCAL_DEVICE ` is used, only the applications on the device that subscribed to this topic will receive the message.
     * 
     * If `TARGET::WORKSPACE` is used, all the applications on the devices of the workspace that are running and that subscribed to this topic will receive the message.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Init sl_hub
     * STATUS_CODE status_hub = HubClient::connect(app_name);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Status: " << status_hub << std::endl;
     * }
     * // To only my device
     * json val_device;
     * val_device["message"] = "Hello myself";
     * HubClient::publishOnTopic("myLocalData", val_device, TARGET::LOCAL_DEVICE);
     * // To all devices in the workspace
     * json val_lan;
     * val_lan["message"] = "Hello all";
     * HubClient::publishOnTopic("myWorkspaceData", val_lan, TARGET::WORKSPACE);
     */
        static STATUS_CODE publishOnTopic(const std::string& topic, const json& value, TARGET target = TARGET::LOCAL_DEVICE);

        /**
     * @api {static function} subscribeToTopic 321
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Subscribes to a specified topic with a callback function.
     * > HubClient::connect must be called before using this function.
     * @apiParam {std::string} topic Defines the topic name.
     * @apiParam {std::function[void]} callback Pointer to a function callback that will be called when a message is received on the subscribed topic.
     * The function must have the following signature: `void callback(const std::string& topic, const std::string& message, TARGET target)`
     * - `const std::string& topic` that defines the topic name.
     * - `const std::string& message` that defines the message (generally JSON but to be parsed).
     * - `TARGET target` that defines the recipient of the message.
     * @apiParam {TARGET} [target] Defines the recipient of the message.
     *
     * If `TARGET::LOCAL_DEVICE` is used, will receive messages only from applications on the device that send to this topic.
     *
     * If `TARGET::WORKSPACE` is used, will receive messages only from applications on devices of the workspace that send to this topic.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * void onNewMessage(std::string topic, std::string message, TARGET target, void *args) {
     *    std::cout << "Message Received: " << message << std:::endl;
     * };
     * int main() {
     *    // Init sl_hub
     *    STATUS_CODE status_hub = HubClient::connect(app_name);
     *    if (status_hub != STATUS_CODE::SUCCESS) {
     *        std::cout << "Status: " << status_hub << std::endl;
     *    }
     *    HubClient::subscribeToTopic("myLocalData", onNewMessage, TARGET::LOCAL_DEVICE);
     * }
     */
        static STATUS_CODE subscribeToTopic(const std::string& topic, const std::function<void(const std::string&, const std::string&, TARGET target)>& callback, TARGET target = TARGET::LOCAL_DEVICE);
        
        // This one is used for Python wrapper only.
        static STATUS_CODE subscribeToTopic_python(void* python_callback, const std::string& topic, const std::function<void(void *, const std::string &, const std::string &, TARGET)>& cython_callback, TARGET target = TARGET::LOCAL_DEVICE);

        /**
     * @api {static function} unsubscribeToTopic 322
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Unsubscribes to a specified topic with a callback function.
     * > HubClient::connect must be called before using this function.
     * @apiParam {std::string} topic Defines the topic name.
     * @apiParam {TARGET} [target] Defines the recipient of the message.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * void onNewMessage(const std::string& topic, const std::string& message, TARGET target) {
     *    std::cout << "Message Received: " << message << std:::endl;
     * };
     * int main() {
     *    // Init sl_hub
     *    STATUS_CODE status_hub = HubClient::connect(app_name);
     *    if (status_hub != STATUS_CODE::SUCCESS) {
     *        std::cout << "Status: " << status_hub << std::endl;
     *    }
     *    HubClient::subscribeToTopic("myLocalData", onNewMessage, TARGET::LOCAL_DEVICE);
     *    //Do stuff
     *    HubClient::unsubscribeToTopic("myLocalData", TARGET::LOCAL_DEVICE);
     * }
     */
        static STATUS_CODE unsubscribeToTopic(const std::string& topic, TARGET target = TARGET::LOCAL_DEVICE);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////// REST API //////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /**
     * @api {static function} getDeviceId 400
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Gets the device's unique id that is stored on the cloud.
     * 
     * This is the device id parameter that needs to be used in a REST request. See the REST API doc for more details.
     * 
     * > HubClient::connect must be called before using this function.
     * @apiParam {std::string} result Device id stored in a std::string.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * std::string device_id;
     * HubClient::getDeviceId(device_id);
     * std::cout << "Device Id: " << device_id << std::endl;
     */
        static STATUS_CODE getDeviceId(std::string &result);

        /**
     * @api {static function} getDeviceType 401
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Gets the device type from the device twin.
     * > HubClient::connect must be called before using this function.
     * @apiParam {std::string#} result Device type stored in a std::string.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * std::string device_type;
     * HubClient::getDeviceType(device_type);
     * std::cout << "Device Type: " << device_type << std::endl;
     */
        static STATUS_CODE getDeviceType(std::string &result);

        /**
     * @api {static function} getDeviceName 402
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Gets the device name from the device twin.
     * > HubClient::connect must be called before using this function.
     * @apiParam {std::string#} result Device name stored in a std::string.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * std::string device_name;
     * HubClient::getDeviceName(device_name);
     * std::cout << "Device Name: " << device_name << std::endl;
     */
        static STATUS_CODE getDeviceName(std::string &result);

        /**
     * @api {static function} getWorkspaceId 403
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Gets the workspace's unique id that is stored on the cloud.
     * 
     * This is the workspace id parameter that needs to be used in a REST request. See the REST API doc for more details.
     * 
     * > HubClient::connect must be called before using this function.
     * @apiParam {std::string#} result Workspace id stored in a std::string.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * std::string workspace_id;
     * HubClient::getWorkspaceId(workspace_id);
     * std::cout << "Workspace Id: " << workspace_id << std::endl;
     */
        static STATUS_CODE getWorkspaceId(std::string &result);

        /**
     * @api {static function} getWorkspaceUrl 405
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Gets the workspace URL of the current workspace.
     * 
     * This is the region URL parameter that needs to be used in a REST request. See the REST API doc for more details.
     * 
     * > HubClient::connect must be called before using this function.
     * @apiParam {std::string#} result Workspace URL stored in a std::string.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.0
     * @apiExample Usage
     * std::string workspace_url;
     * HubClient::getWorkspaceUrl(workspace_url);
     * std::cout << "Workspace URL: " << workspace_url << std::endl;
     */
        static STATUS_CODE getWorkspaceUrl(std::string &result);

        /**
     * @api {static function} getCloudUrl 406
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Gets the cloud URL.
     * 
     * This is the workspace URL parameter that needs to be used in a REST request. See the REST API doc for more details.
     * 
     * > HubClient::connect must be called before using this function.
     * @apiParam {std::string#} result Cloud URL stored in a std::string.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * std::string cloud_url;
     * HubClient::getCloudUrl(cloud_url);
     * std::cout << "Cloud URL: " << cloud_url << std::endl;
     */
        static STATUS_CODE getCloudUrl(std::string &result);

        /**
     * @api {static function} performRESTRequest 411
     * @apiGroup 1.HubClient
     *  
     * @apiDescription Generic function to perform a REST request. See the REST API doc for more details.
     * 
     * > HubClient::connect must be called before using this function.
     * @apiParam {json#} result JSON response of the request.
     * @apiParam {REST_REQUEST_TYPE} type Defines the request type: `GET`, `PUT`, `POST` or `DELETE` as an enum for simplicity.
     * @apiParam {std::string} url URL that defines the end point of the request.
     * @apiParam {json} [payload] JSON payload to add to the request. Not used in `GET` request.
     * @apiParam {std::vector[string]} [headers] Headers to add to the REST Request, default is the result of `HubClient::getHeaders`, containing the access token in the "Authorization" header.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Init sl_hub
     * STATUS_CODE status_hub = HubClient::connect(app_name);
     * if (status_hub != STATUS_CODE::SUCCESS) {
     *     std::cout << "Status: " << status_hub << std::endl;
     * }
     * // Get the device information through a REST request
     * std::string workspace_url;
     * std::string device_id;
     * getWorkspaceUrl(workspace_url);
     * getDeviceId(device_id);
     * std::string endpoint = workspace_url + "/devices/" + device_id;
     * json result;
     * if (HubClient::performRESTRequest(result,REST_REQUEST_TYPE::GET,endpoint) == STATUS_CODE::SUCCESS)
     *     std::cout << "Device Information as REST response: " << result.dump(1) << std::endl;
     */
        static STATUS_CODE performRESTRequest(json &result, REST_REQUEST_TYPE type, const std::string& url, const json& payload = json(), const std::vector<std::string>& headers = getHeaders());

        /**
     * @api {static function} getHeaders 410
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Returns base headers for REST Request to the cloud with "Authorization" containing an access token.
     * 
     * > HubClient::connect must be called before using this function.
     * 
     * @apiSuccess {vector[string]} Vector containing the base "Authorization" header containing an access token.
     * @apiError {vector[string]} Empty vector.
     * @apiExample Usage
     * std::vector<std::string> headers = HubClient::getHeaders();
     */
        static std::vector<std::string> getHeaders();
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////// CALLBACK /////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /**
     * @api {static function} registerFunction 340
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Registers a function callback associated with a function name.
     * 
     * > HubClient::connect must be called before using this function.
     * 
     * @apiParam {std::function[void(FunctionEvent-event)]} callback Function callback that will be called. The function must have these arguments:
     * - FunctionEvent# event: A FunctionEvent containing data to be used in the callback. See `FunctionEvent` for more infos.
     * @apiParam {CallbackParameters} callback_parameters Parameters containing information about the callback function, name, type etc. See `CallbackParameters` for more infos.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Callback function
     * void onInitParamUpdate(FunctionEvent &event) {
     *     event.status = 0;
     *     HubClient::sendLog("Init Parameters Update. Re-opening the camera.",LOG_LEVEL::INFO);
     *     p_zed.close();
     *     sl::InitParameters initParameters;
     *     initParameters.camera_resolution = (sl::RESOLUTION) HubClient::getParameter<int>("camera_resolution", (int) RESOLUTION::HD2K);
     *     initParameters.camera_image_flip = HubClient::getParameter<bool>("camera_image_flip", false);
     *     initParameters.camera_fps = HubClient::getParameter<int>("camera_fps", 15);
     *     p_zed.open(initParameters);
     * }
     * int main(int argc, char **argv) {
     *     // Create camera object
     *     std::shared_ptr<sl::Camera> p_zed;
     *     p_zed.reset(new sl::Camera());
     *     // Init sl_hub
     *     STATUS_CODE status_hub = HubClient::connect();
     *     if (status_hub != STATUS_CODE::SUCCESS) {
     *         std::cout << "HubClient " << status_hub << std::endl;
     *         exit(1);
     *     }
     *     status_hub = HubClient::registerCamera(p_zed);
     *     if (status_hub != STATUS_CODE::SUCCESS) {
     *         std::cout << "Registration " << status_hub << std::endl;
     *         exit(1);
     *     }
     *     // Setup a callback function in case of an application parameter change
     *     CallbackParameters callback_param;
     *     // Use "|" if you want your callback to be set on several parameters
     *     callback_param.setParameterCallback("onParamChange", "camera_resolution|camera_fps|camera_image_flip", CALLBACK_TYPE::ON_PARAMETER_UPDATE);
     *     // Register your function
     *     HubClient::registerFunction(onInitParamUpdate, callback_param);
     * }
     */
        static STATUS_CODE registerFunction(const std::function<void(FunctionEvent &event)>& callback, const CallbackParameters& callback_parameters);

        // This one is used for Python wrapper only.
        static STATUS_CODE registerFunction_python(void *python_callback, const std::function<void(void *, FunctionEvent &event)> &cython_callback, const CallbackParameters& callback_parameters);

        /**
     * @api {static function} unregisterFunction 341
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Unregisters callback associated with the function name.
     * 
     * > HubClient::connect must be called before using this function.
     * 
     * @apiParam {std::string} function_name The function name of the function to be unregistered.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * // Setup a callback function in case of an application parameter change
     * CallbackParameters callback_param;
     * // Use "|" if you want your callback to be set on several parameters
     * callback_param.setParameterCallback("onParamChange", "camera_resolution|camera_fps|camera_image_flip", CALLBACK_TYPE::ON_PARAMETER_UPDATE);
     * // Register your function
     * HubClient::registerFunction(onInitParamUpdate, callback_param);
     * // Unregister your function
     * HubClient::unregisterFunction("onParamChange");
     */
        static STATUS_CODE unRegisterFunction(const std::string& function_name);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////// CONFIGURATION / STATUS //////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /**
     * @api {static function} getConfiguration 420
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Gets the content of a section of the device twin.
     * 
     * > HubClient::connect must be called before using this function.
     * 
     * @apiParam {std::string} key The name of the section of the device twin, subsections are separated by dots.
     * @apiParam {json#} result The content of the device twin section stored in a json.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * sl_hub::json content;
     * // Get the host software description from the device twin.
     * HubClient::getConfiguration("host.software", content);
     * std::cout << content.dump(2) << std::endl;
     */
        static STATUS_CODE getConfiguration(const std::string& key, json &result);

        /**
     * @api {static function} updateConfiguration 421
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Updates the content of a section of the device twin.
     * 
     * > HubClient::connect must be called before using this function.
     * 
     * @apiParam {std::string} key The key of the section of the device's twin, subsections are separated by periods.
     * @apiParam {json#} new_conf The new value of the device twin section to update.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * sl_hub::json content;
     * // Get user_data from the device twin.
     * HubClient::getConfiguration("user_data", content);
     * std::cout << "Before: " << content.dump(2) << std::endl;
     * content["data"] = 3
     * // Update user_data
     * HubClient::updateConfiguration("user_data", content);
     * HubClient::getConfiguration("user_data", content);
     * std::cout << "After: " << content.dump(2) << std::endl;
     */
        static STATUS_CODE updateConfiguration(const std::string& key, json &new_conf, bool notify_cloud = true);

        static STATUS_CODE loadApplicationParameters(const std::string& parameters_json_path);

        //deprecated
        static STATUS_CODE getApplicationUserData(json &result);
        static STATUS_CODE setApplicationUserData(json value);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////// PARAMETERS ////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /**
     * @api {static function} getParameter 422
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Gets the requested device/application parameter defined by its name (`parameters.requested` in device/application twin).
     * 
     * > HubClient::connect must be called before using this function.
     * 
     * @apiParam {std::string} parameter_name Defines the parameter name that is stored on the cloud.
     * @apiParam {PARAMETER_TYPE} parameter_type The type of the parameter `PARAMETER_TYPE::DEVICE` for device parameters `PARAMETER_TYPE::APPLICATION` for application parameters.
     * @apiParam {T} default_value The default value to return if no parameter is found.
     * @apiSuccess {T#} The value of the parameter.
     * @apiExample Usage
     * std::string param_str = HubClient::getParameter<std::string>("first_param", PARAMETER_TYPE::APPLICATION, "default_value");
     */
        template <typename T>
        static T getParameter(const std::string& parameter_name, PARAMETER_TYPE parameter_type, const T& default_value);

        /**
     * @api {static function} setParameter 423
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Sets the requested device/application parameter defined by its name (`parameters.requested` in device/application twin).
     * 
     * > HubClient::connect must be called before using this function.
     * 
     * @apiParam {std::string} parameter_name Defines the parameter name that is stored on the cloud.
     * @apiParam {PARAMETER_TYPE} parameter_type The type of the parameter `PARAMETER_TYPE::DEVICE` for device parameters `PARAMETER_TYPE::APPLICATION` for application parameters.
     * @apiParam {T#} value The value to set.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * HubClient::setParameter<std::string>("first_param", PARAMETER_TYPE::APPLICATION, "value");
     */
        template <typename T>
        static STATUS_CODE setParameter(const std::string& param_name, PARAMETER_TYPE parameter_type, T value);

        /**
     * @api {static function} reportParameter 424
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Reports the device/application parameter defined by its name (`parameters.reported` in device/application twin).
     * 
     * > HubClient::connect must be called before using this function.
     * 
     * @apiParam {std::string} parameter_name Defines the parameter name that is stored on the cloud.`
     * @apiParam {PARAMETER_TYPE} parameter_type The type of the parameter `PARAMETER_TYPE::DEVICE` for device parameters `PARAMETER_TYPE::APPLICATION` for application parameters.
     * @apiParam {T} value The value to set.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * HubClient::reportParameter<std::string>("first_param", PARAMETER_TYPE::APPLICATION, "value");
     */
        template <typename T>
        static STATUS_CODE reportParameter(const std::string& param_name, PARAMETER_TYPE parameter_type, T value);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////// LOGS ////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /**
     * @api {static function} sendLog 300
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Sends a log message (cloud and terminal, depending on `LOG_LEVEL` chosen).
     * 
     * > HubClient::connect must be called before using this function.
     * 
     * @apiParam {std::string-or-json} message Message to be sent in a string or json format.
     * @apiParam {LOG_LEVEL} lvl `LOG_LEVEL` of the message. Will be filtered (displayed or not) according to the log level threshold set by `setLogLevelThreshold`.
     * @apiParam {std::string} [label] Label of the message (can be filtered).
     * @apiParam {LogParameters} [log_parameters] Defines a log parameter for this message. See `LogParameters` for more details.
     * @apiSuccess {STATUS_CODE} The `SUCCESS` status code.
     * @apiError {STATUS_CODE} Any other status code `!= SUCCESS`.
     * @apiExample Usage
     * HubClient::sendLog("An error happened", LOG_LEVEL::ERROR);
     */
        static void sendLog(const std::string& message, LOG_LEVEL lvl, const std::string& label = "", const LogParameters& log_parameters = LogParameters());
        static void sendLog(json& message, LOG_LEVEL lvl, const std::string& label = "", const LogParameters& log_parameters = LogParameters());

        /**
     * @api {static function} setLogLevelThreshold 301
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Sets the log level limit to be displayed. Every log with `LOG_LEVEL` below the limit will not be printed.
     * 
     * > HubClient::connect must be called before using this function.
     * 
     * @apiParam {LOG_LEVEL} local_terminal_log_level Log limit for terminal display.
     * @apiParam {LOG_LEVEL} cloud_log_level Log limit for cloud display.
     * @apiExample Usage
     * // Set threshold to error for terminal and info for cloud
     * HubClient::setLogLevelThreshold(LOG_LEVEL::ERROR, LOG_LEVEL::INFO);
     * // This log will be printed in the terminal but not sent to the cloud
     * HubClient::sendLog(log_msg, LOG_LEVEL::ERROR);
     */
        static void setLogLevelThreshold(LOG_LEVEL local_terminal_log_level, LOG_LEVEL cloud_log_level);

        /**
     \brief Get the current log level limit.
     \param local_terminal_log_level: current log limit for terminal display
     \param cloud_log_level: current log limit for cloud display
     */
        /**
     * @api {static function} getLogLevelThreshold 302
     * @apiGroup 1.HubClient
     * 
     * @apiDescription Gets the current log level limit.
     * 
     * > HubClient::connect must be called before using this function.
     * 
     * @apiParam {LOG_LEVEL#} local_terminal_log_level Log limit for terminal display is returned in this variable.
     * @apiParam {LOG_LEVEL#} cloud_log_level Log limit for cloud display is returned in this variable.
     * @apiExample Usage
     * // get threshold
     * LOG_LEVEL terminal_log, cloud_log;
     * HubClient::getLogLevelThreshold(terminal_log, cloud_log);
     * std::cout << "Terminal Log: " << terminal_log << std::endl;
     * std::cout << "Cloud Log: " << cloud_log << std::endl;
     */
        static void getLogLevelThreshold(LOG_LEVEL &local_terminal_log_level, LOG_LEVEL &cloud_log_level);

        static void setForPython();

	     static std::shared_ptr<HubClientHandler> getHandler();

    protected:
        /*As the class is a singleton, the constructor is private*/
        HubClient();

        /*As the class is a singleton, the destructor is private*/
        ~HubClient();

        /* Unique instance .*/
        static HubClient m_single_this;

        /* Internal use*/
        std::shared_ptr<HubClientHandler> m_HubClientHandler;
        
        static STATUS_CODE setup(const std::string &app_id, bool verbose);
    };

}
#endif //HubClient_hpp
