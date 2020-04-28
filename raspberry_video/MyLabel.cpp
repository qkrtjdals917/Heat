#include "MyLabel.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "SPI.h"
#include <sys/stat.h>

#include <fcntl.h>
#include <QPushButton>

extern uint16_t frameBuffer1[82*60];
extern uint16_t frameBuffer2[82*60];
extern int i_max_changed;
extern int i_min_changed;
extern int button_h, button_w;
extern int temp_max_int;
extern int temp_cnt;

MyLabel::MyLabel(QWidget *parent) : QLabel(parent)
{
	myMax= new QLabel(this); 
}
MyLabel::~MyLabel()
{
}

//when the system calls setImage, we'll set the label's pixmap
void MyLabel::setImage(QImage image) {
  QPixmap pixmap = QPixmap::fromImage(image);
 
  int w = this->width();
  int h = this->height();
  setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
  
   if (i_max_changed == 1){
    	
  	//QLabel *myMax= new QLabel(this); 
	myMax->setGeometry(10,10,80,40);
	QFont f("Arial",20,QFont::Bold);
	myMax->setFont(f);

	char s[20];
	float temp=temp_max_int/59.5-100;
	sprintf(s,"%4.1f",temp);

  	myMax->setText(s);
	
  	myMax->move(button_w*8,button_h*8);
  	myMax->show(); 
	
	
        
	i_max_changed=0;
  }

//qDebug()<<"temp_cnt:"<<temp_cnt;
 if (temp_cnt > 90){
//SpiClosePort(0);
qDebug()<<"SPI 0 Close";
for (int jjj=0;jjj<500;jjj++) ;
		SpiOpenPort(1);

		qDebug() << "Hello SPI1";
		int adc_temp;
		read(spi_cs1_fd, &adc_temp, 1);
		qDebug()<<"SPI 1adc :" <<adc_temp;

		SpiClosePort(1);
		qDebug() << "Bye SPI1";

//qDebug() << "/nHello SPI1 temp :" ;//<< temp_cnt;//adc_temp;
for (int jjj=0;jjj<500;jjj++) ;
//		SpiOpenPort(0);
qDebug()<<"SPI 0 Open";
temp_cnt=0;
}

  FILE *fptr;
        	 
	fptr = fopen("/home/pi/Downloads/LeptonModule-master/software/raspberrypi_video/aa.dat","wb");

	
	fwrite(frameBuffer1,2,80*60,fptr);
	fclose(fptr);
FILE *fptr1;
fptr1 = fopen("/home/pi/Downloads/LeptonModule-master/software/raspberrypi_video/bb.txt","wb");
	
for (int i=0;i<80*60;i++){
	fprintf(fptr1,"%d %d\n",i,frameBuffer1[i]);
	if (i==(80*60-1)){
		fprintf(fptr1,"\n END ");
	}
}	
fclose(fptr1);

	
}


void MyLabel::SetText() {
  
  	QLabel::setText("Hello");
  	QLabel::move(button_w,button_h);
  	QLabel::show(); 
	i_max_changed=0;
  }
