//
//  Sphere.hpp
//  engine3d
//
//  Created by Brian Dolan on 5/4/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Sphere_hpp
#define Sphere_hpp

#include <vector>
#include <cassert>
#include <cmath>
#include "Vec3.hpp"
#include "IndexedLineList.hpp"
#include "IndexedTriangleList.hpp"
#include "FlatShadingEffect.hpp"
#include "GouraudEffect.hpp"

class Sphere
{
public:
    Sphere(float radius = 1.0f, int numVertices = 16)
    {
        // number of vertices for latitude and longitude
        size_t numVerticesLat = numVertices;
        size_t numVerticesLong = numVertices * 2;

        // number of vertices on the side of the sphere excluding north/south poles
        size_t numVerticesLatSide = numVerticesLat - 1;
        
        float angleLat = 0.0f;
        float angleLong = 0.0f;
        float angleLatIncrement = M_PI / numVerticesLat;
        float angleLongIncrement = 2.0f * M_PI / numVerticesLong;
        
        // index of the current vertex being processed - in general, "i" is used throughout
        // this function to represent an index into the vertex vector
        size_t i = 0;
        
        // loop through all the longitudes, and actually go one past in order to handle
        // surface wrapping
        for (size_t longNum = 0; longNum < numVerticesLong + 1; longNum++)
        {
            // don't start exacty at the "north pole" - start one step off, as we
            // will add the triangle-based "caps" later
            angleLat = angleLatIncrement;
            
            // loop through latitudes but stop *before* processing the "south pole"
            // (for the same reason)
            for (size_t latNum = 0; latNum < numVerticesLatSide; latNum++)
            {
                // create a vertex - unless processing the last/wraparound longitude
                if (longNum < numVerticesLong)
                {
                    float x = radius * sin(angleLat) * cos(angleLong);
                    float y = radius * cos(angleLat);
                    float z = radius * sin(angleLat) * sin(angleLong);
                    
                    AddVertex(x, y, z);
                }
                
                // connect the previous longitude with the current longitude, with rectangles (two
                // triangles), and handle wraparound processing for the final (wrapped around) longitude
                if (longNum > 0)
                {
                    size_t iCurrLong = (longNum < numVerticesLong ? i : i % numVerticesLong);
                    size_t iPrevLong = i - numVerticesLatSide;
                    
                    // create one or two triangles connected to this vertex, ensuring that normals
                    // are facing outward according to the left-hand rule
                    
                    if (latNum < (numVerticesLatSide - 1))
                        triangles.push_back({iCurrLong, iPrevLong + 1, iPrevLong});
                    
                    if (latNum > 0)
                        triangles.push_back({iCurrLong, iPrevLong, iCurrLong - 1});
                }
                
                angleLat += angleLatIncrement;
                i++;
            }
            
            angleLong += angleLongIncrement;
        }
        
        // we will create a single vertex for each of the north and south poles, at the end of the
        // vertex vector - keep track of these indices, but don't add them yet, in order to keep the math
        // coming up simple
        size_t iNorthPole = vertices.size();
        size_t iSouthPole = vertices.size() + 1;
        
        // create "caps" at the north and south poles using triangles (rather than rectangles used
        // elsewhere)
        // we will loop around the circle of vertices closest to the north pole, with the knowledge
        // that the indices of the similar south pole circle are simply a fixed offset from the north
        size_t iNorth = numVerticesLatSide;
        for (size_t longNum = 0; longNum < numVerticesLong + 1; longNum++)
        {
            if (longNum > 0)
            {
                size_t iCurrLong = (longNum < numVerticesLong ? iNorth : iNorth % numVerticesLong);
                size_t iPrevLong = iNorth - numVerticesLatSide;
                
                // north pole cap
                triangles.push_back({iCurrLong, iPrevLong, iNorthPole});
                
                // south pole cap
                triangles.push_back({iPrevLong + (numVerticesLatSide - 1),
                                     iCurrLong + (numVerticesLatSide - 1), iSouthPole});
                
                iNorth += numVerticesLatSide;
            }
        }
        
        // now, finally, actually add the north/south poles vertices
        AddVertex(0.0f, radius, 0.0f); // north pole
        AddVertex(0.0f, -radius, 0.0f); // south pole
        
        assert(normals.size() == vertices.size());
    }
    IndexedTriangleList<Vec3> GetIndexedTriangleList()
    {
        return { vertices, triangles };
    }
    IndexedTriangleList<FlatShadingEffect::Vertex> GetIndexedTriangleListFS()
    {
        std::vector<FlatShadingEffect::Vertex> verticesFS;
        for (const auto& v : vertices)
            verticesFS.emplace_back(v);
        
        return { verticesFS, triangles };
    }
    IndexedTriangleList<GouraudEffect::Vertex> GetIndexedTriangleListG()
    {
        std::vector<GouraudEffect::Vertex> verticesG;
        for (size_t i = 0; i < vertices.size(); i++)
            verticesG.push_back({vertices[i], normals[i]});
        
        return { verticesG, triangles };
    }
    
private:
    void AddVertex(float x, float y, float z)
    {
        Vec3 v(x, y, z);
        vertices.push_back(v);
        
        // the normal vector is in the same direction as the vertex position vector itself
        // since the sphere is centered around origin
        normals.push_back(v.Norm());
    }
    
    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<IndexedTriangle> triangles;
};

#endif /* Sphere_hpp */
