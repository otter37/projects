#define _CRT_SECURE_NO_WARNINGS

//
//  surfaceExtraction.cpp
//  Homework3
//
//  Created by Ben Jones on 1/6/17.
//  Copyright © 2017 Ben Jones. All rights reserved.
//

#include "SurfaceExtraction.hpp"

#include <cstdio>
#include <cstdint>

enum FaceType {
	NX,
	NY,
	NZ,
	PX,
	PY,
	PZ
};

struct Triangle{
	float normal[3];
	float v1[3];
	float v2[3];
	float v3[3];
};

inline void FillPlane(int a1, int a2, int b1, int b2, int c, int cInd, Triangle& t1, Triangle& t2)
{
    t1.v1[cInd] = c;
    t2.v1[cInd] = c;
    t1.v2[cInd] = c;
    t2.v2[cInd] = c;
    t1.v3[cInd] = c;
    t2.v3[cInd] = c;
    int aInd = (cInd +1) % 3;
    int bInd = (cInd +2) % 3;
    
    t1.v1[aInd] = a1;
    t1.v2[aInd] = a2;
    t1.v3[aInd] = a2;
    
    t2.v1[aInd] = a1;
    t2.v2[aInd] = a2;
    t2.v3[aInd] = a1;
    
    t1.v1[bInd] = b1;
    t1.v2[bInd] = b1;
    t1.v3[bInd] = b2;
    
    t2.v1[bInd] = b1;
    t2.v2[bInd] = b2;
    t2.v3[bInd] = b2;
    
}

void ExtractFace(int x, int y, int z, FaceType face, Triangle& t1, Triangle& t2)
{
    for (int i= 0; i < 3; i++)
	{
        t1.normal[i] = 0;
        t2.normal[i] = 0;
    }
    switch(face){
        case NX:
            t1.normal[0] = -1;
            t2.normal[0] = -1;
            FillPlane(y + 1, y, z, z+1, x, 0, t1, t2);
            break;
        case NY:
            t1.normal[1] = -1;
            t2.normal[1] = -1;
            FillPlane(z + 1, z, x, x+1, y, 1, t1, t2);
            break;
        case NZ:
            t1.normal[2] = -1;
            t2.normal[2] = -1;
            FillPlane(x + 1, x, y, y + 1, z, 2, t1, t2);
            break;
        case PX:
            t1.normal[0] = 1;
            t2.normal[0] = 1;
            FillPlane(y, y + 1, z, z +1, x + 1, 0, t1, t2);
            break;
        case PY:
            t1.normal[1] = 1;
            t2.normal[1] = 1;
            FillPlane(z, z + 1, x, x + 1, y + 1, 1, t1, t2);
            break;
        case PZ:
            t1.normal[2] = 1;
            t2.normal[2] = 1;
            FillPlane(x, x +1, y, y + 1, z + 1, 2, t1, t2);
            break;
    }
}

void WriteTriangle(FILE* f, const Triangle& t){
    fwrite(t.normal, sizeof(float), 3, f);
    fwrite(t.v1, sizeof(float), 3, f);
    fwrite(t.v2, sizeof(float), 3, f);
    fwrite(t.v3, sizeof(float), 3, f);
    uint16_t zero = 0;
    fwrite(&zero, sizeof(zero), 1, f);
}

void WriteSTL(const VoxelShape &model, const char* filename){
    
    FILE* f = fopen(filename, "wb");
    uint8_t header[80] = {0};
    fwrite(header, sizeof(uint8_t), 80, f);
    uint32_t numTris = 0;
    fwrite(&numTris, sizeof(numTris), 1, f);
    Triangle t1, t2;
	int nx, ny, nz;
	model.GetSize(nx, ny, nz);
	for (int x = 0; x < nx; x++)
	{
        for (int y = 0; y < ny; y++)
		{
            for (int z = 0; z < nz; z++)
			{
                if (model.GetBit(x, y, z))
				{
                    if ( ((x -1) < 0) || !model.GetBit(x - 1, y, z))
					{
                        ExtractFace(x, y, z, NX, t1, t2);
                        WriteTriangle(f, t1);
                        WriteTriangle(f, t2);
                        numTris += 2;
                    }
                    if ( ((x +1) >= nx) || !model.GetBit(x + 1, y, z))
					{
                        ExtractFace(x, y, z, PX, t1, t2);
                        WriteTriangle(f, t1);
                        WriteTriangle(f, t2);
                        numTris += 2;
                    }
                    if ( ((y -1) < 0) || !model.GetBit(x, y -1, z))
					{
                        ExtractFace(x, y, z, NY, t1, t2);
                        WriteTriangle(f, t1);
                        WriteTriangle(f, t2);
                        numTris += 2;
                    }
                    if ( ((y + 1) >= ny) || !model.GetBit(x, y+1, z))
					{
                        ExtractFace(x, y, z, PY, t1, t2);
                        WriteTriangle(f, t1);
                        WriteTriangle(f, t2);
                        numTris += 2;
                    }
                    if ( (z - 1 < 0) || !model.GetBit(x, y, z-1) )
					{
                        
                        ExtractFace(x, y, z, NZ, t1, t2);
                        WriteTriangle(f, t1);
                        WriteTriangle(f, t2);
                        numTris += 2;
                        
                    }
                    if ( (z + 1 >= nz) || !model.GetBit(x, y, z +1) )
					{
                        ExtractFace(x, y, z, PZ, t1, t2);
                        WriteTriangle(f, t1);
                        WriteTriangle(f, t2);
                        numTris += 2;
                        
                    }
                }
            }
        }
    }
    fseek(f, 80, SEEK_SET);
    fwrite(&numTris, sizeof(numTris), 1, f);
    
}
