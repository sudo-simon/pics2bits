#include "include/pics2bits.hpp"

#include <cstdint>
#include <iostream>
#include <map>
#include <opencv2/core/matx.hpp>
#include <string>
#include <vector>


using namespace p2b;
using namespace std;

// ----------------------------------------------------------------------

map<string, string> parseArgs(int argc, char** argv){
    
    //? Update with new arguments when needed
    map<string, string> ret_map = {
        {"image", NULL},
        {"pixel_size", NULL},
        {"thresholds_type", NULL},
        {"thresholds_v", NULL}
    };

    string arg;
    for (int i=1; i<argc; ++i){

        arg = (string) argv[i];

        if (arg == "-h" || arg == "--help"){
            cout << 
                "Binary to demo the pics2bits library\n"
                "Accepted arguments:\n"
                "\t-i, --image : the  input image path\n"
                "\t-s, --pixelsize : the size of the pixel in the bitmap, one of {1, 2, 4}\n"
                "\t--thresholds-type : 'gray' if thresholds are grayscale values, 'color' if thresholds are triplets of BGR values\n"
                "\t-t, --thresholds : the list of threshold values to use in the bitmap, between 0 and 255, in a correct amount\n"
            << endl;
            exit(0);
        }
        
        if (arg == "-i" || arg == "--image"){
            ret_map["image"] = (string) argv[i+1];
        }

        if (arg == "-s" || arg == "--pixelsize"){
            ret_map["pixel_size"] = (string) argv[i+1];
        }

        if (arg == "--thresholds_type"){
            ret_map["thresholds_type"] = (string) argv[i+1];
        }

        if (arg == "-t" || arg == "--thresholds"){
            string th_s = "";
            int j = 1;
            while ((i+j < argc) && ((string)argv[i+j]).find_first_not_of("0123456789") == string::npos){
                string th_value = (string) argv[i+j];
                th_s.append(th_value+" ");
                ++j;
            }
            ret_map["thresholds_v"] = th_s;
        }

    }

    return ret_map;

}




// ----------------------------------------------------------------------

int main(int argc, char** argv){
    cout << "pics2bits - the coolest bitmaps in town" << endl;




    map<string, string> arg_map = parseArgs(argc, argv);

    string img_path = arg_map["image"];
    uint8_t pixel_size = (uint8_t) stoi(arg_map["pixel_size"]);
    string th_type = arg_map["thresholds_type"];
    if (th_type != "gray" && th_type != "color"){
        cout << "thresholds_type must be one of 'gray' or 'color'" << endl;
        exit(1);
    }

    if (th_type == "gray"){
        vector<uint8_t> thresholds_v = {};
        string tmp_val = "";
        for (char& c : arg_map["thresholds_v"]){
            if (c == ' '){
                //TODO
            }
            else {
                //TODO
            }
        }
    }
    
    else {
        vector<cv::Vec3b> thresholds_v = {};
        string tmp_val = "";
        for (char& c : arg_map["thresholds_v"]){
            if (c == ' '){
                //TODO
            }
            else {
                //TODO
            }
        }
    }













    return 0;
}