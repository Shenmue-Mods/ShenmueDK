#pragma once

#include <stdint.h>
#include <cstring>
#include <algorithm>
#include <limits>

#include <Eigen/Dense>

#include "shendk/utils/math.h"
#include "shendk/types/image.h"
#include "shendk/files/image/pvr/twiddle.h"

namespace shendk {
namespace pvr {

// TODO: cleanup parameter mess (blockWidth, blockHeight)
// TODO: add dynamic pixel format (RGBA, RGB)

struct VectorQuantizer {

    struct VQBlock {

        VQBlock()
            : pixels(Eigen::MatrixXf(4, 4))
        {}

        VQBlock(uint8_t blockSize, uint8_t pixelSize)
            : pixels(Eigen::MatrixXf(blockSize, pixelSize))
        {}

        inline void zero() {
            pixels = pixels.Zero(pixels.rows(), pixels.cols());
        }

        inline void add(VQBlock& block) {
            pixels += block.pixels;
        }

        inline void div(int group) {
            pixels /= group;
        }

        inline float distance(const VQBlock& block) const {
            return std::abs((pixels - block.pixels).sum());
        }

        uint8_t* toArrayTwiddled() {
            // TODO: only works with RGBA blocks
            uint8_t* array = new uint8_t[pixels.rows() * 4];
            uint64_t destinationIndex = 0;
            int64_t sourceIndex = 0;
            uint64_t* twiddleMap = createTwiddleMap(pixels.rows());
            uint32_t blockHeight, blockWidth;
            blockHeight = blockWidth = pixels.rows() / 2;
            for (uint32_t y = 0; y < blockHeight; y++) {
                for (uint32_t x = 0; x < blockWidth; x++) {
                    destinationIndex = (twiddleMap[x] << 1) | twiddleMap[y];
                    array[destinationIndex * 4]     = static_cast<uint8_t>(pixels(sourceIndex));
                    array[destinationIndex * 4 + 1] = static_cast<uint8_t>(pixels(sourceIndex + 1));
                    array[destinationIndex * 4 + 2] = static_cast<uint8_t>(pixels(sourceIndex + 2));
                    array[destinationIndex * 4 + 3] = static_cast<uint8_t>(pixels(sourceIndex + 3));
                    sourceIndex += 4;
                }
            }
            delete[] twiddleMap;
            return array;
        }

        uint8_t* toArray() {
            Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> matrix = pixels.cast<uint8_t>();
            uint8_t* array = new uint8_t(pixels.size());
            memcpy(array, matrix.data(), pixels.size());
            return array;
            return nullptr;
        }

        uint32_t group = 0;
        Eigen::MatrixXf pixels;
    };

    static uint8_t* quantizeImage(Image& image, uint32_t codeBookSize, uint8_t blockSize, uint8_t pixelSize) {

        std::vector<VQBlock> codeBook = createCodebook(image, codeBookSize);
        return quantizeImage(image, codeBook, blockSize);
    }

    static uint8_t* quantizeImage(Image& image, std::vector<VQBlock> codeBook, uint8_t blockSize) {
        std::vector<VQBlock> blocks = createBlocks(image, blockSize, blockSize);
        uint8_t* result = new uint8_t[blocks.size()];
        double dump = 0.0;
        for (int i = 0; i < blocks.size(); i++) {
            VQBlock block = blocks[i];
            result[i] = nearest(block, codeBook.data(), codeBook.size(), dump);
        }
        return result;
    }

private:

    static std::vector<VQBlock> createBlocks(Image& image, int blockWidth = 2, int blockHeight = 2) {
        //Convert bitmap to blocks
        int width = image.width() / blockWidth;
        int height = image.height() / blockHeight;
        int blockSize = blockWidth * blockHeight * 4;
        int blockCount = width * height;
        std::vector<VQBlock> blocks(blockCount);

        for (int y = 0; y < height - 1; y += blockHeight) {
            for (int x = 0; x < width - 1; x += blockWidth) {
                int blockIndex = (y / blockHeight) * width + (x / blockWidth);
                blocks[blockIndex] = VQBlock(blockWidth * blockHeight, 4);
                for (int k = 0; k < blockHeight; k++) {
                    for (int l = 0; l < blockWidth; l++) {
                        int imageIndex = ((y + k) * width + (x + l)) * 4;
                        int subVectorIndex = k * blockWidth + l;
                        blocks[blockIndex].pixels(subVectorIndex * 4)     = image[imageIndex].b;
                        blocks[blockIndex].pixels(subVectorIndex * 4 + 1) = image[imageIndex].g;
                        blocks[blockIndex].pixels(subVectorIndex * 4 + 2) = image[imageIndex].r;
                        blocks[blockIndex].pixels(subVectorIndex * 4 + 3) = image[imageIndex].a;
                    }
                }
            }
        }
        return blocks;
    }

    static std::vector<VQBlock> initializeCodeBook(std::vector<VQBlock> imageBlocks, int codeBookSize, bool fastInitialization = false) {
        std::vector<VQBlock> codeBook(codeBookSize);
        if (fastInitialization) {
            int i = 0;
            for (i = 0; i < codeBookSize; i++)
            {
                codeBook[i % codeBookSize] = VQBlock(imageBlocks[i]);
            }
            for (i = 0; i < imageBlocks.size(); i++)
            {
                imageBlocks[i].group = i % codeBookSize;
            }
            return codeBook;
        } else {
            int i = 0;
            int imageLength = imageBlocks.size();
            double sum = 0.0;
            double* distances = new double[imageBlocks.size()];
            codeBook[0] = VQBlock(imageBlocks[std::rand() % imageBlocks.size()]);
            for (int cluster = 1; cluster < codeBookSize; cluster++) {
                sum = 0;
                for (i = 0; i < imageLength; i++) {
                    nearest(imageBlocks[i], codeBook.data(), cluster, distances[i]);
                    sum += distances[i];
                }
                sum = sum * std::clamp(std::rand(), 0, 0x7fff) / (0x7fff - 1.0);
                for (i = 0; i < imageLength; i++) {
                    if ((sum -= distances[i]) > 0) continue;
                    codeBook[cluster] = VQBlock(imageBlocks[i]);
                    break;
                }
            }
            double dump = 0.0;
            for (i = 0; i < imageLength; i++) {
                imageBlocks[i].group = nearest(imageBlocks[i], codeBook.data(), codeBookSize, dump);
            }
            return codeBook;
        }
    }

    static std::vector<VQBlock> createCodebook(Image& image, int codeBookSize, int blockWidth = 2, int blockHeight = 2)
    {
        if (image.width() % blockWidth != 0 || image.height() % blockHeight != 0) {
            throw std::runtime_error("The image can't be devided by the given block size!");
        }

        std::vector<VQBlock> imageBlocks = createBlocks(image, blockWidth, blockHeight);
        std::vector<VQBlock> codeBookBlocks = initializeCodeBook(imageBlocks, codeBookSize);

        int i = 0;
        int j = 0;
        int changed = 0;
        int nearestIndex = 0;
        VQBlock imageBlock;
        VQBlock codeBookBlock;
        double dump = 0.0;
        int runs = 0;

        do {
            for (i = 0; i < codeBookSize; i++) {
                codeBookBlock = codeBookBlocks[i];
                codeBookBlock.group = 0;
                codeBookBlock.zero();
            }
            for (j = 0; j < imageBlocks.size(); j++) {
                imageBlock = imageBlocks[j];
                codeBookBlock = codeBookBlocks[imageBlock.group];
                codeBookBlock.group += 1;
                codeBookBlock.add(imageBlock);
            }
            for (i = 0; i < codeBookSize; i++) {
                codeBookBlock = codeBookBlocks[i];
                codeBookBlock.div(codeBookBlock.group);
            }
            changed = 0;
            for (j = 0; j < imageBlocks.size(); j++) {
                imageBlock = imageBlocks[j];
                nearestIndex = nearest(imageBlock, codeBookBlocks.data(), codeBookSize, dump);
                if (nearestIndex != imageBlock.group)
                {
                    changed++;
                    imageBlock.group = nearestIndex;
                }
            }
            runs++;
        } while (changed > (imageBlocks.size() >> 10));

        for (i = 0; i < codeBookSize; i++) {
            codeBookBlock = codeBookBlocks[i];
            codeBookBlock.group = i;
        }
        return codeBookBlocks;
    }

    static int nearest(const VQBlock& block, VQBlock* cluster, int clusterSize, double& nearestDistance) {
        int i = 0;
        int nearestIndex = 0;
        double distance = 0;
        double minDistance = 0;
        for (i = 0; i < clusterSize; i++) {
            minDistance = std::numeric_limits<double>::infinity();
            nearestIndex = block.group;
            for (i = 0; i < clusterSize; i++) {
                if (minDistance > (distance = cluster[i].distance(block))) {
                    minDistance = distance;
                    nearestIndex = i;
                }
            }
        }
        nearestDistance = minDistance;
        return nearestIndex;
    }

};

}
}
