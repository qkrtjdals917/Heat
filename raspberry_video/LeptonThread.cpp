#include "LeptonThread.h"
#include <QLabel>
#include <QPainter>
#include "Palettes.h"
#include "SPI.h"
#include "Lepton_I2C.h"
#include <wiringPi.h>


//#include <stdio.h>
extern int button_h,button_w;
int i_max_changed=0;
int i_min_changed=0;
extern QLabel *myMax;
extern QLabel *myMax2;

int temp_max_int;
int temp_cnt=0;
float cur_temp = 0;

LeptonThread::LeptonThread() : QThread()
{
}

LeptonThread::~LeptonThread() {
}

void LeptonThread::run()
{
	//create the initial image
	myImage = QImage(80, 60, QImage::Format_RGB888);
	 
	//open spi port
	SpiOpenPort(0);

    int dht11_dat[5] = {0, } ;
	int i_max=0;
	int i_min=0;
	
	while(true) {
		//file open
		FILE *fptr1;
		fptr1 = fopen("/home/pi/Downloads/LeptonModule-master/software/raspberrypi_video/aa.txt","wb");
	
		//read data packets from lepton over SPI
		int resets = 0;
		for(int j=0;j<PACKETS_PER_FRAME;j++) {
			//if it's a drop packet, reset j to 0, set to -1 so he'll be at 0 again loop
			read(spi_cs0_fd, result+sizeof(uint8_t)*PACKET_SIZE*j, sizeof(uint8_t)*PACKET_SIZE);
			int packetNumber = result[j*PACKET_SIZE+1];
			if (temp_cnt > 89)
				qDebug()<< "Packet" << packetNumber << j; 
			if(packetNumber != j) {
				//qDebug()<< "Packet" << packetNumber << "j"<<j; 

				j = -1;
				resets += 1;
				usleep(1000);
				//Note: we've selected 750 resets as an arbitrary limit, since there should never be 750 "null" packets between two valid transmissions at the current poll rate
				//By polling faster, developers may easily exceed this count, and the down period between frames may then be flagged as a loss of sync
				if(resets == 750) {
					SpiClosePort(0);
					usleep(750000);
					SpiOpenPort(0);
				}
			}
		}
		

		frameBuffer = (uint16_t *)result;
		
		
		if(resets >= 30) {
			//qDebug() << "done reading, resets: " << resets;
			qDebug() << "happy :" << frameBuffer[10];
		}
		


		

		
		int row, column;
		uint16_t value;
		uint16_t minValue = 65535;
		uint16_t maxValue = 0;

		
		for(int i=0;i<FRAME_SIZE_UINT16;i++) {
			//skip the first 2 uint16_t's of every packet, they're 4 header bytes
			if(i % PACKET_SIZE_UINT16 < 2) {
				continue;
			}
			
			//flip the MSB and LSB at the last second
			int temp = result[i*2];
			result[i*2] = result[i*2+1];
			result[i*2+1] = temp;
			
			value = frameBuffer[i];
			frameBuffer1[i]=value;
			
				fprintf(fptr1,"%d ",value);
			
			
			if(value > maxValue) {
				maxValue = value;
				i_max=i;
				i_max_changed=1;
	
			}
			if(value < minValue) {
				minValue = value;
				i_min=i;
				i_min_changed=1;
			}
			column = i % PACKET_SIZE_UINT16 - 2;
			row = i / PACKET_SIZE_UINT16 ;
		}

		float diff = maxValue - minValue;
		float scale = 255/diff;
			
		QRgb color;
		for(int i=0;i<FRAME_SIZE_UINT16;i++) {
			if(i % PACKET_SIZE_UINT16 < 2) {
				continue;
			}
			value = (frameBuffer[i] - minValue) * scale;
			const int *colormap = colormap_ironblack;
			color = qRgb(colormap[3*value], colormap[3*value+1], colormap[3*value+2]);
			column = (i % PACKET_SIZE_UINT16 ) - 2;
			row = i / PACKET_SIZE_UINT16;
			myImage.setPixel(column, row, color);
		}
		//frameBuffer1=(uint16_t *)result;
		fclose(fptr1);
		//lets emit the signal for update
			
			button_w=i_max % PACKET_SIZE_UINT16 - 2;
			button_h=i_max/ PACKET_SIZE_UINT16 ;
                       

		//const QRect rectangle=QRect(5,5,20,20);
            uint8_t laststate = HIGH ;
                    uint8_t counter = 0 ;
                    uint8_t j = 0, i ;
                    uint8_t flag = HIGH ;
                    uint8_t state = 0 ;

                    dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0 ;
                    pinMode(DHTPIN, OUTPUT) ;
                    digitalWrite(DHTPIN, LOW) ;
                    delay(18) ;
                    digitalWrite(DHTPIN, HIGH) ;
                    delayMicroseconds(30) ;
                    pinMode(DHTPIN, INPUT) ;

                    for (i = 0; i < MAXTIMINGS; i++) {
                        counter = 0 ;
                        while ( digitalRead(DHTPIN) == laststate) {
                            counter++ ;
                            delayMicroseconds(1) ;
                            if (counter == 200) break ;
                        }
                        laststate = digitalRead(DHTPIN) ;
                        if (counter == 200) break ;
                        if ((i >= 4) && (i % 2 == 0)) {
                            dht11_dat[j / 8] <<= 1 ;
                            if (counter > 50) dht11_dat[j / 8] |= 1 ;
                            j++ ;
                        }
                    }
                    if ((j >= 40) && (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xff))) {
                          cur_temp = dht11_dat[2]+ 0.1*dht11_dat[3];
                    }
		QPainter p(&myImage);
		p.setPen(QPen(Qt::red));
		p.setFont(QFont("Times",5,QFont::Bold));
		//p.drawText(myImage.rect(),Qt::AlignCenter,"Hot")

//p.drawText(button_w,button_h,s);
        p.drawText(button_w,button_h,"Hot");
		//p.drawText(10,5,"Hello");
		//p.drawText(rectangle,Qt::AlignCenter,"Hello");
		temp_max_int=maxValue;
 temp_cnt++;

		emit updateImage(myImage);
		
	}
	
	
	//finally, close SPI port just bcuz
	SpiClosePort(0);
}

void LeptonThread::save() {
    FILE *fptr1;
    fptr1 = fopen("/home/pi/Downloads/LeptonModule-master/software/raspberrypi_video/bb.txt","wt");
    fprintf(fptr1,"abcd");
    fprintf(fptr1,"current temp %d\n",cur_temp);
    for (int i=0;i<80*60;i++){
        fprintf(fptr1,"%d %d\n",i,frameBuffer1[i]);
        if (i==(80*60-1)){
            fprintf(fptr1,"\n END ");
        }
    }
    fclose(fptr1);
}
