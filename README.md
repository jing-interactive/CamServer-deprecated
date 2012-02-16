CamServer is a lightweight camera based interative system written in C++.
The main platform is Windows, but linux and MacOSX are also supported.
It uses OpenCV and some libraries from OpenFrameworks.
Folder /Server contains the main application. 
Folder /Client contains some processing sketches.
Folder /Tutorial contains the source files of the tutorial.
OSC and TUIO protocol is employed to send reconginzed blob/face information to client softwares.

It now supports Kinect and PS3 Camera.

The tutorials are provided at [here](http://www.everbox.com/f/bwy7u4c3xDkmKpKMbtTWu7ojqR)
But it's a little bit outdated.

使用说明

CamServer.exe [-i camera/image/video] [-dos] [-log] [-minim] [-delay 5] [-client 192.168.1.122 ][-port 3333] [-face]

第一个参数，可以是
camera		摄像头的编号，默认为0，即第一个摄像头，同时支持使用CLEye驱动的ps3摄像头
image		图片路径
video		视频文件的路径
如果不指定，则打开第一个摄像头
 
-client 192.168.1.122	指定客户端的地址，默认为localhost，即本机地址
-port 3333	指定客户端的端口，默认为3333
-face 1.5	人脸识别功能，默认不开启，可指定放缩比例（比例未实现）
-finger		手指识别功能，默认不开启
-hand		手指识别功能，默认不开启，未实现
-minim		启动时为程序最小化
-delay	5	延迟5秒后再运行
-log 		在文件中记录运行状况，默认不适用