//
//  Cube.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/24/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Cube_hpp
#define Cube_hpp

#include <vector>
#include <cassert>
#include "Vec3.hpp"
#include "Vec3Tex.hpp"
#include "IndexedLineList.hpp"
#include "IndexedTriangleList.hpp"

class Cube
{
public:
    Cube()
    {
        // vertices
        float len = 0.5f;
        vertices.emplace_back(-len, -len, -len);
        vertices.emplace_back(-len, -len,  len);
        vertices.emplace_back(-len,  len, -len);
        vertices.emplace_back(-len,  len,  len);
        vertices.emplace_back( len, -len, -len);
        vertices.emplace_back( len, -len,  len);
        vertices.emplace_back( len,  len, -len);
        vertices.emplace_back( len,  len,  len);
        
        // lines
        lines.push_back({0,1});
        lines.push_back({0,2});
        lines.push_back({0,4});
        lines.push_back({3,1});
        lines.push_back({3,2});
        lines.push_back({4,5});
        lines.push_back({4,6});
        lines.push_back({5,1});
        lines.push_back({6,2});
        lines.push_back({7,3});
        lines.push_back({7,5});
        lines.push_back({7,6});
        
        // triangles
        triangles.push_back({1,2,0}); // left panel
        triangles.push_back({3,2,1});
        triangles.push_back({4,1,0}); // bottom panel
        triangles.push_back({4,5,1});
        triangles.push_back({6,2,3}); // top panel
        triangles.push_back({7,6,3});
        triangles.push_back({6,5,4}); // right panel
        triangles.push_back({6,7,5});
        triangles.push_back({2,6,0}); // front panel
        triangles.push_back({6,4,0});
        triangles.push_back({5,3,1}); // back panel
        triangles.push_back({5,7,3});
        
        // textures
        
        // map a texture onto the left panel
        textureCoords.emplace_back(1.0f, 1.0f); // vertex 0
        textureCoords.emplace_back(0.0f, 1.0f); // vertex 1
        textureCoords.emplace_back(1.0f, 0.0f); // vertex 2
        textureCoords.emplace_back(0.0f, 0.0f); // vertex 3
        
        // map a texture onto the right panel
        textureCoords.emplace_back(0.0f, 1.0f); // vertex 4
        textureCoords.emplace_back(1.0f, 1.0f); // vertex 5
        textureCoords.emplace_back(0.0f, 0.0f); // vertex 6
        textureCoords.emplace_back(1.0f, 0.0f); // vertex 7
        
        assert(textureCoords.size() == vertices.size());
    }
    IndexedLineList GetIndexedLineList()
    {
        return { vertices, lines };
    }
    IndexedTriangleList<Vec3> GetIndexedTriangleList()
    {
        // these follow a left-hand-rule winding order such that normals
        // are facing outward
        return { vertices, triangles };
    }
    IndexedTriangleList<Vec3Tex> GetIndexedTriangleListTex()
    {
        std::vector<Vec3Tex> verticesTex;
        for (int i = 0; i < vertices.size(); i++)
            verticesTex.push_back({vertices[i], textureCoords[i]});

        return { verticesTex, triangles };
    }
    
private:
    std::vector<Vec3> vertices;
    std::vector<Line> lines;
    std::vector<Triangle> triangles;
    std::vector<Vec2> textureCoords;
};

#endif /* Cube_hpp */
