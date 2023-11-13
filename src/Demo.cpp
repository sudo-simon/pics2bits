#include "p2b/core.hpp"
#include "p2b/utils.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <map>
#include <opencv2/core/matx.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <vector>


using namespace p2b;
using namespace std;

// ----------------------------------------------------------------------

/*

    ! Very few checks on input are performed here, the correctness of use is left to the user.
    ! More precise checks are performed inside the library itself,
    ! but security of final implementation must be enforced.

*/



map<string, string> parseArgs(int argc, char** argv){
    
    //? Update with new arguments when needed
    map<string, string> ret_map = {
        {"image", ""},
        {"pixel_size", ""},
        //{"thresholds", ""},
        {"color",""}
        //{"palette", ""}
    };

    string arg;
    for (int i=0; i<argc; ++i){

        arg = (string) argv[i];

        if (arg == "-h" || arg == "--help" || argc == 1){
            cout << 
                "Binary to demo the pics2bits library\n"
                "Accepted arguments:\n"
                "\t-i, --image : the  input image path\n"
                "\t-s, --pixelsize : the size of the pixel in the bitmap, one of {1, 2, 4}\n"
                //"\t-t, --thresholds : the list of threshold values to use in the bitmap, between 0 and 255, in the correct amount (2**pixel_size - 1)\n"
                "\t-c, --color : boolean flag to specify if output has to be shown in color\n"
                //"\t-p, --palette : the list of colors to use to paint the bitmap, in the form g g g ... or (b,g,r) (b,g,r) (b,g,r) ..."
            << endl;
            exit(0);
        }
        
        if (arg == "-i" || arg == "--image"){
            ret_map["image"] = (string) argv[i+1];
        }

        if (arg == "-s" || arg == "--pixelsize"){
            ret_map["pixel_size"] = (string) argv[i+1];
        }

        /*
        if (arg == "-t" || arg == "--thresholds"){
            string th_s = "";
            int j = 1;
            while ((i+j < argc) && ((string)argv[i+j]).find_first_not_of("0123456789") == string::npos){
                string th_value = (string) argv[i+j];
                th_s.append(th_value+" ");
                ++j;
            }
            ret_map["thresholds"] = th_s;
        }
        */

        if (arg == "-c" || arg == "--color"){
            ret_map["color"] = "true";
        }

        /*
        if (arg == "-p" || arg == "--palette"){
            string palette = "";
            int j = 1;
            while ((i+j < argc) && ( ((string)argv[i+j]).find_first_not_of("0123456789") == string::npos || ((string)argv[i+j]).find('(') != string::npos )){
                string color = (string) argv[i+j];
                palette.append(color+" ");
                ++j;
            }
            ret_map["palette"] = palette;
        }
        */

    }

    return ret_map;

}



void aux_imshow(string window_name, cv::Mat img){
    cv::namedWindow(window_name,cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_EXPANDED);
    cv::resizeWindow(window_name,800, 800);
    cv::imshow(window_name,img);
    cv::waitKey(0);
    cv::destroyAllWindows();
}






// ----------------------------------------------------------------------

int main(int argc, char** argv){
    cout << "pics2bits - the coolest bitmaps in town\n" << endl;

    //char dmsg[256] = "\0";    //? Used for debugging purposes


    map<string, string> arg_map = parseArgs(argc, argv);

    string img_path = arg_map["image"];
    if (img_path == ""){
        ERROR_MSG("input image needed");
        exit(1);
    }

    uint8_t pixel_size = (arg_map["pixel_size"]!="") ? (uint8_t) stoi(arg_map["pixel_size"]) : 2;

    bool use_color = (arg_map["color"]=="true") ? true : false;


    /*
    string tmp_val = "";
    vector<uint8_t> thresholds_v;
    for (char& c : arg_map["thresholds"]){
        if (c == ' '){
            uint8_t th = (uint8_t) stoi(tmp_val);
            thresholds_v.push_back(th);
            tmp_val = "";
        }
        else {
            tmp_val += c;
        }
    }

    tmp_val = "";
    bool coloredPalette = (arg_map["palette"].find('(') == string::npos) ? false : true;
    vector<uint8_t> grayscale_palette;
    vector<cv::Vec3b> color_palette;
    for (char& c : arg_map["palette"]){
        if (!coloredPalette){

            if (c == ' '){
                uint8_t color = (uint8_t) stoi(tmp_val);
                grayscale_palette.push_back(color);
                tmp_val = "";
            }
            else{
                tmp_val += c;
            }

        }
        else{

            if (c == ' '){
                vector<uint8_t> tmp_vec;
                string tmp_bgr = "";
                for (char& val_c : tmp_val){
                    if (val_c == ','){
                        tmp_vec.push_back((uint8_t)stoi(tmp_bgr));
                        tmp_bgr = "";
                        continue;
                    }
                    if (((string)"0123456789").find(val_c) != string::npos)
                        tmp_bgr += val_c;
                }
                cv::Vec3b color = cv::Vec3b(tmp_vec[0], tmp_vec[1], tmp_vec[2]);
                color_palette.push_back(color);
                tmp_vec.clear();
                tmp_val = "";
            }
            else{
                tmp_val += c;
            }

        }
    }
    */



    //? Hard coded params
    vector<uint8_t> th_vector_1b = {125};
    vector<uint8_t> gray_palette_1b = {255};
    vector<cv::Vec3b> col_palette_1b = {cv::Vec3b(255,255,255)};

    vector<uint8_t> th_vector_2b = {85, 170, 255};
    vector<uint8_t> gray_palette_2b = {85, 170, 255};
    vector<cv::Vec3b> col_palette_2b = {cv::Vec3b(0,0,255), cv::Vec3b(0,255,0), cv::Vec3b(255,0,0)};
    
    vector<uint8_t> th_vector_4b = {17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};
    vector<uint8_t> gray_palette_4b = {17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};
    vector<cv::Vec3b> col_palette_4b = {
        cv::Vec3b(0,0,51), cv::Vec3b(0,0,102), cv::Vec3b(0,0,153), cv::Vec3b(0,0,204), cv::Vec3b(0,0,255),
        cv::Vec3b(0,51,0), cv::Vec3b(0,102,0), cv::Vec3b(0,153,0), cv::Vec3b(0,204,0), cv::Vec3b(0,255,0),
        cv::Vec3b(51,0,0), cv::Vec3b(102,0,0), cv::Vec3b(153,0,0), cv::Vec3b(204,0,0), cv::Vec3b(255,0,0)
    };



    cv::Mat input_img = cv::imread(img_path);
    aux_imshow("Input image", input_img);
    
    cv::Mat out_img;
    Bitmap bmp; //? tmp initialization

    auto start = chrono::high_resolution_clock::now();
    auto end = chrono::high_resolution_clock::now();
    long t_img2bmp = -1;
    long t_bmp2img = -1;


    if (!use_color) {
        switch (pixel_size) {
            case 1:
                start = chrono::high_resolution_clock::now();
                bmp = toBitmap(input_img, pixel_size, th_vector_1b);
                end = chrono::high_resolution_clock::now();
                t_img2bmp = (chrono::duration_cast<chrono::milliseconds>(end-start)).count();

                start = chrono::high_resolution_clock::now();
                bmp.toGrayscaleImage_parallel(&out_img, gray_palette_1b);
                end = chrono::high_resolution_clock::now();
                t_bmp2img = (chrono::duration_cast<chrono::milliseconds>(end-start)).count();
                break;
            case 2:
                start = chrono::high_resolution_clock::now();
                bmp = toBitmap(input_img, pixel_size, th_vector_2b);
                end = chrono::high_resolution_clock::now();
                t_img2bmp = (chrono::duration_cast<chrono::milliseconds>(end-start)).count();

                start = chrono::high_resolution_clock::now();
                bmp.toGrayscaleImage_parallel(&out_img, gray_palette_2b);
                end = chrono::high_resolution_clock::now();
                t_bmp2img = (chrono::duration_cast<chrono::milliseconds>(end-start)).count();
                break;
            case 4:
                start = chrono::high_resolution_clock::now();
                bmp = toBitmap(input_img, pixel_size, th_vector_4b);
                end = chrono::high_resolution_clock::now();
                t_img2bmp = (chrono::duration_cast<chrono::milliseconds>(end-start)).count();

                start = chrono::high_resolution_clock::now();
                bmp.toGrayscaleImage_parallel(&out_img, gray_palette_4b);
                end = chrono::high_resolution_clock::now();
                t_bmp2img = (chrono::duration_cast<chrono::milliseconds>(end-start)).count();
                break;
        }

        aux_imshow("Grayscale output bitmap", out_img);
        out_img.release();
    }

    else {
        switch (pixel_size) {
            case 1:
                start = chrono::high_resolution_clock::now();
                bmp = toBitmap(input_img, pixel_size, th_vector_1b);
                end = chrono::high_resolution_clock::now();
                t_img2bmp = (chrono::duration_cast<chrono::milliseconds>(end-start)).count();

                start = chrono::high_resolution_clock::now();
                bmp.toBGRImage_parallel(&out_img, col_palette_1b);
                end = chrono::high_resolution_clock::now();
                t_bmp2img = (chrono::duration_cast<chrono::milliseconds>(end-start)).count();
                break;
            case 2:
                start = chrono::high_resolution_clock::now();
                bmp = toBitmap(input_img, pixel_size, th_vector_2b);
                end = chrono::high_resolution_clock::now();
                t_img2bmp = (chrono::duration_cast<chrono::milliseconds>(end-start)).count();

                start = chrono::high_resolution_clock::now();
                bmp.toBGRImage_parallel(&out_img, col_palette_2b);
                end = chrono::high_resolution_clock::now();
                t_bmp2img = (chrono::duration_cast<chrono::milliseconds>(end-start)).count();
                break;
            case 4:
                start = chrono::high_resolution_clock::now();
                bmp = toBitmap(input_img, pixel_size, th_vector_4b);
                end = chrono::high_resolution_clock::now();
                t_img2bmp = (chrono::duration_cast<chrono::milliseconds>(end-start)).count();

                start = chrono::high_resolution_clock::now();
                bmp.toBGRImage_parallel(&out_img, col_palette_4b);
                end = chrono::high_resolution_clock::now();
                t_bmp2img = (chrono::duration_cast<chrono::milliseconds>(end-start)).count();
                break;
        }
        
        aux_imshow("Color output bitmap", out_img);
        out_img.release();
    }



   





    PRINT_METRICS(input_img, bmp, t_img2bmp, t_bmp2img);
    
    return 0;
}