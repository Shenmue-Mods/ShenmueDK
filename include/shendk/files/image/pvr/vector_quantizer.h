#pragma once

#include <stdint.h>
#include <vector>

#include "shendk/types/image.h"

namespace shendk {
namespace pvr {

// TODO: cleanup parameter mess (blockWidth, blockHeight)
// TODO: add dynamic pixel format (RGBA, RGB)

struct VQBlock;

static int nearest(const VQBlock& block, VQBlock* cluster, int clusterSize, double& nearestDistance);
static std::vector<VQBlock> createBlocks(Image& image, int blockWidth = 2, int blockHeight = 2);
static std::vector<VQBlock> initializeCodeBook(std::vector<VQBlock> imageBlocks, int codeBookSize, bool fastInitialization = false);
static std::vector<VQBlock> createCodebook(Image& image, int codeBookSize, int blockWidth = 2, int blockHeight = 2);
static uint8_t* quantizeImage(Image& image, std::vector<VQBlock> codeBook, uint8_t blockSize);
static uint8_t* quantizeImage(Image& image, uint32_t codeBookSize, uint8_t blockSize, uint8_t pixelSize);

}
}
