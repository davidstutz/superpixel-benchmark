/**
 * Copyright (c) 2016, David Stutz
 * Contact: david.stutz@rwth-aachen.de, davidstutz.de
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <iomanip>
#include <fstream>
#include <glog/logging.h>
#include "io_util.h"

////////////////////////////////////////////////////////////////////////////////
// readMat
////////////////////////////////////////////////////////////////////////////////

int IOUtil::readMat(boost::filesystem::path file, cv::Mat &mat, std::string field) {
    LOG_IF(FATAL, !boost::filesystem::is_regular_file(file)) 
            << "File does not exist: " << file.string() << ".";
    LOG_IF(FATAL, field.empty()) << "Cannot write to empty field using cv::FileStorage.";
    
    cv::FileStorage fs(file.string(), cv::FileStorage::READ);
    
    fs[field] >> mat;
    
    fs.release();
    return mat.rows;
}

////////////////////////////////////////////////////////////////////////////////
// writeMat
////////////////////////////////////////////////////////////////////////////////

int IOUtil::writeMat(boost::filesystem::path file, const cv::Mat& mat, std::string field) {
    LOG_IF(FATAL, mat.empty()) << "Matrix is empty.";
    LOG_IF(FATAL, field.empty()) << "Cannot read from empty field using cv::FileStorage.";
    
    cv::FileStorage fs(file.string(), cv::FileStorage::WRITE);
    fs << field << mat;
    
    fs.release();
    return mat.rows;
}

////////////////////////////////////////////////////////////////////////////////
// writeArrayCSV
////////////////////////////////////////////////////////////////////////////////

template<typename T>
int IOUtil::writeArrayCSV(boost::filesystem::path file, T** array, int rows, int cols, 
        std::string separator, int precision) {
    
    std::ofstream file_stream(file.c_str());
    for (int i = 0; i < rows; i ++) {
        for (int j = 0; j < cols; j++) {
            file_stream << std::setprecision(precision) << array[i][j];
            
            if (j < cols - 1) {
                file_stream << ",";
            }
        }
        
        file_stream << "\n";
    }
    
    file_stream.close();
    return rows;
}

template int IOUtil::writeArrayCSV<int>(boost::filesystem::path, int**, int, int, 
        std::string, int);
template int IOUtil::writeArrayCSV<float>(boost::filesystem::path, float**, int, int, 
        std::string, int);
template int IOUtil::writeArrayCSV<unsigned char>(boost::filesystem::path, unsigned char**, 
        int, int, std::string, int);

////////////////////////////////////////////////////////////////////////////////
// writeMatCSV
////////////////////////////////////////////////////////////////////////////////

template<typename T>
int IOUtil::writeMatCSV(boost::filesystem::path file, const cv::Mat& mat, 
        std::string separator, int precision) {
    
    LOG_IF(FATAL, mat.channels() != 1) << "Can only write one-channel matrices to CSV file.";
    LOG_IF(FATAL, precision <= 0) << "Invalid precision.";
    LOG_IF(FATAL, separator.empty()) << "Cannot use empty separator.";
    
    std::ofstream file_stream(file.c_str());
    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++) {
            file_stream << std::setprecision(precision) << mat.at<T>(i, j);
            
            if (j < mat.cols - 1) {
                file_stream << separator;
            }
        }
        
        if (i < mat.rows  - 1) {
            file_stream << "\n";
        }
    }
    
    file_stream.close();
    return mat.rows;
}

template int IOUtil::writeMatCSV<int>(boost::filesystem::path, 
        const cv::Mat&, std::string, int);
template int IOUtil::writeMatCSV<float>(boost::filesystem::path, 
        const cv::Mat&, std::string, int);
template int IOUtil::writeMatCSV<unsigned char>(boost::filesystem::path, 
        const cv::Mat&, std::string, int);

////////////////////////////////////////////////////////////////////////////////
// listSubdirectories
////////////////////////////////////////////////////////////////////////////////

void IOUtil::listSubdirectories(boost::filesystem::path directory,
        std::multimap<std::string, boost::filesystem::path> subdirectories) {
    
    LOG_IF(FATAL, !boost::filesystem::is_directory(directory)) 
            << "Directory does not exist: " << directory.string() << ".";
    
    subdirectories.clear();
    boost::filesystem::directory_iterator end;
    
    for (boost::filesystem::directory_iterator it(directory); it != end; ++it) {
        if (boost::filesystem::is_directory(it->path())) {
            subdirectories.insert(std::multimap<std::string, boost::filesystem::path>::value_type(it->path().string(), it->path()));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// readDirectory
////////////////////////////////////////////////////////////////////////////////

void IOUtil::readDirectory(boost::filesystem::path directory,
        std::multimap<std::string, boost::filesystem::path> &files,
        std::string prefix, std::string suffix) {
    
    LOG_IF(FATAL, !boost::filesystem::is_directory(directory)) 
            << "Directory does not exist: " << directory.string() << ".";
    
    files.clear();
    boost::filesystem::directory_iterator end;
    
    for (boost::filesystem::directory_iterator it(directory); it != end; ++it) {
        
        bool correct = true;
        std::string found_prefix = it->path().stem().string().substr(0, prefix.length());
        
        if (found_prefix != prefix) {
            correct = false;
        }
        
        int length = it->path().stem().string().length();
        if (length < suffix.length()) {
            correct = false;
        }
        else {
            std::string found_suffix = it->path().stem().string().substr(length - suffix.length(), suffix.length());

            if (found_suffix != suffix) {
                correct = false;
            }
        }
        
        if (correct) {
            files.insert(std::multimap<std::string, boost::filesystem::path>::value_type(it->path().string(), it->path()));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// readDirectory
////////////////////////////////////////////////////////////////////////////////

void IOUtil::readDirectory(boost::filesystem::path directory, 
        std::vector<std::string> extensions, 
        std::multimap<std::string, boost::filesystem::path> &files,
        std::string prefix, std::string suffix,
        std::vector<std::string> exclude) {
    
    LOG_IF(FATAL, !boost::filesystem::is_directory(directory)) 
            << "Directory does not exist: " << directory.string() << ".";
    LOG_IF(FATAL, extensions.empty()) << "No allowed extensions given.";
    
    files.clear();
    boost::filesystem::directory_iterator end;
    
    for (boost::filesystem::directory_iterator it(directory); it != end; ++it) {
        
        // Check extensions.
        bool correct = false;
        for (std::vector<std::string>::iterator ex = extensions.begin(); 
                ex != extensions.end(); ++ex) {
            
            if (*ex == it->path().extension().string()) {
                correct = true;
            }
        }
        
        for (std::vector<std::string>::iterator ex = exclude.begin(); 
                ex != exclude.end(); ex++) {
            
            if (it->path().stem().string().find(*ex) != std::string::npos) {
                correct = false;
            }
        }
        
        std::string found_prefix = it->path().stem().string().substr(0, prefix.length());
        if (found_prefix != prefix) {
            correct = false;
        }
        
        int length = it->path().stem().string().length();
        if (length < suffix.length()) {
            correct = false;
        }
        else {
            std::string found_suffix = it->path().stem().string().substr(length - suffix.length(), suffix.length());

            if (found_suffix != suffix) {
                correct = false;
            }
        }
        
        if (correct) {
            files.insert(std::multimap<std::string, boost::filesystem::path>::value_type(it->path().string(), it->path()));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// getImageExtensions
////////////////////////////////////////////////////////////////////////////////

void IOUtil::getImageExtensions(std::vector<std::string> &extensions) {
    extensions.clear();
    extensions.push_back(".png");
    extensions.push_back(".jpg");
    extensions.push_back(".jpeg");
    extensions.push_back(".bmp");
    extensions.push_back(".PNG");
    extensions.push_back(".JPG");
    extensions.push_back(".JPEG");
    extensions.push_back(".BMP");
}

////////////////////////////////////////////////////////////////////////////////
// getTxtExtensions
////////////////////////////////////////////////////////////////////////////////

void IOUtil::getTxtExtensions(std::vector<std::string> &extensions) {
    extensions.clear();
    extensions.push_back(".txt");
    extensions.push_back(".TXT");
}

////////////////////////////////////////////////////////////////////////////////
// getCSVExtensions
////////////////////////////////////////////////////////////////////////////////

void IOUtil::getCSVExtensions(std::vector<std::string> &extensions) {
    extensions.clear();
    extensions.push_back(".csv");
    extensions.push_back(".CSV");
}

////////////////////////////////////////////////////////////////////////////////
// readMatCSVInt
////////////////////////////////////////////////////////////////////////////////

int IOUtil::readMatCSVInt(boost::filesystem::path file, cv::Mat &result) {
    LOG_IF(FATAL, !boost::filesystem::is_regular_file(file)) 
            << "File does not exist: " << file.string() << ".";
    
    std::ifstream file_stream(file.c_str());
    
    int i = 0;
    int cols = 0; // To be determined.
    
    std::string line;
    while (std::getline(file_stream, line, '\n')) {
        
        std::stringstream line_stream(line);
        std::string cell;
        
        std::vector<std::string> data;
        while(std::getline(line_stream, cell, ',')) {
            data.push_back(cell);
        }
        
        // Intialize output matrix.
        if (i == 0) {
            cols = data.size();
            result.create(0, cols, CV_32SC1);
            
//            LOG(INFO) << "Reading CSV file with " << cols << " columns (" 
//                    << file.string() << ").";
        }
        
        LOG_IF (FATAL, data.size() != cols) << "Invalid CSV file: " << cols << "!=" 
                << data.size() << " " << i << "(" << file.string() << ").";;
        
        cv::Mat result_row(1, cols, CV_32SC1, cv::Scalar(0));
    
        for (int j = 0; j < cols; j++) {
            result_row.at<int>(0, j) = atoi(data[j].c_str());
        }
        
        result.push_back(result_row);
        i++;
    }
    
//    LOG(INFO) << "Read CSV file with " << i << " rows (" 
//            << file.string() << ").";
    
    file_stream.close();
    
    return i;
}

////////////////////////////////////////////////////////////////////////////////
// readMatCSVFloat
////////////////////////////////////////////////////////////////////////////////

int IOUtil::readMatCSVFloat(boost::filesystem::path file, cv::Mat &result) {
    LOG_IF(FATAL, !boost::filesystem::is_regular_file(file)) 
            << "File does not exist: " << file.string() << ".";
    
    std::ifstream file_stream(file.c_str());
    
    int i = 0;
    int cols = 0; // To be determined.
    
    std::string line;
    while (std::getline(file_stream, line, '\n')) {
        
        std::stringstream line_stream(line);
        std::string cell;
        
        std::vector<std::string> data;
        while(std::getline(line_stream, cell, ',')) {
            data.push_back(cell);
        }
        
        // Intialize output matrix.
        if (i == 0) {
            cols = data.size();
            result.create(0, cols, CV_32SC1);
            
//            LOG(INFO) << "Reading CSV file with " << cols << " columns (" 
//                    << file.string() << ").";
        }
        
        LOG_IF (FATAL, data.size() != cols) << "Invalid CSV file: " << cols << "!=" 
                << data.size() << " " << i << "(" << file.string() << ").";;
        
        cv::Mat result_row(1, cols, CV_32SC1, cv::Scalar(0));
    
        for (int j = 0; j < cols; j++) {
            result_row.at<float>(0, j) = atof(data[j].c_str());
        }
        
        result.push_back(result_row);
        i++;
    }
    
//    LOG(INFO) << "Read CSV file with " << i << " rows (" 
//            << file.string() << ").";
    
    file_stream.close();
    
    return i;
}

////////////////////////////////////////////////////////////////////////////////
// readCSVHeaderString
////////////////////////////////////////////////////////////////////////////////

int IOUtil::readCSVHeaderString(boost::filesystem::path file, std::vector<std::string> &header) {
    LOG_IF(FATAL, !boost::filesystem::is_regular_file(file)) 
            << "File does not exist: " << file.string() << ".";
    
    std::ifstream file_stream(file.c_str());
    
    std::string line;
    std::getline(file_stream, line, '\n');
        
    std::stringstream line_stream(line);
    std::string cell;

    header.clear();
    while(std::getline(line_stream, cell, ',')) {
        header.push_back(cell);
    }
    
    file_stream.close();
    
    return header.size();
}

////////////////////////////////////////////////////////////////////////////////
// readCSVSummary
////////////////////////////////////////////////////////////////////////////////

int IOUtil::readCSVSummary(boost::filesystem::path file, std::vector<std::string>& row_headers, 
        std::vector<std::string>& col_headers, cv::Mat &result) {

    LOG_IF(FATAL, !boost::filesystem::is_regular_file(file)) 
            << "File does not exist: " << file.string() << ".";
    
    std::ifstream file_stream(file.c_str());
    
    int i = 0;
    int cols = 0; // To be determined.
    
    std::string line;
    while (std::getline(file_stream, line, '\n')) {
        
        std::stringstream line_stream(line);
        std::string cell;
        
        std::vector<std::string> data;
        while(std::getline(line_stream, cell, ',')) {
            data.push_back(cell);
        }
        
        // Intialize output matrix.
        if (i == 0) {
            cols = data.size() - 1;
            result.create(0, cols, CV_32SC1);
            
//            LOG(INFO) << "Reading CSV file with " << cols << " columns (" 
//                    << file.string() << ").";
            
            for (unsigned int k = 0; k < data.size(); k++) {
                col_headers.push_back(data[k]);
            }
        }
        
        LOG_IF (FATAL, data.size() != cols + 1) << "Invalid CSV file: " << cols << "!=" 
                << data.size() << " " << i << "(" << file.string() << ").";;
        
        cv::Mat result_row(1, cols, CV_32SC1, cv::Scalar(0));
    
        row_headers.push_back(data[0]);
        if (i > 0) {
            for (int j = 1; j < cols; j++) {
                result_row.at<float>(0, j - 1) = atof(data[j].c_str());
            }
            
            result.push_back(result_row);
        }
        
        i++;
    }
    
    LOG_IF(FATAL, col_headers.size() != result.cols + 1)
            << "Read invalid number of columns.";
    LOG_IF(FATAL, row_headers.size() != result.rows + 1)
            << "Read invalid number of rows.";
//    LOG(INFO) << "Read CSV file with " << i << " rows (" 
//            << file.string() << ").";
    
    file_stream.close();
    
    return i;
}