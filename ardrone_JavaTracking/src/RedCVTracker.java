import static com.googlecode.javacv.cpp.opencv_core.IPL_DEPTH_8U;
import static com.googlecode.javacv.cpp.opencv_core.cvCreateImage;
import static com.googlecode.javacv.cpp.opencv_core.cvFlip;
import static com.googlecode.javacv.cpp.opencv_core.cvGetSize;
import static com.googlecode.javacv.cpp.opencv_core.cvInRangeS;
import static com.googlecode.javacv.cpp.opencv_core.*;
import static com.googlecode.javacv.cpp.opencv_imgproc.*;
import static com.googlecode.javacv.cpp.opencv_imgproc.CV_MEDIAN;
import static com.googlecode.javacv.cpp.opencv_imgproc.cvCvtColor;
import static com.googlecode.javacv.cpp.opencv_imgproc.cvEqualizeHist;
import static com.googlecode.javacv.cpp.opencv_imgproc.cvGetCentralMoment;
import static com.googlecode.javacv.cpp.opencv_imgproc.cvGetSpatialMoment;
import static com.googlecode.javacv.cpp.opencv_imgproc.cvMoments;
import static com.googlecode.javacv.cpp.opencv_imgproc.cvSmooth;
import static com.googlecode.javacv.cpp.opencv_highgui.*;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.image.BufferedImage;

import javax.swing.JPanel;

import com.googlecode.javacv.CanvasFrame;
import com.googlecode.javacv.FrameGrabber;
import com.googlecode.javacv.VideoInputFrameGrabber;
import com.googlecode.javacv.cpp.opencv_core.CvScalar;
import com.googlecode.javacv.cpp.opencv_core.IplImage;
import com.googlecode.javacv.cpp.opencv_imgproc.CvMoments;

public class RedCVTracker implements Runnable {
    final int INTERVAL = 1000;// 1sec
    final int CAMERA_NUM = 0; // Default camera for this time
    
    static int hueLowerR = 90;
    static int hueUpperR = 150;

    /**
     * Correct the color range- it depends upon the object, camera quality,
     * environment.
     */
    
    //BLUE, GREEN, RED
    
    //H: 180
    //S: 255
    //V: 255
    
    
    //static CvScalar rgba_min = cvScalar(0, 0, 75, 0);// RED wide dabur birko
    //static CvScalar rgba_max = cvScalar(40, 40, 220, 0);
    
    
    
    //dopoldne
    static CvScalar hsv_min = cvScalar(29, 160, 40, 0);
    static CvScalar hsv_max = cvScalar(78, 255, 255, 0);
    
    //BGR
    static CvScalar rgba_min = cvScalar(0, 0, 90, 0);
    static CvScalar rgba_max = cvScalar(50, 50, 255, 0);

    IplImage image;
    CanvasFrame canvas = new CanvasFrame("Quad Cam Live");
    CanvasFrame path = new CanvasFrame("Detection");
    int ii = 0;
    JPanel jp = new JPanel();
    BufferedImage im = null;
    KontrolniStolp1 stolp = null;
    FlightNavigator nav = new FlightNavigator();
    

    public RedCVTracker(KontrolniStolp1 stolp) {
        path.setContentPane(jp);
        this.stolp = stolp;
        canvas.setVisible(false);
        path.setVisible(false);
    }

    @Override
    public void run() {
        //FrameGrabber grabber = new VideoInputFrameGrabber(CAMERA_NUM);
        try {
            //grabber.start();
        	long milis = System.currentTimeMillis();
            IplImage img;
            int posX = 0;
            int posY = 0;
            while (true) {
                img = IplImage.createFrom(im);//grabber.grab();
                if (img != null) {
                    // show image on window
                    cvFlip(img, img, 1);// l-r = 90_degrees_steps_anti_clockwise
                    //canvas.showImage(img);
                    IplImage detectThrs = hsvThreshold(img);//getThresholdImage(img);//getThresholdImage(img);//hsvThreshold(img); //
                    
                    canvas.showImage(detectThrs);
                    
                    CvMoments moments = new CvMoments();
                    cvMoments(detectThrs, moments, 1);
                    //cvtColor
                    double mom10 = cvGetSpatialMoment(moments, 1, 0);
                    double mom01 = cvGetSpatialMoment(moments, 0, 1);
                    double area = cvGetCentralMoment(moments, 0, 0);
                    posX = (int) (mom10 / area);
                    posY = (int) (mom01 / area);
                    // only if its a valid position
                    //if (posX > 0 && posY > 0) {
                        paint(img, posX, posY);
                    //}
                        
                    stolp.posX = posX;
                    stolp.posY = posY;
                        
                    if(stolp.isTakenOff&&stolp.flightMode.equals("visual")){
                    	nav.navigateRotationBasedOnRedDot(stolp, posX, posY, img.width(), img.height());
                    	//nav.navigatePitchBasedOnRedDot(stolp, posX, posY, img.width(), img.height());
                    }
                    
                    milis = System.currentTimeMillis() - milis;
                    if(!(milis > 50))
                    	Thread.sleep(50-milis);
                    
                }
                // Thread.sleep(INTERVAL);
            }
        } catch (Exception e) {
        }
    }

    private void paint(IplImage img, int posX, int posY) {
        Graphics g = jp.getGraphics();
        path.setSize(img.width(), img.height());
         g.clearRect(0, 0, img.width(), img.height());
        g.setColor(Color.RED);
         g.fillOval(posX, posY, 20, 20);
        g.drawOval(posX, posY, 20, 20);
        //System.out.println(posX + " , " + posY);

    }

    private IplImage getThresholdImage(IplImage orgImg) {
        IplImage imgThreshold = cvCreateImage(cvGetSize(orgImg), 8, 1);
        //
        
        cvInRangeS(orgImg, rgba_min, rgba_max, imgThreshold);// red
        //cvMorphologyEx(imgThreshold, imgThreshold, null, null, CV_MOP_GRADIENT, 1);
        cvSmooth(imgThreshold, imgThreshold, CV_MEDIAN, 15);
        cvSaveImage(++ii + "dsmthreshold.jpg", imgThreshold);
        return imgThreshold;
    }
    
    static IplImage hsvThreshold(IplImage orgImg) {
        IplImage imgHSV = cvCreateImage(cvGetSize(orgImg), 8, 3);
        cvCvtColor(orgImg, imgHSV, CV_BGR2HSV);
        IplImage imgThreshold = cvCreateImage(cvGetSize(orgImg), 8, 1);
        cvInRangeS(imgHSV, hsv_min, hsv_max, imgThreshold);
        cvReleaseImage(imgHSV);
        cvSmooth(imgThreshold, imgThreshold, CV_MEDIAN, 13);
        return imgThreshold;
    }

    public IplImage Equalize(BufferedImage bufferedimg) {
        IplImage iploriginal = IplImage.createFrom(bufferedimg);
        IplImage srcimg = IplImage.create(iploriginal.width(), iploriginal.height(), IPL_DEPTH_8U, 1);
        IplImage destimg = IplImage.create(iploriginal.width(), iploriginal.height(), IPL_DEPTH_8U, 1);
        cvCvtColor(iploriginal, srcimg, CV_BGR2HSV);
        cvEqualizeHist(srcimg, destimg);
        return destimg;
    }
}
