DEPRECATED, please check https://github.com/jing-interactive/AirTouch
====================

﻿CamServer is a lightweight camera based multi-touch software written in C++.
It uses OpenCV and some libraries from OpenFrameworks.  
Folder /src contains the main application.   
Folder /samples contains some processing sketches.  
Folder /samples/Tutorial contains the source files of the tutorial.  
OSC and TUIO protocol is employed to send reconginzed blob/face information to client softwares.  

使用说明


CamServer.exe [camera/image/video] [-client 192.168.1.122 ][-port 3333] [-log] [-minim] [-delay 5]  
第一个参数，可以是  
camera		摄像头的编号，默认为0，即第一个摄像头，同时支持使用CLEye驱动的ps3摄像头  
image		图片路径  
video		视频文件的路径  
如果不指定，则打开第一个摄像头   

-client 192.168.1.122	指定客户端的地址，默认为localhost，即本机地址  
-port 3333	指定客户端的端口，默认为 3333  
-delay	5	延迟5秒后再运行  
-log 		在文件中记录运行状况，默认不使用  

TODO:
1. multi-roi 
