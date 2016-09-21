/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: arren.glover@iit.it
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

#ifndef __V_PARTICLEMODULE__
#define __V_PARTICLEMODULE__

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <iCub/emorph/all.h>
#include <queue>

#include "vParticleFilter.h"

/*////////////////////////////////////////////////////////////////////////////*/
//VPARTICLEREADER
/*////////////////////////////////////////////////////////////////////////////*/
class vParticleReader : public yarp::os::BufferedPort<emorph::vBottle>
{
private:

    //debug stuff
    yarp::os::BufferedPort<yarp::os::Bottle> scopeOut;
    yarp::os::BufferedPort<yarp::sig::ImageOf <yarp::sig::PixelBgr> > debugOut;
    yarp::os::Bottle weights;
    yarp::os::Stamp pstamp;


    //event reps
    emorph::temporalSurface surfaceLeft;
    emorph::vtsHelper unwrap;

    //particle storage and variables
    std::priority_queue<vParticle> sortedlist;
    std::vector<vParticle> indexedlist;
    vParticle pmax;
    double pwsum;
    double pwsumsq;
    double avgx;
    double avgy;
    double avgr;
    double avgtw;

    //parameters
    emorph::resolution res;
    bool strict;
    int nparticles;
    int rate;

public:

    vParticleReader(unsigned int width = 128, unsigned int height = 128);

    bool    open(const std::string &name, bool strictness = false);
    void    onRead(emorph::vBottle &inBot);
    void    close();
    void    interrupt();

};

/*////////////////////////////////////////////////////////////////////////////*/
//vTemporalHandler
/*////////////////////////////////////////////////////////////////////////////*/
class vSurfaceHandler : public yarp::os::BufferedPort<emorph::vBottle>
{
private:

    //parameters
    emorph::resolution res;
    bool strict;

    //data
    emorph::vQueue queriedQ;
    emorph::temporalSurface surfaceLeft;
    emorph::temporalSurface surfaceRight;
    yarp::os::Stamp pstamp;
    emorph::vtsHelper unwrap;
    unsigned long int tnow;
    unsigned int condTime;
    unsigned int tw;
    bool eventsQueried;
    yarp::os::Semaphore waitsignal;
    yarp::os::Mutex mutexsignal;
    double ptime;

public:

    vSurfaceHandler(unsigned int width = 128, unsigned int height = 128);

    emorph::vQueue queryEvents(unsigned long int conditionTime, unsigned int temporalWindow);

    bool    open(const std::string &name, bool strictness = false);
    void    onRead(emorph::vBottle &inBot);
    void    close();
    void    interrupt();

};

/*////////////////////////////////////////////////////////////////////////////*/
// vParticleObserver
/*////////////////////////////////////////////////////////////////////////////*/
class vPartObsThread : public yarp::os::Thread
{
private:

    int pStart;
    int pEnd;

    double normval;

    std::vector<vParticle> *particles;
    std::vector<int> *deltats;
    emorph::vQueue *stw;

public:

    vPartObsThread(int pStart, int pEnd);
    void setDataSources(std::vector<vParticle> *particles,
                        std::vector<int> *deltats, emorph::vQueue *stw);
    double getNormVal() { return normval; }
    bool threadInit() { return true; }
    void run();
    void threadRelease() {}
};

/*////////////////////////////////////////////////////////////////////////////*/
//particleProcessor
/*////////////////////////////////////////////////////////////////////////////*/
class particleProcessor : public yarp::os::Thread
{
private:

    vSurfaceHandler eventhandler;
    std::vector<vPartObsThread *> computeThreads;
    int nThreads;
    emorph::resolution res;
    double ptime;

    yarp::os::BufferedPort<yarp::sig::ImageOf <yarp::sig::PixelBgr> > debugOut;
    yarp::os::BufferedPort<yarp::os::Bottle> scopeOut;
    emorph::vtsHelper unwrap;
    std::vector<vParticle> indexedlist;
    double avgx;
    double avgy;
    double avgr;
    double avgtw;
    double maxtw;
    double maxlikelihood;
    int nparticles;
    int rate;
    std::string name;
    bool strict;

public:

    particleProcessor(unsigned int height, unsigned int weight, std::string name, bool strict);
    bool threadInit();
    void run();
    void threadRelease();
};




/*////////////////////////////////////////////////////////////////////////////*/
//VPARTICLEMODULE
/*////////////////////////////////////////////////////////////////////////////*/
class vParticleModule : public yarp::os::RFModule
{
    //the event bottle input and output handler
    vParticleReader *particleCallback;
    particleProcessor *particleThread;

public:

    //the virtual functions that need to be overloaded
    virtual bool configure(yarp::os::ResourceFinder &rf);
    virtual bool interruptModule();
    virtual bool close();
    virtual double getPeriod();
    virtual bool updateModule();

};


#endif
