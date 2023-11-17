#include "p2b/core.hpp"
#include "p2b/utils.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <filesystem>
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
        {"dir", ""},
        {"mode", ""},
        {"pixel_size", ""},
        {"resizing", ""},
        {"color",""}
    };

    string arg;
    for (int i=0; i<argc; ++i){

        arg = (string) argv[i];

        if (arg == "-h" || arg == "--help" || argc == 1){
            cout << 
                "Binary to demo the pics2bits library\n"
                "Accepted arguments:\n"
                "\t-i, --images : the  input image paths separated by spaces\n"
                "\t-d, --dir : the directory containing all input images to perform tests\n"
                    "\t\t(you can use the provided demo_pics directory)\n"
                "\t-m, --mode : the mode in which to manage additional images, one of {a, u}\n"
                    "\t\t(a = add, u = update), default = a\n"
                "\t-s, --pixelsize : the size of the pixel in the bitmap, one of {1, 2, 4}\n"
                    "\t\tdefault = 2\n"
                "\t-r, --resizing : boolean flag to use minimal resizing of the bitmap\n"
                    "\t\tdefault = false\n"
                "\t-c, --color : boolean flag to specify if output has to be shown in color\n"
                    "\t\tdefault = false\n"
            << endl;
            exit(0);
        }
        
        if (arg == "-i" || arg == "--image"){
            int inc = 1;
            while ((i+inc)<argc && ((string)argv[i+inc]).find_first_of('-') != 0){
                ret_map["image"] += (string) argv[i+inc] + " ";
                inc++;
            }
        }

        if (arg == "-d" || arg == "--dir"){
            ret_map["dir"] = (string) argv[i+1];
        }

        if (arg == "-m" || arg == "--mode"){
            ret_map["mode"] = (string) argv[i+1];
        }

        if (arg == "-s" || arg == "--pixelsize"){
            ret_map["pixel_size"] = (string) argv[i+1];
        }

        if (arg == "-r" || arg == "--resizing"){
            ret_map["resizing"] = "true";
        }

        if (arg == "-c" || arg == "--color"){
            ret_map["color"] = "true";
        }

    }

    return ret_map;

}








int aux_imshow(string window_name, cv::Mat img){
    cv::namedWindow(window_name,cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_EXPANDED);
    cv::resizeWindow(window_name,800, 800);
    cv::imshow(window_name,img);
    int key = cv::waitKey(0);
    cv::destroyAllWindows();
    return key;
}





//? The main testing routine
void aux_testRoutineGrayscale(
    Bitmap* bitmap_ptr,
    vector<string> images,
    cv::Mat* input_img_ptr, 
    char mode, 
    uint8_t pixel_size, 
    bool min_resizing, 
    const vector<uint8_t>& th_vector, 
    cv::Mat* output_img_ptr,
    const vector<uint8_t>& gs_palette,
    long* img2bmp_ptr,
    long* bmp2img_ptr
){
    *input_img_ptr = cv::imread(images[0]);
    aux_imshow("Input image", *input_img_ptr);

    auto start = chrono::high_resolution_clock::now();
    *bitmap_ptr = toBitmap(input_img_ptr, pixel_size, th_vector);
    auto end = chrono::high_resolution_clock::now();
    *img2bmp_ptr = (chrono::duration_cast<chrono::milliseconds>(end-start)).count();

    start = chrono::high_resolution_clock::now();
    bitmap_ptr->toGrayscaleImage_parallel(output_img_ptr, gs_palette);
    end = chrono::high_resolution_clock::now();
    *bmp2img_ptr = (chrono::duration_cast<chrono::milliseconds>(end-start)).count();

    aux_imshow("Grayscale output bitmap", *output_img_ptr);

    char msg[128] = "\0";
    const char* directions[4] = { "UP", "RIGHT", "DOWN", "LEFT" };
    int add_direction = p2b::DIR_UP;
    int key = -1;

    switch (mode) {
        case 'a':
            for (size_t i=1; i<images.size(); ++i){
                
                *input_img_ptr = cv::imread(images[i]);
                add_direction = (i-1)%4;
                snprintf(
                    msg, 128, 
                    "Image #%ld that will be added to bitmap (choose direction with arrow keys, default = %s)",
                    i, directions[add_direction]
                );
                key = aux_imshow(msg, *input_img_ptr);

                /*
                    ! CHANGE THIS WITH YOUR CORRECT ARROW KEY VALUES
                    ! A LINE OF CODE TO PRINT THIS VALUES CAN BE DECOMMENTED BELOW
                */
                //! cout<<"KEY = "<<key<<endl;

                switch (key) {
                    case 82: //? UP
                        add_direction = p2b::DIR_UP;
                        break;
                    case 83: //? RIGHT
                        add_direction = p2b::DIR_RIGHT;
                        break;    
                    case 84: //? DOWN
                        add_direction = p2b::DIR_DOWN;
                        break;
                    case 81: //? LEFT
                        add_direction = p2b::DIR_LEFT;
                        break;
                }
                
                start = chrono::high_resolution_clock::now();
                addBits(bitmap_ptr, input_img_ptr, add_direction, min_resizing);
                end = chrono::high_resolution_clock::now();

                snprintf(
                    msg, 128, 
                    "Time to add image #%ld = %ld ms",
                    i, (chrono::duration_cast<chrono::milliseconds>(end-start)).count()
                );
                cout << msg << endl;
                bitmap_ptr->toGrayscaleImage_parallel(output_img_ptr, gs_palette);
                aux_imshow("Resulting bitmap", *output_img_ptr);

            }
            break;

        case 'u':
            for (size_t i=1; i<images.size(); ++i){
                
                *input_img_ptr = cv::imread(images[i]);
                snprintf(
                    msg, 128, 
                    "Image #%ld that will update the bitmap",
                    i
                );
                aux_imshow(msg, *input_img_ptr);
                
                start = chrono::high_resolution_clock::now();
                updateBitmap(bitmap_ptr, input_img_ptr);
                end = chrono::high_resolution_clock::now();

                snprintf(
                    msg, 128, 
                    "Time to update bitmap with image #%ld = %ld ms",
                    i, (chrono::duration_cast<chrono::milliseconds>(end-start)).count()
                );
                cout << msg << endl;
                bitmap_ptr->toGrayscaleImage_parallel(output_img_ptr, gs_palette);
                aux_imshow("Resulting bitmap", *output_img_ptr);

            }
            break;

        default:
            ERROR_MSG("invalid mode, can only be 'a' or 'u'");
            exit(1);
    }

}




void aux_testRoutineColor(
    Bitmap* bitmap_ptr,
    vector<string> images,
    cv::Mat* input_img_ptr, 
    char mode, 
    uint8_t pixel_size, 
    bool min_resizing, 
    const vector<uint8_t>& th_vector, 
    cv::Mat* output_img_ptr,
    const vector<cv::Vec3b>& col_palette,
    long* img2bmp_ptr,
    long* bmp2img_ptr
){
    ERROR_MSG("color version of this demo still not implemented, sorry :(");
    exit(0);
}























// ----------------------------------------------------------------------

int main(int argc, char** argv){
    cout << "\npics2bits - the coolest bitmaps in town\n" << endl;

    //char dmsg[256] = "\0";    //? Used for debugging purposes


    map<string, string> arg_map = parseArgs(argc, argv);

    string img_paths = arg_map["image"];
    string dir_path = arg_map["dir"];
    if (img_paths == "" && dir_path == ""){
        ERROR_MSG("one of --image and --dir argument is required");
        exit(1);
    }
    if (img_paths != "" && dir_path != ""){
        cout << "Both --image and --dir arguments provided, only using --dir in this case" << endl;
    }


    vector<string> images = vector<string>();

    if (dir_path == ""){
        int del_i = img_paths.find(' '); 
        while (del_i != -1) { // Loop until no delimiter is left in the string.
            images.push_back(img_paths.substr(0, del_i));
            img_paths.erase(img_paths.begin(), img_paths.begin() + del_i + 1);
            del_i = img_paths.find(' ');
        }
    }
    else {
        for (const filesystem::directory_entry& file : filesystem::directory_iterator(dir_path)){
            images.push_back((string) file.path());
        }
        std::sort(
            images.begin(),
            images.end(),
            [](string a, string b){return a<b;}
        );
    }

    cout << "---- Input image paths ----" << endl;
    for (size_t i=0; i<images.size(); ++i){
        cout << "#" << i << " = " << images[i] << endl;
    }
    cout<<"\n"<<endl;



    char mode = (arg_map["mode"] != "") ? arg_map["mode"][0] : 'a';
    uint8_t pixel_size = (arg_map["pixel_size"]!="") ? (uint8_t) stoi(arg_map["pixel_size"]) : 2;
    bool min_resizing = (arg_map["resizing"]=="true") ? true : false;
    bool use_color = (arg_map["color"]=="true") ? true : false;



    //? Hard coded thresholds and palettes
    vector<uint8_t> th_vector_1b = {125};
    vector<uint8_t> gray_palette_1b = {255};
    vector<cv::Vec3b> col_palette_1b = {cv::Vec3b(255,255,255)};

    vector<uint8_t> th_vector_2b = {85, 170, 255};
    vector<uint8_t> gray_palette_2b = {85, 170, 255};
    vector<cv::Vec3b> col_palette_2b = {cv::Vec3b(0,0,255), cv::Vec3b(0,255,0), cv::Vec3b(255,0,0)};
    vector<vector<uint8_t>> col_palette_test = {
        {0,0,255},
        {0,255,0},
        {255,0,0}
    };

    vector<uint8_t> th_vector_4b = {17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};
    vector<uint8_t> gray_palette_4b = {17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};
    vector<cv::Vec3b> col_palette_4b = {
        cv::Vec3b(0,0,51), cv::Vec3b(0,0,102), cv::Vec3b(0,0,153), cv::Vec3b(0,0,204), cv::Vec3b(0,0,255),
        cv::Vec3b(0,51,0), cv::Vec3b(0,102,0), cv::Vec3b(0,153,0), cv::Vec3b(0,204,0), cv::Vec3b(0,255,0),
        cv::Vec3b(51,0,0), cv::Vec3b(102,0,0), cv::Vec3b(153,0,0), cv::Vec3b(204,0,0), cv::Vec3b(255,0,0)
    };


    //? tmp initializations
    cv::Mat input_img;
    cv::Mat out_img;
    Bitmap bmp;

    long t_img2bmp = -1;
    long t_bmp2img = -1;


    if (!use_color) {

        switch (pixel_size) {
            case 1:
                aux_testRoutineGrayscale(
                    &bmp, 
                    images, 
                    &input_img, 
                    mode,
                    pixel_size, 
                    min_resizing,
                    th_vector_1b, 
                    &out_img, 
                    gray_palette_1b, 
                    &t_img2bmp, 
                    &t_bmp2img
                );
                break;
            case 2:
                aux_testRoutineGrayscale(
                    &bmp, 
                    images, 
                    &input_img, 
                    mode,
                    pixel_size, 
                    min_resizing,
                    th_vector_2b, 
                    &out_img, 
                    gray_palette_2b, 
                    &t_img2bmp, 
                    &t_bmp2img
                );
                break;
            case 4:
                aux_testRoutineGrayscale(
                    &bmp, 
                    images, 
                    &input_img, 
                    mode,
                    pixel_size, 
                    min_resizing,
                    th_vector_4b, 
                    &out_img, 
                    gray_palette_4b, 
                    &t_img2bmp, 
                    &t_bmp2img
                );
                break;
        }

    }

    else {

        switch (pixel_size) {
            case 1:
                aux_testRoutineColor(
                    &bmp,
                    images,
                    &input_img, 
                    mode, 
                    pixel_size, 
                    min_resizing, 
                    th_vector_1b, 
                    &out_img,
                    col_palette_1b,
                    &t_img2bmp,
                    &t_bmp2img
                );
                break;
            case 2:
                aux_testRoutineColor(
                    &bmp,
                    images,
                    &input_img, 
                    mode, 
                    pixel_size, 
                    min_resizing, 
                    th_vector_2b, 
                    &out_img,
                    col_palette_2b,
                    &t_img2bmp,
                    &t_bmp2img
                );
                break;
            case 4:
                aux_testRoutineColor(
                    &bmp,
                    images,
                    &input_img, 
                    mode, 
                    pixel_size, 
                    min_resizing, 
                    th_vector_4b, 
                    &out_img,
                    col_palette_4b,
                    &t_img2bmp,
                    &t_bmp2img
                );
                break;
        }

    }








    if (mode == 'a'){
        size_t final_bmp_size = (
            bmp.getRows() * bmp.getCols() +
            bmp.getRows() * sizeof(bmp.getVec()[0]) +
            sizeof(bmp)
        );
        cout << "\nFinal bitmap size = " << final_bmp_size << " bytes" << endl;

        size_t potential_gsc_size = (
            bmp.getRows() * bmp.getCols()*(8/bmp.getPixelSize()) +
            bmp.getRows() * sizeof(out_img.at<uint8_t>(0)) +
            sizeof(out_img)
        );
        cout << "Potential corresponding grayscale image size = " << potential_gsc_size << " bytes\n" << endl;
    }


    //? Returning to original pic and bitmap to print correct metrics
    input_img = cv::imread(images[0]);
    switch (pixel_size) {
        case 1:
            bmp = toBitmap(&input_img, pixel_size, th_vector_1b);
            break;
        case 2:
            bmp = toBitmap(&input_img, pixel_size, th_vector_2b);
            break;
        case 4:
            bmp = toBitmap(&input_img, pixel_size, th_vector_4b);
            break;
    }



    PRINT_METRICS(input_img, bmp, t_img2bmp, t_bmp2img);
    input_img.release();
    out_img.release();
    
    return 0;
}