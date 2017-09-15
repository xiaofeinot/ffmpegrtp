#include "SendRTP.h"
#include "H264Encoder.h"
#include "JPEGEncoder.h"
#include "JPEGDecoder.h"
#include <opencv2/opencv.hpp>

/*int main(void)
{
	CvCapture* pCapture;
	IplImage* pCVFrame;
	cv::Mat image;

	pCapture = cvCreateCameraCapture(-1);
	pCVFrame = NULL;


	//cv::waitKey(3000);
	SendRTP rtp;
	rtp.init();



	//Send a audio/vedio stream to server
	CEncoder encoder(640, 480);  //width=640,height=480;
	while (1)
	{
		pCVFrame = cvQueryFrame(pCapture);
		image = cv::Mat(pCVFrame);
		//cv::cvtColor(image, image, CV_BGR2YUV_I420);
		cv::imshow("image", image);

		char c = cv::waitKey(33);
		if (c == 'q')
		{
			break;
		}
		//encoder.Encode(image.data);
		//rtp.SendH264Nalu(encoder.packet.data, encoder.packet.size);
		//encoder.FreePacket();
	//}


    rtp.sess.BYEDestroy(jrtplib::RTPTime(10, 0), 0, 0);
	system("pause");
    return 0;
}*/

int main()
{
	cv::Mat srcImage = cv::imread("cuc_view.jpg");
	int in_w = srcImage.cols;
	int in_h = srcImage.rows;
	cv::cvtColor(srcImage, srcImage, CV_BGR2YUV_I420);

	cv::imshow("image", srcImage);
	cv::waitKey(0);

	SendRTP rtp;
	rtp.init();

	JPEGDecoder decoder;
	CJPEGEncoder encoder;
	encoder.SetSize(in_w, in_h);
	encoder.Encoder(srcImage.data);

	decoder.Decode(encoder.packet.data,encoder.packet.size,NULL);
	int width;
	int height;
	decoder.GetSize(width, height);
	cv::Mat image(cv::Size(width, height), CV_8UC1);
	decoder.GetData(image.data);

	cv::imshow("image", image);
	cv::waitKey(0);

	while (1)
	{
		rtp.SendJPEG(encoder.packet.data, encoder.packet.size);
	}
	encoder.FreePacket();
	rtp.sess.BYEDestroy(jrtplib::RTPTime(10, 0), 0, 0);
	return 0;
}