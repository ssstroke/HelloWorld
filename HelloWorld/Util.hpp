#pragma once

#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_DOUBLE
#include "include/tiny_obj_loader.h"

#include "Color.hpp"
#include "Hittable.hpp"
#include "Material.hpp"
#include "Vec3.hpp"

#include "Texture.hpp"
#include "Vec2.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

Hit_List MeshLoad(const std::string& filename)
{
    tinyobj::ObjReaderConfig reader_config;
    /// Default = "" = search from the same directory of .obj file.
    reader_config.mtl_search_path = ""; // Path to material files

    tinyobj::ObjReader reader;
    if (reader.ParseFromFile(filename, reader_config) == false)
    {
        if (reader.Error().empty() == false)
        {
            std::cerr << "[ERROR]: TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty())
    {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    Hit_List world;
    std::vector<shared_ptr<Material>> mats(materials.size(), nullptr);

    // Loop over shapes.
    for (size_t s = 0; s < shapes.size(); ++s)
    {
        // Loop over faces(polygon).
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
        {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            std::vector<Point3> tri_points;
            tri_points.resize(3);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; ++v)
            {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                const double vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                const double vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                const double vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                tri_points[v] = Point3(vx, vy, vz);
            }

            // Construct material.
            //

            shared_ptr<Material> mat = nullptr;

            // per-face material
            const int mat_id = shapes[s].mesh.material_ids[f];
            const tinyobj::material_t& mat_raw = materials[mat_id];

            if (mats[mat_id] == nullptr)
            {
                // Material has not been constructed yet.

                if (
                    mat_raw.transmittance[0] == 1.0 &&
                    mat_raw.transmittance[1] == 1.0 &&
                    mat_raw.transmittance[2] == 1.0
                    )
                { 
                    // Dielectric.
                    mat = make_shared<Mat_Dielectric>(mat_raw.ior);
                }
                else if (mat_raw.metallic == 1)
                {
                    // Metal.
                    mat = make_shared<Mat_Metal>(Color(
                        mat_raw.diffuse[0], mat_raw.diffuse[1], mat_raw.diffuse[2]),
                        mat_raw.roughness);
                }
                else
                {
                    // Lambertian.
                    if (mat_raw.diffuse_texname != "")
                    {
                        mat = make_shared<Mat_Lambertian>(make_shared<Tex_Image>(
                            mat_raw.diffuse_texname
                        ));
                    }
                    else
                    {
                        mat = make_shared<Mat_Lambertian>(Color(
                            mat_raw.diffuse[0], mat_raw.diffuse[1], mat_raw.diffuse[2])
                        );
                    }
                }

                mats[mat_id] = mat;
            }
            else
            {
                // Material was already constructed.

                mat = mats[mat_id];
            }

            // Construct triangle.
            // 
            // Triangles in the project are stored as Q, u, v, where:
            // Q    - origin point;
            // u, v - vectors originating in Q that represent triangle sides.
            // 
            // Given that, we construct triangle with Q, (R - Q), (S - Q),
            // where R and S are triangle points obtained from .obj file.

            if (mat_raw.diffuse_texname != "")
            {
                std::vector<Vec2> uv = std::vector<Vec2>(3);

                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; ++v)
                {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                    tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

                    uv[v] = Vec2(tx, ty);
                }

                world.Add(make_shared<Hit_Tri>(
                    tri_points[0],
                    tri_points[1] - tri_points[0],
                    tri_points[2] - tri_points[0],
                    uv,
                    mat
                ));
            }
            else
            {
                world.Add(make_shared<Hit_Tri>(
                    tri_points[0],
                    tri_points[1] - tri_points[0],
                    tri_points[2] - tri_points[0],
                    mat
                ));
            }

            index_offset += fv;
        }
    }

    return world;
}

inline static double LinearToGamma(const double linear_component)
{
    if (linear_component > 0)
    {
        return std::sqrt(linear_component);
    }
    return 0;
}
