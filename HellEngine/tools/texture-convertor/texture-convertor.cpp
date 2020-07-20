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
                destTexture.format = CMP_FORMAT_BC1;
                destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);
                destTexture.pData = (CMP_BYTE*)malloc(destTexture.dwDataSize);

                std::cout << "Compressing ... " << bc1_files_in[i];
                CMP_CompressOptions options = { 0 };
                options.dwSize = sizeof(options);

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
                destTexture.format = CMP_FORMAT_BC3;
                destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);
                destTexture.pData = (CMP_BYTE*)malloc(destTexture.dwDataSize);


                std::cout << "Compressing ... " << bc3_files_in[i];
                CMP_CompressOptions options = { 0 };
                options.dwSize = sizeof(options);

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

    std::cout << "Compression done!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
