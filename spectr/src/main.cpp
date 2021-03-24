#include <iostream>
#include "sstream"
#include <stdio.h>
#include <chrono>
#include <thread>
#include "window.h"
#include "view.h"
#include "memory"

using namespace cv;
using namespace std;
using namespace std::chrono_literals;

// void on_mouse(int event, int x, int y, int flags, void* userdata) {
//   return;
// }

int main(int, char**) {
  Mat frame;
  MainWindow& win_main = MainWindow::instance();
  NamedWindow win_plot("Спектрограмма");
    
  VideoCapture cap;
  int deviceID = 0;             // 0 = open default camera
  cap.open(deviceID, cv::CAP_DSHOW);
  // cap.open("vid_test1/img_01.jpg", cv::CAP_IMAGES);
  if (!cap.isOpened()) {
    cerr << "ERROR! Unable to open camera\n";
    return -1;
  }

  cap.set(CAP_PROP_MONOCHROME, 1);
  cap.set(CAP_PROP_FRAME_WIDTH, 1024);
  cap.set(CAP_PROP_FRAME_HEIGHT, 768);
  cap.set(CAP_PROP_FPS, 30);

  // Pick one frame for test
  cap.read(frame);
  cout << "FRAME info: channels " << frame.channels() << "; rows " << frame.rows << "; cols " << frame.cols << endl;
  std::stringstream ss;
  ss << "FPS: " << cap.get(CAP_PROP_FPS) << " W:" << cap.get(CAP_PROP_FRAME_WIDTH) << " H:" << cap.get(CAP_PROP_FRAME_HEIGHT)
    << " ISO:" << cap.get(CAP_PROP_ISO_SPEED) << " AUTO_EXP:" << cap.get(CAP_PROP_AUTO_EXPOSURE);
  cout << ss.str() << endl;


  Model_Video model_vid;
  std::shared_ptr<View> view_origing = make_shared<VideoView>(win_main);
  model_vid.subscribe(view_origing);

  const double start_x = 380; //нм
  const double end_x = 780;
  const double dx = (end_x - start_x) / frame.cols;
  Model_Spectr model_spectr(start_x, dx, 1000);
  
  std::shared_ptr<View> view_plot = make_shared<PlotView>(win_plot);
  model_spectr.subscribe(view_plot);

  win_main.draw(frame);
 // win.overlayText(ss.str(), 4000);

  do
  {
    cap.read(frame);
    if (frame.empty()) {
      cap.set(CAP_PROP_POS_FRAMES, 0);
      continue;
    }
    else {
      model_vid.udpate_data(frame);
      model_spectr.udpate_data(frame);
    }
    cv::waitKey(5);
    
  } while (win_main.visible());

  // the camera will be deinitialized automatically in VideoCapture destructor
  return 0;
}
