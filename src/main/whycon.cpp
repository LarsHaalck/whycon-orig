#include <iostream>
#include <memory>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <unordered_map>

#include "CCircleDetect.h"
#include "geometricMedian.h"

const int numPatterns = 8;

std::unique_ptr<CCircleDetect> detectorArray[numPatterns];
SSegment currentSegmentArray[numPatterns];
SSegment lastSegmentArray[numPatterns];


std::array<std::vector<cv::Point2f>, 4> bigCorners;
std::array<std::vector<cv::Point2f>, 4> smallCorners;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s video.mp4\n", argv[0]);
        return 0;
    }

    for (int i = 0; i < 4; i++)
    {
        bigCorners[i] = std::vector<cv::Point2f>();
        smallCorners[i] = std::vector<cv::Point2f>();
    }

    cv::VideoCapture cap(argv[1]);
    if (!cap.isOpened())
    {
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
    while (true)
    {
        cv::Mat currFrame;
        cap >> currFrame;

        if (currFrame.empty())
            break;

        CRawImage currRawImg(currFrame.ptr<uchar>(0), currFrame.cols, currFrame.rows);

        // track the robots found in the last attempt
        for (int i = 0; i < numPatterns; i++)
        {
            if (currentSegmentArray[i].valid)
            {
                lastSegmentArray[i] = currentSegmentArray[i];
                currentSegmentArray[i]
                    = detectorArray[i]->findSegment(&currRawImg, lastSegmentArray[i]);
            }
        }

        // search for untracked (not detected in the last frame) robots
        for (int i = 0; i < numPatterns; i++)
        {
            if (currentSegmentArray[i].valid == false)
            {
                lastSegmentArray[i].valid = false;
                currentSegmentArray[i]
                    = detectorArray[i]->findSegment(&currRawImg, lastSegmentArray[i]);
            }
            if (currentSegmentArray[i].valid == false)
                break; // does not make sense to search for more patterns if the last was found
        }

        // count number of patterns
        for (int i = 0; i < numPatterns; i++)
        {
            if (currentSegmentArray[i].valid)
            {
                numFound++;
            }
        }
        /* std::cout << "Found: " << numFound << " patterns" << std::endl; */

        if (numFound == numPatterns)
        {
            std::vector<cv::Point2f> upper, lower;
            for (int i = 0; i < numPatterns; i++)
            {
                if (currentSegmentArray[i].y < firstFrame.rows / 2)
                    upper.push_back({currentSegmentArray[i].x, currentSegmentArray[i].y});
                else
                    lower.push_back({currentSegmentArray[i].x, currentSegmentArray[i].y});
            }

            auto sortByX
                = [](const cv::Point2f& lhs, const cv::Point2f& rhs) { return (lhs.x < rhs.x); };

            std::sort(std::begin(upper), std::end(upper), sortByX);
            std::sort(std::begin(lower), std::end(lower), sortByX);

            bigCorners[0].push_back(upper[0]);
            bigCorners[1].push_back(upper[3]);
            bigCorners[2].push_back(lower[0]);
            bigCorners[3].push_back(lower[3]);

            smallCorners[0].push_back(upper[1]);
            smallCorners[1].push_back(upper[2]);
            smallCorners[2].push_back(lower[1]);
            smallCorners[3].push_back(lower[2]);
                
        }

        /* cv::imshow("frame", currFrame); */
        /* cv::waitKey(50); */
        numFound = numStatic = 0;
    }

    auto tl = geometricMedian(bigCorners[0]);
    auto tr = geometricMedian(bigCorners[1]);
    auto bl = geometricMedian(bigCorners[2]);
    auto br = geometricMedian(bigCorners[3]);

    auto tl2 = geometricMedian(smallCorners[0]);
    auto tr2 = geometricMedian(smallCorners[1]);
    auto bl2 = geometricMedian(smallCorners[2]);
    auto br2 = geometricMedian(smallCorners[3]);


    cv::drawMarker(firstFrame, tl, cv::Scalar(0, 255, 0));
    cv::drawMarker(firstFrame, tr, cv::Scalar(0, 255, 0));
    cv::drawMarker(firstFrame, bl, cv::Scalar(0, 255, 0));
    cv::drawMarker(firstFrame, br, cv::Scalar(0, 255, 0));

    cv::drawMarker(firstFrame, tl2, cv::Scalar(0, 0, 255));
    cv::drawMarker(firstFrame, tr2, cv::Scalar(0, 0, 255));
    cv::drawMarker(firstFrame, bl2, cv::Scalar(0, 0, 255));
    cv::drawMarker(firstFrame, br2, cv::Scalar(0, 0, 255));

    cv::imshow("frame", firstFrame);
    while(true)
    {
        auto key = cv::waitKey(0);
        if (key == 27)
            break;
    }

    return 0;
}
