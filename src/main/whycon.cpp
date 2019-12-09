#include <iostream>
#include <memory>

#include <opencv2/opencv.hpp>

#include "CCircleDetect.h"


const int numPatterns = 8;

std::unique_ptr<CCircleDetect> detectorArray[numPatterns];
SSegment currentSegmentArray[numPatterns];
SSegment lastSegmentArray[numPatterns];

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s video.mp4\n", argv[0]);
        return 0;
    }

    cv::VideoCapture cap(argv[1]);
    if (!cap.isOpened()) {
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }

    cv::Mat firstFrame;
    cap >> firstFrame;
    if (firstFrame.empty())
        return -1;

    // initialize the circle detectors - each circle has its own detector instance
    for (int i = 0; i < numPatterns; i++)
        detectorArray[i] = std::make_unique<CCircleDetect>(firstFrame.cols, firstFrame.rows, i);

    int numFound = 0;
    int numStatic = 0;
    while (true) {
        cv::Mat currFrame;
        cap >> currFrame;

        if (currFrame.empty())
            break;

        CRawImage currRawImg(currFrame.ptr<uchar>(0), currFrame.cols,
                currFrame.rows);

        // track the robots found in the last attempt
        for (int i = 0; i < numPatterns; i++) {
            if (currentSegmentArray[i].valid) {
                lastSegmentArray[i] = currentSegmentArray[i];
                currentSegmentArray[i] =
                    detectorArray[i]->findSegment(&currRawImg, lastSegmentArray[i]);
            }
        }

        // search for untracked (not detected in the last frame) robots
        for (int i = 0; i < numPatterns; i++) {
            if (currentSegmentArray[i].valid == false) {
                lastSegmentArray[i].valid = false;
                currentSegmentArray[i] =
                    detectorArray[i]->findSegment(&currRawImg, lastSegmentArray[i]);
            }
            if (currentSegmentArray[i].valid == false)
                break; // does not make sense to search for more patterns if the last was found
        }

        // count number of patterns
        for (int i = 0; i < numPatterns; i++) {
            if (currentSegmentArray[i].valid) {
                numFound++;
                if (currentSegmentArray[i].x == lastSegmentArray[i].x)
                    numStatic++;
            }
        }
        std::cout << "Found: " << numFound << " patterns (static: " << numStatic
            << ")" << std::endl;

        numFound = numStatic = 0;

        cv::imshow("frame", currFrame);
        cv::waitKey(50);
    }

    return 0;
}
