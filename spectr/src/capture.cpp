#include "capture.h"
#include "optlog.h"
#include <exception>
#include <string>

using namespace std::string_literals;

const std::map<const std::string, long> Capture::cap_props = {
    {"POS_MSEC"s, cv::CAP_PROP_POS_MSEC }, // Current position of the video file in milliseconds. 
    {"POS_FRAMES"s, cv::CAP_PROP_POS_FRAMES }, // 0-based index of the frame to be decoded/captured next. 
    {"POS_AVI_RATIO"s,cv::CAP_PROP_POS_AVI_RATIO }, // Relative position of the video file: 0=start of the film, 1=end of the film. 
    {"FRAME_WIDTH"s, cv::CAP_PROP_FRAME_WIDTH}, // Width of the frames in the video stream. 
    {"FRAME_HEIGHT"s, cv::CAP_PROP_FRAME_HEIGHT}, // Height of the frames in the video stream. 
    {"FPS"s, cv::CAP_PROP_FPS}, // Frame rate
    {"FOURCC"s, cv::CAP_PROP_FOURCC}, // 4-character code of codec
    {"FORMAT"s, cv::CAP_PROP_FORMAT}, // Format of the Mat objects 
    {"MODE"s, cv::CAP_PROP_MODE },// Backend-specific value indicating the current capture mode. 
    {"BRIGHTNESS"s, cv::CAP_PROP_BRIGHTNESS }, // Brightness of the image (only for those cameras that support). 
    {"CONTRAST"s, cv::CAP_PROP_CONTRAST}, // Contrast of the image (only for cameras). 
    {"SATURATION"s, cv::CAP_PROP_SATURATION}, // Saturation of the image (only for cameras). 
    {"HUE"s, cv::CAP_PROP_HUE}, // Hue of the image (only for cameras). 
    {"GAIN"s, cv::CAP_PROP_GAIN}, // Gain of the image (only for those cameras that support). 
    {"EXPOSURE"s, cv::CAP_PROP_EXPOSURE}, // Exposure (only for those cameras that support). 
    {"CONVERT_RGB"s, cv::CAP_PROP_CONVERT_RGB }, // Boolean flags indicating whether images should be converted to RGB. 
    {"MONOCHROME"s, cv::CAP_PROP_MONOCHROME },
    {"SHARPNESS"s, cv::CAP_PROP_SHARPNESS },
    {"AUTO_EXPOSURE"s, cv::CAP_PROP_AUTO_EXPOSURE },
    {"GAMMA"s, cv::CAP_PROP_GAMMA },
    {"TEMPERATURE"s, cv::CAP_PROP_TEMPERATURE },
    {"TRIGGER"s, cv::CAP_PROP_TRIGGER },
    {"TRIGGER_DELAY"s, cv::CAP_PROP_TRIGGER_DELAY },
    {"WHITE_BALANCE_RED_V"s, cv::CAP_PROP_WHITE_BALANCE_RED_V },
    {"ZOOM"s, cv::CAP_PROP_ZOOM },
    {"FOCUS"s, cv::CAP_PROP_FOCUS },
    {"GUID"s, cv::CAP_PROP_GUID },
    {"SPEED"s, cv::CAP_PROP_ISO_SPEED },
    {"BACKLIGHT"s, cv::CAP_PROP_BACKLIGHT },
    {"PAN"s, cv::CAP_PROP_PAN },
    {"TILT"s, cv::CAP_PROP_TILT },
    {"ROLL"s, cv::CAP_PROP_ROLL },
    {"IRIS"s, cv::CAP_PROP_IRIS },
    {"SAR_NUM"s, cv::CAP_PROP_BUFFERSIZE }, // Sample aspect ratio: num/den (num) 
    {"SAR_DEN"s, cv::CAP_PROP_AUTOFOCUS }, //Sample aspect ratio: num/den (den) 
    {"BACKEND"s, cv::CAP_PROP_BACKEND }, // Current backend (enum VideoCaptureAPIs). Read-only property. 
    {"CHANNEL"s, cv::CAP_PROP_CHANNEL}, // Video input or Channel Number (only for those cameras that support) 
    {"AUTO_WB"s, cv::CAP_PROP_AUTO_WB} , // enable/ disable auto white-balance 
    {"WB_TEMPERATURE"s, cv::CAP_PROP_WB_TEMPERATURE}, // white-balance color temperature 
    {"CODEC_PIXEL_FORMAT"s, cv::CAP_PROP_CODEC_PIXEL_FORMAT }, // (read-only) codec's pixel format. 4-character code - see VideoWriter::fourcc . 
    {"BITRATE"s, cv::CAP_PROP_BITRATE}, //(read-only) Video bitrate in kbits/s        
    {"ORIENTATION_META"s, cv::CAP_PROP_ORIENTATION_META }, // (read-only) Frame rotation defined by stream meta (applicable for FFmpeg back-end only) 
    {"ORIENTATION_AUTO"s, cv::CAP_PROP_ORIENTATION_AUTO } // if true - rotates output frames of CvCapture considering video file's metadata (applicable for FFmpeg back-end only) 
};



std::unique_ptr<Capture> Capture::create(cv::FileStorage& fs) {
    auto src = fs["source"];
    if (src.isInt()) {
        return std::make_unique<CameraCapture>(src, fs);
    }
    else if (src.isString()) {
        return std::make_unique<FileCapture>(src, fs);
    }
    else {
        throw std::runtime_error("No valid source in config file");
    }
}

void Capture::set_property(std::string prop, double value) {
    auto it = cap_props.find(prop);
    if (it == cap_props.end()) {
        log0 << "Invalid property '" << prop << "'" << std::endl;
        return;
    }

    auto ret = cap.set(it->second, value);
    if (!ret) {
        log0 << "Property '" << prop << "' not supported by backend" << std::endl;
    }

    double result_val = cap.get(it->second);
    if (std::abs(result_val - value) > 0.000001) {
        log0 << "Property '" << prop << "' wasn't set to asked value (" << value << ") and equeals " << result_val << std::endl;
    }
    else {
        log1 << "Property '" << prop << "' was setted to " << value << std::endl;
    }
}

int Capture::height() 
{
    return static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
}

int Capture::width() 
{
    return static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
}

/*---------- Camera Capture ------------------------*/

CameraCapture::CameraCapture(int devNo, cv::FileStorage& fs) {
    if (!cap.open(devNo, cv::CAP_DSHOW)) {
        throw std::runtime_error("Can't open camera #"s + std::to_string(devNo));
    };

    auto options = fs["capture-options"];
    if (options.isMap()) {
        for (auto opt : options) {
            set_property(opt.name(), opt);
        };
    }
    else {
        log1 << "options: " << options.type() << std::endl;
    };
}

void CameraCapture::read(cv::Mat& frame) {
    cap.read(frame);
}

/*---------------- File Capture ------------------------- */

FileCapture::FileCapture(std::string path, cv::FileStorage& fs)
: ticks_in_ms(static_cast<const int64_t>(cv::getTickFrequency()/1000))
{
    if (!cap.open(path, cv::CAP_IMAGES)) {
        throw std::runtime_error("Can't open file sequence with "s + path);
    };
    delay_ms = 1000; // default
    auto options = fs["capture-options"];
    if (options.isMap()) {
        auto v = options["FPS"];
        if (v.isInt() && static_cast<int>(v)>0) {
            delay_ms = 1000 / static_cast<int>(v);
        }
    }
}

void FileCapture::read(cv::Mat& frame) 
{
    if (grab_time != 0) {
        int64_t from_last_grab_ms = std::abs(cv::getTickCount() - grab_time) / ticks_in_ms;
        while (from_last_grab_ms < delay_ms) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms - from_last_grab_ms));
            from_last_grab_ms = std::abs(cv::getTickCount() - grab_time) * ticks_in_ms;
        }
    }
    cap.read(frame);
    if (frame.empty()) {
        cap.set(cv::CAP_PROP_POS_FRAMES, cap.get(cv::CAP_PROP_IMAGES_BASE));
        cap.read(frame);
    }
    grab_time = cv::getTickCount();
}