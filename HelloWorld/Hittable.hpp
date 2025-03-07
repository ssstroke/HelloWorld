#pragma once

#include "AABB.hpp"
#include "Interval.hpp"
#include "Ray.hpp"
#include "Vec3.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>
#include <vector>

class Material;

class HitRecord
{
public:
    Point3 point;

    Vec3 normal;
    bool front_face;

    double t;

    std::shared_ptr<Material> material;

    // NOTE: `outward_normal` is assumed to have unit length.
    void SetFaceNormal(const Ray& ray, const Vec3& outward_normal)
    {
        this->front_face = Dot(ray.Direction(), outward_normal) < 0;
        this->normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool Hit(const Ray& ray, const Interval ray_t, HitRecord& hit_record) const = 0;

    virtual AABB BBox() const = 0;
};

class Hit_List : public Hittable
{
public:
    std::vector<std::shared_ptr<Hittable>> objects;

    Hit_List() {}

    Hit_List(std::shared_ptr<Hittable> object)
    {
        Add(object);
    }

    void Add(const std::shared_ptr<Hittable> object)
    {
        objects.push_back(object);
        this->bbox = AABB(this->bbox, object->BBox());
    }

    void Clear()
    {
        objects.clear();
    }

    AABB BBox() const override
    {
        return this->bbox;
    }

    bool Hit(const Ray& ray, const Interval ray_t, HitRecord& record) const override
    {
        HitRecord temp_record;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object : objects)
        {
            if (object->Hit(ray, Interval(ray_t.min, closest_so_far), temp_record))
            {
                hit_anything = true;

                // Imagine ray hitting sphere that is 1 unit away from the camera.
                // By storing 'closest_so_far' and using it in Hit() function the
                // ray won't hit the sphere that is 2 units away. I.e. we will not
                // hit objects that are behind the closest object. Makes sense ;)
                //
                // I just came back to this code in 2025 (it is a year later now)
                // and I want to thank myself for providing an explanation to my
                // future self.
                closest_so_far = temp_record.t;

                record = temp_record;
            }
        }

        return hit_anything;
    }

private:
    AABB bbox;
};

class Hit_BVHNode : public Hittable
{
public:
    Hit_BVHNode(Hit_List list) : Hit_BVHNode(list.objects, 0, list.objects.size())
    {
        // There's a C++ subtlety here. This constructor (without span indices) creates an
        // implicit copy of the hittable list, which we will modify. The lifetime of the copied
        // list only extends until this constructor exits. That's OK, because we only need to
        // persist the resulting bounding volume hierarchy.
    }

    Hit_BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end)
    {
        this->bbox = AABB::Empty;
        for (size_t i = start; i < end; ++i)
        {
            this->bbox = AABB(this->bbox, objects[i]->BBox());
        }

        const int axis = this->bbox.LongestAxis();

        const auto comparator =
            (axis == 0) ? BoxCompareX :
            (axis == 1) ? BoxCompareY :
            BoxCompareZ;

        const size_t object_span = end - start;

        if (object_span == 1)
        {
            left = right = objects[start];
        }
        else if (object_span == 2)
        {
            left = objects[start];
            right = objects[start + 1];
        }
        else
        {
            std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);

            const auto mid = start + object_span / 2;

            left = std::make_shared<Hit_BVHNode>(objects, start, mid);
            right = std::make_shared<Hit_BVHNode>(objects, mid, end);
        }
    }

    bool Hit(const Ray& ray, Interval ray_t, HitRecord& hit_record) const override
    {
        if (this->bbox.Hit(ray, ray_t))
        {
            const bool hit_left = this->left->Hit(ray, ray_t, hit_record);
            const bool hit_right = this->right->Hit(ray, Interval(ray_t.min, hit_left ? hit_record.t : ray_t.max), hit_record);

            return hit_left || hit_right;
        }

        return false;
    }

    AABB BBox() const override
    {
        return this->bbox;
    }

private:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    AABB bbox;

    static bool BoxCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, const int axis_index)
    {
        return a->BBox().AxisInterval(axis_index).min < b->BBox().AxisInterval(axis_index).min;
    }

    static bool BoxCompareX(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
    {
        return BoxCompare(a, b, 0);
    }

    static bool BoxCompareY(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
    {
        return BoxCompare(a, b, 1);
    }

    static bool BoxCompareZ(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
    {
        return BoxCompare(a, b, 2);
    }
};

class Hit_Sphere : public Hittable
{
public:
    // Stationary sphere.
    Hit_Sphere(const Point3& static_center, const double radius, const std::shared_ptr<Material> material) :
        center(static_center, Vec3(0, 0, 0)), radius(std::max(0.0, radius)), material(material)
    {
        const auto rvec = Vec3(radius, radius, radius);
        this->bbox = AABB(static_center - rvec, static_center + rvec);
    }

    // Moving sphere.
    Hit_Sphere(const Point3& center_0, const Point3& center_1, const double radius, const std::shared_ptr<Material> material) :
        center(center_0, center_1 - center_0), radius(std::max(0.0, radius)), material(material)
    {
        const auto rvec = Vec3(radius, radius, radius);
        const AABB bbox_0(center.At(0) - rvec, center.At(0) + rvec);
        const AABB bbox_1(center.At(1) - rvec, center.At(1) + rvec);
        this->bbox = AABB(bbox_0, bbox_1);
    }

    bool Hit(const Ray& ray, const Interval ray_t, HitRecord& record) const override
    {
        const Point3 current_center = center.At(ray.Time());

        const Vec3 OC = current_center - ray.Origin();

        const auto A = ray.Direction().LengthSquared();
        const auto H = Dot(ray.Direction(), OC);
        const auto C = OC.LengthSquared() - radius * radius;

        const auto DISCRIMINANT = H * H - A * C;
        if (DISCRIMINANT < 0)
        {
            return false;
        }

        const auto SQRT_DISCRIMINANT = std::sqrt(DISCRIMINANT);

        // Find the nearest root that lies in the acceptable range.
        auto root = (H - SQRT_DISCRIMINANT) / A;
        if (ray_t.Surrounds(root) == false)
        {
            root = (H + SQRT_DISCRIMINANT) / A;
            if (ray_t.Surrounds(root) == false)
            {
                return false;
            }
        }

        record.t = root;

        record.point = ray.At(record.t);

        const auto outward_normal = (record.point - current_center) / radius;
        record.SetFaceNormal(ray, outward_normal);

        record.material = this->material;

        return true;
    }

    AABB BBox() const override
    {
        return this->bbox;
    }

private:
    AABB bbox;

    std::shared_ptr<Material> material;

    Ray center;
    double radius = 1;
};
