//
// Created by miacono on 11/08/17.
//

#ifndef ICUB_EVENT_DRIVEN_BOXVISUALIZER_H
#define ICUB_EVENT_DRIVEN_BOXVISUALIZER_H

#include <yarp/os/all.h>
#include <yarp/math/Math.h>
#include <iCub/eventdriven/all.h>

class EventPort : public yarp::os::BufferedPort<ev::vBottle> {
private:
    yarp::os::Semaphore mutex;
    ev::vQueue vLeftQueue;
    ev::vQueue vRightQueue;
    bool isReadingLeft{ false };
    bool isReadingRight{ false };
    bool newEventsLeft{ false };
    bool newEventsRight{ false };

public:
    
    EventPort() {this->useCallback();}
    
    void startReadingLeft() { isReadingLeft = true; }
    void startReadingRight() { isReadingRight = true; }
    void startReading() { startReadingLeft(); startReadingRight(); }
    void stopReading() { isReadingLeft = false; isReadingRight = false; }
    bool isPortReadingLeft() {return isReadingLeft;}
    bool isPortReadingRight() {return isReadingRight;}
    bool isPortReading() {return isReadingRight || isReadingLeft;}
    bool hasNewEvents() {return newEventsLeft || newEventsRight;}
    void clearQueues();
    void onRead(ev::vBottle &bot);
    ev::vQueue getEventsFromChannel(int channel);
};

class BoxesPort : public yarp::os::BufferedPort<yarp::os::Bottle> {
private:
    yarp::os::Bottle boxBottle;
    bool ready{ false };

public:
    
    BoxesPort() {this->useCallback();}
    
    bool isBoxReady() {return ready;}
    void onRead( yarp::os::Bottle &bot ){boxBottle = bot; ready = true;};
    yarp::os::Bottle getBox() { ready = false; return boxBottle;}
};

class BoxConverter : public yarp::os::RFModule{
private:
    EventPort vPortIn;
    BoxesPort boxesPortIn;
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelBgr>> imgPortOut;
    yarp::os::BufferedPort<ev::vBottle> vPortOut;
    yarp::os::BufferedPort<ev::vBottle> vBoxesPortOut;
    std::string confFileName;
    
    int channel;
    int height;
    int width;
    yarp::sig::Matrix H;
    
    double minY{0};
    double minX{0};
    double maxY{0};
    double maxX{0};

public:
    // configure all the module parameters and return true if successful
    virtual bool configure(yarp::os::ResourceFinder &rf);
    virtual bool interruptModule();         // interrupt, e.g., the ports
    virtual bool close();                   // close and shut down the module return
    
    
    virtual bool updateModule();
    virtual double getPeriod();
    
    bool readConfigFile( const yarp::os::ResourceFinder &rf, std::string groupName
                         , yarp::sig::Matrix &homography ) const;
    
    void transformPoint( double &x, double &y, yarp::sig::Matrix homography ) const;
    
};


#endif //ICUB_EVENT_DRIVEN_BOXVISUALIZER_H