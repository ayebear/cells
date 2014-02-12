// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <fstream>
#include <iostream>
#include <SFML/System/Vector2.hpp>

/*
This class is a wrapper around a single dimensional array so that it can be used like a 2D array.
The size can be changed dynamically on runtime in a non-destructive way.
This can also be serialized to/deserialized from a file.
*/
template <class Type>
class Matrix
{
    public:
        Matrix()
        {
            clear();
        }

        Matrix(unsigned width, unsigned height)
        {
            resize(width, height);
        }

        Matrix(const sf::Vector2u& newSize)
        {
            resize(newSize.x, newSize.y);
        }

        void resize(unsigned width, unsigned height, bool preserve = true)
        {
            // Only resize if the new size is different
            if (matrixWidth != width || matrixHeight != height)
            {
                unsigned newMatrixSize = width * height;
                if (!elements.empty() && preserve)
                {
                    // Preserve the original data by copying it
                    std::vector<Type> tempElements(newMatrixSize);
                    copyMatrix(elements, tempElements, matrixWidth, width, matrixHeight, height);
                    elements.swap(tempElements);
                }
                else
                {
                    elements.clear();
                    elements.resize(newMatrixSize, 0);
                }
                matrixWidth = width;
                matrixHeight = height;
                matrixSize = newMatrixSize;
            }
        }

        void resize(const sf::Vector2u& newSize, bool preserve = true)
        {
            resize(newSize.x, newSize.y, preserve);
        }

        // Removes all of the elements, so the size is 0x0
        void clear()
        {
            matrixWidth = 0;
            matrixHeight = 0;
            matrixSize = 0;
            elements.clear();
            elements.shrink_to_fit();
        }

        // You can get/set elements with these functions
        // Note that they are (x, y), which is (column, row)
        Type& operator()(unsigned x, unsigned y)
        {
            return elements[(y * matrixWidth) + x];
        }

        Type& operator()(const sf::Vector2u& pos)
        {
            return elements[(pos.y * matrixWidth) + pos.x];
        }

        unsigned width() const
        {
            return matrixWidth;
        }

        unsigned height() const
        {
            return matrixHeight;
        }

        unsigned size() const
        {
            return matrixSize;
        }

        bool saveToFile(const std::string& filename) const
        {
            // std::cout << "saveToFile(" << filename << ")\n";

            bool status = false;

            // Create a buffer for the file
            unsigned fileSize = (matrixSize / 8) + 8; // 8 extra bytes for width and height
            std::vector<char> fileData(fileSize, 0);

            // Store the width and height in the buffer
            // std::cout << "Writing " << matrixWidth << " x " << matrixHeight << " to buffer\n";
            writeUintToString(fileData.data(), matrixWidth);
            writeUintToString(fileData.data() + 4, matrixHeight);

            // Pack the bits into the buffer
            for (unsigned i = 0; i < matrixSize; ++i)
                fileData[(i / 8) + 8] |= (((elements[i] != 0) & 0x1) << (i % 8));

            // Write the buffer to the file
            std::ofstream outFile(filename.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
            if (outFile.is_open())
            {
                outFile.seekp(0, std::ios::beg);
                outFile.write(fileData.data(), fileSize);
                outFile.close();
                status = true;
            }
            return status;
        }

        bool loadFromFile(const std::string& filename)
        {
            // std::cout << "loadFromFile(" << filename << ")\n";

            // Deserialize from a file. Note that this will change the size of the matrix
            // in memory to whatever was specified in the file.
            bool status = false;

            std::ifstream inFile;
            inFile.open(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
            if (inFile.is_open())
            {
                // Get the file size
                unsigned fileSize = inFile.tellg();
                if (fileSize > 8)
                {
                    // Allocate a buffer for the file
                    std::vector<char> fileData(fileSize, 0);

                    // Read the file into the buffer
                    inFile.seekg(0, std::ios::beg);
                    inFile.read(fileData.data(), fileSize);
                    inFile.close();

                    // Get the new matrix size and resize the matrix
                    unsigned newWidth = readUintFromString(fileData.data());
                    unsigned newHeight = readUintFromString(fileData.data() + 4);
                    resize(newWidth, newHeight, false);

                    // std::cout << "loadFromFile(): newWidth = " << newWidth << ", newHeight = " << newHeight << "\n";

                    // Unpack the bits into the matrix
                    unsigned minSize = std::min(((fileSize - 8) * 8), matrixSize); // In case the file is larger or smaller
                    for (unsigned i = 0; i < minSize; ++i)
                        elements[i] = ((fileData[(i / 8) + 8] >> (i % 8)) & 0x1);

                    // Fill the remaining part of the matrix if the file was not large enough
                    if (minSize < matrixSize)
                        for (unsigned i = minSize; i < matrixSize; ++i)
                            elements[i] = false;

                    status = true;
                }
            }
            return status;
        }

    private:
        void copyMatrix(const std::vector<Type>& source, std::vector<Type>& dest, unsigned sourceWidth, unsigned destWidth, unsigned sourceHeight, unsigned destHeight) const
        {
            unsigned height = std::min(sourceHeight, destHeight);
            unsigned width = std::min(sourceWidth, destWidth);
            // Fill data
            dest.clear();
            dest.resize(destWidth * destHeight, 0);
            // Copy data
            for (unsigned y = 0; y < height; ++y)
                for (unsigned x = 0; x < width; ++x)
                    dest[(y * destWidth) + x] = source[(y * sourceWidth) + x];
        }

        // This writes an unsigned int to a char array
        void writeUintToString(char* dest, unsigned num) const
        {
            for (unsigned i = 0, p = 1; i < 4; ++i, p *= 256)
                dest[i] = (num / p) & 0xff;
        }

        // This reads a char array and returns an unsigned int
        unsigned readUintFromString(const char* source) const
        {
            unsigned result = 0;
            for (unsigned i = 0, p = 1; i < 4; ++i, p *= 256)
                result += static_cast<unsigned char>(source[i]) * p;
            return result;
        }

        std::vector<Type> elements;
        unsigned matrixWidth;
        unsigned matrixHeight;
        unsigned matrixSize;
};

#endif
