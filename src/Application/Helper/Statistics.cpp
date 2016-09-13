#pragma once
#ifndef OPENMAKAENGINE_STATISTICS_CPP
#define OPENMAKAENGINE_STATISTICS_CPP

#include <fstream>
#include "Statistics.h"
#include "../Controller.h"

using namespace std;
using namespace om;

const std::string Statistics::values[25] = {

        "Detector",
        "Extractor",
        "Matcher",

        "InputResolution",
        "ProcessingResolution",

        "DisplayFunction(ms)",
        "ObjectFound",

        "AnalyzeSceneFrame(ms)",
        "AnalyzerProcess(ms)",

        "SceneKeypoints",
        "ObjectKeypoints",
        "GoodSceneKeypoints",
        "GoodTrainKeypoints",

        "Inliers",
        "Outliers",
        "InliersCalc(ms)",

        "DescriptorType",
        "MatchingDescriptors(ms)",

        "PerspectiveTransform(ms)",

        "TrackerInitialize(ms)",
        "TrackerCorners",
        "isInImage",
        "isObjectInsideImage(ms)",
        "TrackingProcess(ms)",

        // TODO maybe add deviations during multiple test of the same kind?
        "Deviation"

};

Statistics::Statistics(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Creating Statistics instance.." << endl;
    }

    setDefaults();
    wroteHeader = false;

}

Statistics::~Statistics(void) {
}

void Statistics::add(string key, string value) {
    stats[key] = value;
}

void Statistics::write(string filename) {

    ofstream file;
    if (!wroteHeader) {

        // overwrite existing file
        file.open(Controller::STORAGE_PATH + "/" + filename, ios::out);

        // print header line (csv)
        for (int i = 0; i < (sizeof(values) / sizeof(*values)); i++) {
            file << values[i] << ";";
        }

        // print linefeed
        file << endl;

        // close file
        file.close();

        // set header state true
        wroteHeader = true;

    }

    // reopen file in appending mode
    file.open(Controller::STORAGE_PATH + "/" + filename, ios::app);

    // print all attribute keys
    // for (auto outer_iter = stats.begin(); outer_iter != stats.end(); ++outer_iter) {
    // used this to generate header line
    // cout << outer_iter->first << ";";
    // }
    // cout <<endl;

    // print value line (csv)
    for (int i = 0; i < (sizeof(values) / sizeof(*values)); i++) {
        file << stats[values[i]] << ";";
    }

    // print linefeed
    file << std::endl;

    // close file again
    file.close();
}

void Statistics::reset() {

    // clear first
    stats.clear();

    // set default values
    setDefaults();

}

void Statistics::setDefaults() {

    // set default values
    stats["Detector"] = "nill";
    stats["Extractor"] = "nill";
    stats["Matcher"] = "nill";

    stats["InputResolution"] = "nill";
    stats["ProcessingResolution"] = "nill";

    stats["DisplayFunction(ms)"] = "0";
    stats["ObjectFound"] = "nill";

    stats["AnalyzeSceneFrame(ms)"] = "0";
    stats["AnalyzerProcess(ms)"] = "0";

    stats["SceneKeypoints"] = "0";
    stats["ObjectKeypoints"] = "0";
    stats["GoodSceneKeypoints"] = "0";
    stats["GoodTrainKeypoints"] = "0";

    stats["Inliers"] = "0";
    stats["Outliers"] = "0";
    stats["InliersCalc(ms)"] = "0";

    stats["DescriptorType"] = "nill";
    stats["MatchingDescriptors(ms)"] = "0";

    stats["PerspectiveTransform(ms)"] = "0";

    stats["TrackerInitialize(ms)"] = "0";
    stats["TrackerCorners"] = "0";
    stats["isInImage"] = "nill";
    stats["isObjectInsideImage(ms)"] = "0";
    stats["TrackingProcess(ms)"] = "0";

    stats["Deviation"] = "10";

}

#endif
