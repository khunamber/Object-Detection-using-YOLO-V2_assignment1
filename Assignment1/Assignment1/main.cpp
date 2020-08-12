#include<iostream>
#include<opencv2\opencv.hpp>
#include <opencv2\ml\ml.hpp>
#include <opencv2\ml.hpp>

using namespace std;

int main()
{
	cv::VideoCapture cap(0); //กล้องตัวที่ 1
	//เช็คกล้องรึไฟล์เปิดสำเร็จรึป่าว
	//เปิดกล้อง ถ้าไม่เจอจะแสดงข้อความว่า 'Eror, cannot open webcam'
	if (!cap.isOpened())
	{
		cout << "Eror, cannot open webcam" << endl;
		cv::waitKey(0); 
		return -1;
	}

	while (true)
	{
		//กำหนด ตัวแปร ของค่าต่างๆ
		cv::Mat img_gray;
		cv::Mat img_gray_blur;
		cv::Mat img_thres;
		cv::Mat closing;
		cv::Mat kernel = cv::Mat(3, 3, CV_8UC1, cv::Scalar(1));
		cv::Mat frame;
		int area;
		int iterations = 10;
		cap >> frame;

		vector<cv::Vec4i> hierarchy;
		vector<vector<cv::Point>> contours;
		
		//แปลงภาพจาก RGB --> GRAY
		cv::cvtColor(frame, img_gray, cv::COLOR_BGR2GRAY);
		//แปลงภาพจาก GRAY --> BLUR
		cv::GaussianBlur(img_gray, img_gray_blur, cv::Size(11, 11), 0);
		//แปลงภาพจาก BLUR --> THRESHOLD
		cv::adaptiveThreshold(img_gray_blur, img_thres, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 23, 2);
		//แปลงภาพจาก THRESHOLD
		cv::morphologyEx(img_thres, closing, cv::MORPH_CLOSE, kernel, cv::Point(-1,-1), iterations);
		//ทำการ fingcontours
		cv::findContours(closing, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		int count_coin1 = 0;//ตัวแปร การนับเหรียญ 1 บาท
		int count_coin10 = 0;//ตัวแปร การนับเหรียญ 10 บาท

		//นำค่าของ Contours มาทำ for Loop เพื่อหาพื้นที่ของแต่ละเหรียญ
		for (int cnt = 0; cnt < contours.size(); cnt++) {

			area = cv::contourArea(contours[cnt]);

			//กำหนดค่า area ของเหรียญ 10 และนับ
			if (area > 6000 && area < 10000) {
				cv::ellipse(frame, cv::fitEllipse(contours[cnt]), cv::Scalar(23, 234, 0), 3);
				count_coin10 += 1;
			}
			//กำหนดค่า area ของเหรียญ 1 และนับ
			if (area > 1000 && area < 5000) {
				cv::ellipse(frame, cv::fitEllipse(contours[cnt]), cv::Scalar(229, 0, 210), 3);
				count_coin1 += 1;
			}
		}
		//***************แสดงข้อความจำนวนของเหรียญ************//
		//เหรียญ 1 บาท
		cv::putText(frame, "Coin 1 Bath: " + to_string(count_coin1), cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(229, 0, 210), 3);
		//เหรียญ 10 บาท
		cv::putText(frame, "Coin 10 Bath: " + to_string(count_coin10), cv::Point(350, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(23, 234, 0), 3);
		
		//แสดงผลลัพธ์ออกทางหน้าจอ
		cv::imshow("original", frame);
		cv::imshow("Gray", img_gray);
		cv::imshow("Blur", img_gray_blur);
		cv::imshow("Threshold", img_thres);
		cv::imshow("Closing", closing);
		  
		char c;
		c = (char)cv::waitKey(10);
		if (c == 27)
		{
			break;
		}
	}

	cap.release();
	cv::destroyAllWindows();
	return 0;
}

	