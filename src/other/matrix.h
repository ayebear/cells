// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef MATRIX_H
#define MATRIX_H

#include <memory>
#include <fstream>
#include <iostream>
#include <SFML/System/Vector2.hpp>

/*
This class is a wrapper around a single dimensional bool array so that it can be used like a 2D array.
The size can be changed dynamically on runtime in a non-destructive way.
This can also be serialized to/deserialized from a file, and packs the bits to not waste any space.
In the future, this might need to be a templated class again, so this will be kept in the header file.
*/
class BoolMatrix
{
    public:
        BoolMatrix() {}

        BoolMatrix(unsigned width, unsigned height)
        {
            resize(width, height);
        }

        BoolMatrix(const sf::Vector2u& newSize)
        {
            resize(newSize.x, newSize.y);
        }

        BoolMatrix& operator=(BoolMatrix& rhs)
        {
            if (this != &rhs)
            {
                matrixWidth = rhs.matrixWidth;
                matrixHeight = rhs.matrixHeight;
                matrixSize = rhs.matrixSize;
                data.reset(new bool[matrixSize]); // Allocate a new buffer
                for (unsigned i = 0; i < matrixSize; ++i) // Copy the data from the other matrix
                    data[i] = rhs.data[i];
            }
            return *this;
        }

        void resize(unsigned width, unsigned height, bool preserve = true)
        {
            // Only resize if the new size is different
            if (matrixWidth != width || matrixHeight != height)
            {
                unsigned newMatrixSize = width * height;
                if (data && preserve)
                {
                    // Preserve the original data by copying it
                    std::unique_ptr<bool[]> tempData(new bool[newMatrixSize]);
                    copyMatrix(data.get(), tempData.get(), matrixWidth, width, matrixHeight, height);
                    data = std::move(tempData);
                }
                else
                {
                    data.reset(new bool[newMatrixSize]); // Destructively create a new array
                    fillMatrix(data.get(), newMatrixSize, false); // Fill the array
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
            data.reset();
        }

        // You can get/set elements with these functions
        // Note that they are (x, y), which is (column, row)
        bool& operator()(unsigned x, unsigned y)
        {
            return data[(y * matrixWidth) + x];
        }

        bool& operator()(const sf::Vector2u& pos)
        {
            return data[(pos.y * matrixWidth) + pos.x];
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
            std::cout << "saveToFile(" << filename << ")\n";

            bool status = false;

            // Create a buffer for the file
            unsigned fileSize = (matrixSize / 8) + 8; // 8 extra bytes for width and height
            std::unique_ptr<char[]> fileData(new char[fileSize]);
            for (unsigned i = 0; i < fileSize; ++i)
                fileData[i] = 0; // Fill with zeros, which is necessary for packing the bits properly

            // Store the width and height in the buffer
            writeUintToChar(fileData.get(), matrixWidth);
            writeUintToChar(fileData.get() + 4, matrixHeight);

            // Pack the bits into the buffer
            for (unsigned i = 0; i < matrixSize; ++i)
                fileData[(i / 8) + 8] |= ((data[i] & 0x1) << (i % 8));

            // Write the buffer to the file
            std::ofstream outFile(filename.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
            if (outFile.is_open())
            {
                outFile.seekp(0, std::ios::beg);
                outFile.write(fileData.get(), fileSize);
                outFile.close();
                status = true;
            }
            return status;
        }

        bool loadFromFile(const std::string& filename)
        {
            std::cout << "loadFromFile(" << filename << ")\n";

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
                    std::unique_ptr<char[]> fileData(new char[fileSize]);

                    // Read the file into the buffer
                    inFile.seekg(0, std::ios::beg);
                    inFile.read(fileData.get(), fileSize);
                    inFile.close();

                    // Get the new matrix size and resize the matrix
                    unsigned newWidth = readUintFromChar(fileData.get());
                    unsigned newHeight = readUintFromChar(fileData.get() + 4);
                    resize(newWidth, newHeight, false);

                    std::cout << "newWidth = " << newWidth << ", newHeight = " << newHeight << "\n";

                    // Unpack the bits into the matrix
                    unsigned minSize = std::min(((fileSize - 8) * 8), matrixSize); // In case the file is larger or smaller
                    for (unsigned i = 0; i < minSize; ++i)
                        data[i] = ((fileData[(i / 8) + 8] >> (i % 8)) & 0x1);

                    // Fill the remaining part of the matrix if the file was not large enough
                    if (minSize < matrixSize)
                        for (unsigned i = minSize; i < matrixSize; ++i)
                            data[i] = false;

                    status = true;
                }
            }
            return status;
        }

    private:
        void copyMatrix(bool* source, bool* dest, unsigned sourceWidth, unsigned destWidth, unsigned sourceHeight, unsigned destHeight) const
        {
            unsigned height = std::min(sourceHeight, destHeight);
            unsigned width = std::min(sourceWidth, destWidth);
            // Fill data
            fillMatrix(dest, (destWidth * destHeight), false);
            // Copy data
            for (unsigned y = 0; y < height; ++y)
                for (unsigned x = 0; x < width; ++x)
                    dest[(y * destWidth) + x] = source[(y * sourceWidth) + x];
        }

        void fillMatrix(bool* dest, unsigned size, bool fill) const
        {
            for (unsigned i = 0; i < size; ++i)
                dest[i] = fill;
        }

        // This writes an unsigned int to a char array
        void writeUintToChar(char* dest, unsigned num) const
        {
            for (unsigned i = 0; i < 4; ++i)
                dest[i] = (num >> (i * 8)) & 0xff;
            // TODO: Make this endian-safe by using arithmetic instead of bitwise operations
        }

        // This reads a char array and returns an unsigned int
        unsigned readUintFromChar(const char* source) const
        {
            unsigned result = 0;
            for (unsigned i = 0; i < 4; ++i)
                result += (static_cast<unsigned>(source[i]) << (i * 8));
            return result;
        }

        std::unique_ptr<bool[]> data;
        unsigned matrixWidth;
        unsigned matrixHeight;
        unsigned matrixSize;
};

/*void printMatrix(BoolMatrix& matrix)
{
    for (unsigned y = 0; y < matrix.height(); ++y)
    {
        for (unsigned x = 0; x < matrix.width(); ++x)
            std::cout << matrix(x, y) << " ";
        std::cout << std::endl;
    }
}*/

#endif
