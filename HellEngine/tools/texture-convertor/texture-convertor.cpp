// texture-convertor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Compressonator.h>
#include <string>
#include <array>
#include <stb_image.h>
#include "DDS_Helpers.h"

struct stb_free
{
    void operator()(void* p)
    {
        stbi_image_free(p);
    }
};

namespace
{
    bool CompressionCallback(float fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
    {
        std::printf("\rCompression progress = %2.0f  ", fProgress);
        return false;
    }
}

int main()
{
    //Textures with color
    {
        std::array<std::string, 23> bc1_files_in =
        {
            "res/textures/Black.png",
            "res/textures/Couch_ALB.png",
            "res/textures/DoorFrame_ALB.png",
            "res/textures/Door_ALB.tga",
            "res/textures/FloorBoards_ALB.tga",
            "res/textures/Glock_ALB.png",
            "res/textures/Hands_ALB.tga",
            "res/textures/Light_ALB.tga",
            "res/textures/NumberGrid_ALB.png",
            "res/textures/PlasterCeiling_ALB.tga",
            "res/textures/Red_ALB.png",
            "res/textures/Shell_ALB.png",
            "res/textures/Shotgun_ALB.png",
            "res/textures/Stairs01_ALB.tga",
            "res/textures/Trims_ALB.tga",
            "res/textures/WallPaper_ALB.png",
            "res/textures/White_ALB.png",
            "res/textures/White.png",
            "res/textures/ZombieBoy_Top_ALB.png",
            "res/textures/Zombie_Face_ALB.png",
            "res/textures/Zombie_Hair_ALB.png",
            "res/textures/Zombie_Jeans_ALB.png",
            "res/textures/Zombie_Shirt_ALB.png"
        };

        std::array<std::string, 23> bc1_files_out =
        {
            "res/textures/Black.dds",
            "res/textures/Couch_ALB.dds",
            "res/textures/DoorFrame_ALB.dds",
            "res/textures/Door_ALB.dds",
            "res/textures/FloorBoards_ALB.dds",
            "res/textures/Glock_ALB.dds",
            "res/textures/Hands_ALB.dds",
            "res/textures/Light_ALB.dds",
            "res/textures/NumberGrid_ALB.dds",
            "res/textures/PlasterCeiling_ALB.dds",
            "res/textures/Red_ALB.dds",
            "res/textures/Shell_ALB.dds",
            "res/textures/Shotgun_ALB.dds",
            "res/textures/Stairs01_ALB.dds",
            "res/textures/Trims_ALB.dds",
            "res/textures/WallPaper_ALB.dds",
            "res/textures/White_ALB.dds",
            "res/textures/White.dds",
            "res/textures/ZombieBoy_Top_ALB.dds",
            "res/textures/Zombie_Face_ALB.dds",
            "res/textures/Zombie_Hair_ALB.dds",
            "res/textures/Zombie_Jeans_ALB.dds",
            "res/textures/Zombie_Shirt_ALB.dds"
        };

        {
            for (auto i = 0; i < bc1_files_in.size(); ++i)
            {
                int32_t w;
                int32_t h;
                int32_t nrChannels;

                std::unique_ptr<stbi_uc, stb_free > p(stbi_load(bc1_files_in[i].c_str(), &w, &h, &nrChannels, 0), stb_free());

                //swizzle
                if (nrChannels == 3)
                {
                    uint8_t*      image = p.get();
                    const uint64_t pitch = static_cast<uint64_t>(w) * 3UL;

                    for (auto r = 0; r < h; ++r)
                    {
                        uint8_t* row = image + r * pitch;
                        for (auto c = 0UL; c < static_cast<uint64_t>(w); ++c)
                        {
                            uint8_t* pixel = row + c * 3UL;
                            uint8_t  p = pixel[0];
                            pixel[0] = pixel[2];
                            pixel[2] = p;
                        }
                    }
                }

                CMP_Texture srcTexture = { 0 };
                srcTexture.dwSize = sizeof(CMP_Texture);
                srcTexture.dwWidth = w;
                srcTexture.dwHeight = h;
                srcTexture.dwPitch = nrChannels == 4 ? w * 4 : w * 3;
                srcTexture.format = nrChannels == 4 ? CMP_FORMAT_RGBA_8888 : CMP_FORMAT_RGB_888;
                srcTexture.dwDataSize = srcTexture.dwHeight * srcTexture.dwPitch;
                srcTexture.pData = p.get();

                CMP_Texture destTexture = { 0 };
                destTexture.dwSize = sizeof(destTexture);
                destTexture.dwWidth = w;
                destTexture.dwHeight = h;
                destTexture.dwPitch = w;
                destTexture.format = CMP_FORMAT_DXT1;
                destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);
                destTexture.pData = (CMP_BYTE*)malloc(destTexture.dwDataSize);

                std::cout << "Compressing ... " << bc1_files_in[i];
                CMP_CompressOptions options = { 0 };
                options.dwSize = sizeof(options);
                options.fquality = 0.88f;

                CMP_ERROR   cmp_status;
                cmp_status = CMP_ConvertTexture(&srcTexture, &destTexture, &options, &CompressionCallback);
                if (cmp_status != CMP_OK)
                {
                    free(destTexture.pData);
                    std::printf("Compression returned an error %d\n", cmp_status);
                    return cmp_status;
                }
                else
                {
                    SaveDDSFile(bc1_files_out[i].c_str(), destTexture);
                    free(destTexture.pData);
                }
            }
        }
    }

    
    //Textures with alpha
    {
        std::array<std::string, 9> bc3_files_in =
        {
            "res/textures/Blood1.png",
            "res/textures/Blood2.png",
            "res/textures/Blood3.png",
            "res/textures/Blood10.png",
            "res/textures/BloodWallSplatter1.png",
            "res/textures/Bullet.png",
            "res/textures/Muzzle1.png",
            "res/textures/Zombie_Hair_ALB.png",
            "res/textures/Zombie_Hair_RMA.png"
        };

        std::array<std::string, 9> bc3_files_out =
        {
            "res/textures/Blood1.dds",
            "res/textures/Blood2.dds",
            "res/textures/Blood3.dds",
            "res/textures/Blood10.dds",
            "res/textures/BloodWallSplatter1.dds",
            "res/textures/Bullet.dds",
            "res/textures/Muzzle1.dds",
            "res/textures/Zombie_Hair_ALB.dds",
            "res/textures/Zombie_Hair_RMA.dds"
        };

        {
            for (auto i = 0; i < bc3_files_in.size(); ++i)
            {
                int32_t w;
                int32_t h;
                int32_t nrChannels;

                std::unique_ptr<stbi_uc, stb_free > p(stbi_load(bc3_files_in[i].c_str(), &w, &h, &nrChannels, 0), stb_free());

                //swizzle
                if (nrChannels == 3)
                {
                    uint8_t* image = p.get();
                    const uint64_t pitch = static_cast<uint64_t>(w) * 3UL;

                    for (auto r = 0; r < h; ++r)
                    {
                        uint8_t* row = image + r * pitch;
                        for (auto c = 0UL; c < static_cast<uint64_t>(w); ++c)
                        {
                            uint8_t* pixel = row + c * 3UL;
                            uint8_t  p = pixel[0];
                            pixel[0] = pixel[2];
                            pixel[2] = p;
                        }
                    }
                }

                CMP_Texture srcTexture = { 0 };
                srcTexture.dwSize = sizeof(CMP_Texture);
                srcTexture.dwWidth = w;
                srcTexture.dwHeight = h;
                srcTexture.dwPitch = nrChannels == 4 ? w * 4 : w * 3;
                srcTexture.format = nrChannels == 4 ? CMP_FORMAT_RGBA_8888 : CMP_FORMAT_RGB_888;
                srcTexture.dwDataSize = srcTexture.dwHeight * srcTexture.dwPitch;
                srcTexture.pData = p.get();

                CMP_Texture destTexture = { 0 };
                destTexture.dwSize = sizeof(destTexture);
                destTexture.dwWidth = w;
                destTexture.dwHeight = h;
                destTexture.dwPitch = w;
                destTexture.format = CMP_FORMAT_DXT3;
                destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);
                destTexture.pData = (CMP_BYTE*)malloc(destTexture.dwDataSize);


                std::cout << "Compressing ... " << bc3_files_in[i];
                CMP_CompressOptions options = { 0 };
                options.dwSize = sizeof(options);
                options.fquality = 0.88f;

                CMP_ERROR   cmp_status;
                cmp_status = CMP_ConvertTexture(&srcTexture, &destTexture, &options, &CompressionCallback);
                if (cmp_status != CMP_OK)
                {
                    free(destTexture.pData);
                    std::printf("Compression returned an error %d\n", cmp_status);
                    return cmp_status;
                }
                else
                {
                    SaveDDSFile(bc3_files_out[i].c_str(), destTexture);
                    free(destTexture.pData);
                }
            }
        }
    }


    //Normal maps, must be separate for now bc1 compression, but can be 2 channel
    {
        std::array<std::string, 19> normal_maps =
        {
            "res/textures/Couch_NRM.png",
            "res/textures/DoorFrame_NRM.png",
            "res/textures/Door_NRM.tga",
            "res/textures/FloorBoards_NRM.tga",
            "res/textures/Glock_NRM.tga",
            "res/textures/Hands_NRM.tga",
            "res/textures/Light_NRM.tga",
            "res/textures/PlasterCeiling_NRM.tga",
            "res/textures/Shell_NRM.png",
            "res/textures/Shotgun_NRM.png",
            "res/textures/Shotgun_NRM.tga",
            "res/textures/Stairs01_NRM.tga",
            "res/textures/Trims_NRM.tga",
            "res/textures/WallPaper_NRM.tga",
            "res/textures/ZombieBoy_Top_NRM.png",
            "res/textures/Zombie_Face_NRM.png",
            "res/textures/Zombie_Hair_NRM.png",
            "res/textures/Zombie_Jeans_NRM.png",
            "res/textures/Zombie_Shirt_NRM.png",
        };

        std::array<std::string, 19> normal_maps_out =
        {
            "res/textures/Couch_NRM.dds",
            "res/textures/DoorFrame_NRM.dds",
            "res/textures/Door_NRM.dds",
            "res/textures/FloorBoards_NRM.dds",
            "res/textures/Glock_NRM.dds",
            "res/textures/Hands_NRM.dds",
            "res/textures/Light_NRM.dds",
            "res/textures/PlasterCeiling_NRM.dds",
            "res/textures/Shell_NRM.dds",
            "res/textures/Shotgun_NRM.dds",
            "res/textures/Shotgun_NRM.dds",
            "res/textures/Stairs01_NRM.dds",
            "res/textures/Trims_NRM.dds",
            "res/textures/WallPaper_NRM.dds",
            "res/textures/ZombieBoy_Top_NRM.dds",
            "res/textures/Zombie_Face_NRM.dds",
            "res/textures/Zombie_Hair_NRM.dds",
            "res/textures/Zombie_Jeans_NRM.dds",
            "res/textures/Zombie_Shirt_NRM.dds",
        };

        {
            for (auto i = 0; i < normal_maps.size(); ++i)
            {
                int32_t w;
                int32_t h;
                int32_t nrChannels;

                std::unique_ptr<stbi_uc, stb_free > p(stbi_load(normal_maps[i].c_str(), &w, &h, &nrChannels, 0), stb_free());

                //swizzle
                if (nrChannels == 3)
                {
                    uint8_t* image = p.get();
                    const uint64_t pitch = static_cast<uint64_t>(w) * 3UL;

                    for (auto r = 0; r < h; ++r)
                    {
                        uint8_t* row = image + r * pitch;
                        for (auto c = 0UL; c < static_cast<uint64_t>(w); ++c)
                        {
                            uint8_t* pixel = row + c * 3UL;
                            uint8_t  p = pixel[0];
                            pixel[0] = pixel[2];
                            pixel[2] = p;
                        }
                    }
                }

                CMP_Texture srcTexture = { 0 };
                srcTexture.dwSize = sizeof(CMP_Texture);
                srcTexture.dwWidth = w;
                srcTexture.dwHeight = h;
                srcTexture.dwPitch = nrChannels == 4 ? w * 4 : w * 3;
                srcTexture.format = nrChannels == 4 ? CMP_FORMAT_RGBA_8888 : CMP_FORMAT_RGB_888;
                srcTexture.dwDataSize = srcTexture.dwHeight * srcTexture.dwPitch;
                srcTexture.pData = p.get();

                CMP_Texture destTexture = { 0 };
                destTexture.dwSize = sizeof(destTexture);
                destTexture.dwWidth = w;
                destTexture.dwHeight = h;
                destTexture.dwPitch = w;
                destTexture.format = CMP_FORMAT_DXT3;
                destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);
                destTexture.pData = (CMP_BYTE*)malloc(destTexture.dwDataSize);

                std::cout << "Compressing ... " << normal_maps[i];
                CMP_CompressOptions options = { 0 };
                options.dwSize = sizeof(options);
                options.fquality = 0.88f;

                CMP_ERROR   cmp_status;
                cmp_status = CMP_ConvertTexture(&srcTexture, &destTexture, &options, &CompressionCallback);
                if (cmp_status != CMP_OK)
                {
                    free(destTexture.pData);
                    std::printf("Compression returned an error %d\n", cmp_status);
                    return cmp_status;
                }
                else
                {
                    SaveDDSFile(normal_maps_out[i].c_str(), destTexture);
                    free(destTexture.pData);
                }
            }
        }


        //rma maps, must be separate for now bc1 compression, but it can be investigated compressing as seprate bc4
        {
            std::array<std::string, 18> rma_maps =
            {
                "res/textures/Couch_RMA.png",
                "res/textures/DoorFrame_RMA.png",
                "res/textures/Door_RMA.tga",
                "res/textures/FloorBoards_RMA.tga",
                "res/textures/Glock_RMA.tga",
                "res/textures/Hands_RMA.tga",
                "res/textures/Light_RMA.tga",
                "res/textures/Picture2_RMA.tga",
                "res/textures/PlasterCeiling_RMA.tga",
                "res/textures/Shell_RMA.png",
                "res/textures/Shotgun_RMA.png",
                "res/textures/Stairs01_RMA.tga",
                "res/textures/Trims_RMA.tga",
                "res/textures/WallPaper_RMA.tga",
                "res/textures/ZombieBoy_Top_RMA.png",
                "res/textures/Zombie_Face_RMA.png",
                "res/textures/Zombie_Jeans_RMA.png",
                "res/textures/Zombie_Shirt_RMA.png"
            };

            std::array<std::string, 19> rma_out =
            {
                "res/textures/Couch_RMA.dds",
                "res/textures/DoorFrame_RMA.dds",
                "res/textures/Door_RMA.dds",
                "res/textures/FloorBoards_RMA.dds",
                "res/textures/Glock_RMA.dds",
                "res/textures/Hands_RMA.dds",
                "res/textures/Light_RMA.dds",
                "res/textures/Picture2_RMA.dds",
                "res/textures/PlasterCeiling_RMA.dds",
                "res/textures/Shell_RMA.dds",
                "res/textures/Shotgun_RMA.dds",
                "res/textures/Stairs01_RMA.dds",
                "res/textures/Trims_RMA.dds",
                "res/textures/WallPaper_RMA.dds",
                "res/textures/ZombieBoy_Top_RMA.dds",
                "res/textures/Zombie_Face_RMA.dds",
                "res/textures/Zombie_Jeans_RMA.dds",
                "res/textures/Zombie_Shirt_RMA.dds"
            };

            {
                for (auto i = 0; i < rma_maps.size(); ++i)
                {
                    int32_t w;
                    int32_t h;
                    int32_t nrChannels;

                    std::unique_ptr<stbi_uc, stb_free > p(stbi_load(rma_maps[i].c_str(), &w, &h, &nrChannels, 0), stb_free());

                    //swizzle
                    if (nrChannels == 3)
                    {
                        uint8_t* image = p.get();
                        const uint64_t pitch = static_cast<uint64_t>(w) * 3UL;

                        for (auto r = 0; r < h; ++r)
                        {
                            uint8_t* row = image + r * pitch;
                            for (auto c = 0UL; c < static_cast<uint64_t>(w); ++c)
                            {
                                uint8_t* pixel = row + c * 3UL;
                                uint8_t  p = pixel[0];
                                pixel[0] = pixel[2];
                                pixel[2] = p;
                            }
                        }
                    }

                    CMP_Texture srcTexture = { 0 };
                    srcTexture.dwSize = sizeof(CMP_Texture);
                    srcTexture.dwWidth = w;
                    srcTexture.dwHeight = h;
                    srcTexture.dwPitch = nrChannels == 4 ? w * 4 : w * 3;
                    srcTexture.format = nrChannels == 4 ? CMP_FORMAT_RGBA_8888 : CMP_FORMAT_BGR_888;
                    srcTexture.dwDataSize = srcTexture.dwHeight * srcTexture.dwPitch;
                    srcTexture.pData = p.get();

                    CMP_Texture destTexture = { 0 };
                    destTexture.dwSize = sizeof(destTexture);
                    destTexture.dwWidth = w;
                    destTexture.dwHeight = h;
                    destTexture.dwPitch = w;
                    destTexture.format = CMP_FORMAT_DXT3;
                    destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);
                    destTexture.pData = (CMP_BYTE*)malloc(destTexture.dwDataSize);


                    std::cout << "Compressing ... " << rma_out[i];
                    CMP_CompressOptions options = { 0 };
                    options.dwSize = sizeof(options);
                    options.fquality = 0.88f;

                    CMP_ERROR   cmp_status;
                    cmp_status = CMP_ConvertTexture(&srcTexture, &destTexture, &options, &CompressionCallback);
                    if (cmp_status != CMP_OK)
                    {
                        free(destTexture.pData);
                        std::printf("Compression returned an error %d\n", cmp_status);
                        return cmp_status;
                    }
                    else
                    {
                        SaveDDSFile(rma_out[i].c_str(), destTexture);
                        free(destTexture.pData);
                    }
                }
            }
        }
    }

    std::cout << "Compression done!\n";
}

