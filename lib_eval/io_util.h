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

#ifndef IO_UTIL_H
#define	IO_UTIL_H

#include <vector>
#include <map> 
#include <string>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>

#ifndef DIRECTORY_SEPARATOR
    #if defined(WIN32) || defined(_WIN32)
        #define DIRECTORY_SEPARATOR "\\"
    #else
        #define DIRECTORY_SEPARATOR "/"
    #endif
#endif

/** \brief I/O utilities.
 * \author David Stutz
 */
class IOUtil {
public:
    /** \brief Read an OpenCV matrix.
     * \param[in] file path to file to read
     * \param[out] mat matrix read
     * \param[in] field the field the matrix was saved in, default is "mat"
     * \return number of rows read
     */
    static int readMat(boost::filesystem::path file, cv::Mat &mat, std::string field = "mat");
    
    /** \brief Write a matrix to the given file.
     * 
     * Will overwrite the file if it already exists.
     * 
     * \param[in] file path to file to (over-) write
     * \param[in] mat matrix to write
     * \param[in] field the field the matrix was saved in, default is "mat"
     * \return number of rows written
     */
    static int writeMat(boost::filesystem::path file, const cv::Mat& mat, 
            std::string field = "mat");
    
    /** \brief Write two dimensional array to CSV file.
     * \param[in] file path to file to write
     * \param[in] array two-dimenstional array holding the data
     * \param[in] rows number of rows
     * \param[in] cols number of cols
     * \param[in] separator separator to use for CSV
     * \param[in] precision
     * \return number of rows read
     */
    template<typename T>
    static int writeArrayCSV(boost::filesystem::path file, T** array, int rows, int cols, 
            std::string separator = ",", int precision = 6);
    
    /** \brief Write the content of a cv::Mat to a CSV file.
     * 
     * Only works for cv::Mats with one channel!
     * 
     * \param[in] file path to file to write
     * \param[in] mat matrix to write
     * \param[in] separator separator to use for CSV
     * \param[in] precision
     * \return number of rows read
     */
    template<typename T>
    static int writeMatCSV(boost::filesystem::path file, const cv::Mat& mat, 
            std::string separator = ",", int precision = 6);
    
    /** \brief Read CSV file into matrix.
     * \param[in] file path to file
     * \param[out] result matrix read
     * \return number of rows read
     */
    static int readMatCSVInt(boost::filesystem::path file, cv::Mat &result);
    
    /** \brief Read CSV file into matrix.
     * \param[in] file path to file
     * \param[in] rows number of rows to read
     * \param[in] cols number of cols to read
     * \param[out] result matrix read
     * \return number of rows read
     */
    static int readMatCSVInt(boost::filesystem::path file, int rows, int cols, cv::Mat &result);
    
    /** \brief Read CSV file into matrix.
     * \param[in] file path to file
     * \param[out] result matrix read
     * \return number of rows read
     */
    static int readMatCSVFloat(boost::filesystem::path file, cv::Mat &result);
    
    /** \brief Read header of CSV file into string array.
     * \param[in] file path to file
     * \param[out] header header strings as vector
     */
    static int readCSVHeaderString(boost::filesystem::path file, std::vector<std::string> &header);
    
    /** \brief Read a CSV summary file as produced by EvaluationSummary.
     * \param[in] file path to summary file
     * \param[out] row_headers row names (i.e. first column)
     * \param[out] col_headers column names (i.e. first row)
     * \param[out] data data corresponding to the inner of the CSV file
     */
    static int readCSVSummary(boost::filesystem::path file, std::vector<std::string>& row_headers, 
            std::vector<std::string>& col_headers, cv::Mat &data);
    
    /** \brief Creates an ordered list of all subdirectories.
     * \param[in] directory directory to find subdirectories in
     * \param[out] subdirectores multimap of subdirectories found
     */
    static void listSubdirectories(boost::filesystem::path directory, 
            std::multimap<std::string, boost::filesystem::path> subdirectories);
    
    /** \brief Creates an ordered multimap of all files in the given directory.
     * 
     * Files must match one of the given extensions.
     * 
     * \param[in] directory directory to read
     * \param[in] extensions extensions to allow
     * \param[out] files files found in the directory
     * \param[in] prefix prefix to allow for files
     * \param[in] suffix suffix to allow for files
     * \param[in] exclude file names to exclude
     */
    static void readDirectory(boost::filesystem::path directory, 
        std::vector<std::string> extensions, 
        std::multimap<std::string, boost::filesystem::path> &files,
        std::string prefix = "", std::string suffix = "",
        std::vector<std::string> exclude = std::vector<std::string>{});
        
    /** \brief Creates an ordered multimap of all files in the given directory.
     * 
     * \param[in] directory directory to read
     * \param[out] files files found in the directory
     * \param[in] prefix prefix to allow for files
     * \param[in] suffix suffix to allow for files
     */
    static void readDirectory(boost::filesystem::path directory,
        std::multimap<std::string, boost::filesystem::path> &files,
        std::string prefix = "", std::string suffix = "");
    
    /** \brief Gets a vector containing common extensions for images
     * \param[out] extensions image extensions
     */
    static void getImageExtensions(std::vector<std::string> &extensions);
    
    /** \brief Gets a vector containing common extensions for text files.
     * \param[out] extensions txt extensions
     */
    static void getTxtExtensions(std::vector<std::string> &extensions);
    
    /** \brief Get a vector of common CSV extensions.
     * \param[out] extensions csv exnteions
     */
    static void getCSVExtensions(std::vector<std::string> &extensions);
    
};

#endif	/* IO_UTIL_H */
