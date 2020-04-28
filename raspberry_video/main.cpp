#include <QApplication>
#include <QThread>
#include <QMutex>
#include <QMessageBox>

#include <QColor>
#include <QLabel>
#include <QtDebug>
#include <QString>
#include <QPushButton>
#include <stdio.h>
#include "LeptonThread.h"
#include "MyLabel.h"

#define PACKET_SIZE 164
#define PACKET_SIZE_UINT16 (PACKET_SIZE/2)
#define PACKETS_PER_FRAME 60
#define FRAME_SIZE_UINT16 (PACKET_SIZE_UINT16*PACKETS_PER_FRAME)
#define FPS 27;

uint16_t frameBuffer1[PACKET_SIZE_UINT16*PACKETS_PER_FRAME];
uint16_t frameBuffer2[PACKET_SIZE_UINT16*PACKETS_PER_FRAME];
int button_h=200,button_w=200;


int main( int argc, char **argv )
{
	
	//create the app
	QApplication a( argc, argv );

	int h_res=640, v_res=480;
	QWidget *myWidget = new QWidget;
	myWidget->setGeometry(200, 100, 200, 200);
	

	//create an image placeholder for myLabel
	//fill the top left corner with red, just bcuz
	QImage myImage;
	myImage = QImage(h_res, v_res, QImage::Format_RGB888);
	QRgb red = qRgb(255,0,0);
	for(int i=0;i<80;i++) {
		for(int j=0;j<60;j++) {
			myImage.setPixel(i, j, red);
		}
	}

	//create a label, and set it's image to the placeholder
	MyLabel myLabel(myWidget);
	myLabel.setGeometry(0, 0, h_res, v_res);
	myLabel.setPixmap(QPixmap::fromImage(myImage));
	
	MyLabel myMax(myWidget);
	
	//create a FFC button
	QPushButton *button1 = new QPushButton("Perform FFC", myWidget);
	//button1->setGeometry(320/2-50, 290-35, 100, 30);
	button1->setGeometry(button_h, button_w, 100, 30);
        qDebug() << "WWW" << button_h;
	//create a thread to gather SPI data
	//when the thread emits updateImage, the label should update its image accordingly
	LeptonThread *thread = new LeptonThread();
	QObject::connect(thread, SIGNAL(updateImage(QImage)), &myLabel, SLOT(setImage(QImage)));
	//QObject::connect(thread, SIGNAL(updateImage(QImage)), &myMax, SLOT(SetText()));
	//connect ffc button to the thread's ffc action
	QObject::connect(button1, SIGNAL(clicked()), thread, SLOT(performFFC()));
	thread->start();
	
	myWidget->show();
	
	FILE *fptr;
	fptr = fopen("/home/pi/Downloads/LeptonModule-master/software/raspberrypi_video/aa.dat","rb");
	if (fptr == 0) 
		printf("fptr : null");
	else
		printf("fptr : OK");
	//printf("fb : %d",frameBuffer1[80*30+30]);

	fread(frameBuffer2,2,80*60,fptr);
	fclose(fptr);
	printf("fb1 : %d",frameBuffer2[80*30+30]);

	return a.exec();
}

