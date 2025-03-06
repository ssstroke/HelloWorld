#pragma once

#include "AABB.hpp"
#include "Hittable.hpp"
#include "HittableList.hpp"
#include "RTWeekend.hpp"

#include <algorithm>

class BVHNode : public Hittable
{
public:
    BVHNode(HittableList list) : BVHNode(list.objects, 0, list.objects.size())
    {
        // There's a C++ subtlety here. This constructor (without span indices) creates an
        // implicit copy of the hittable list, which we will modify. The lifetime of the copied
        // list only extends until this constructor exits. That's OK, because we only need to
        // persist the resulting bounding volume hierarchy.
    }

    BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end)
    {
        const int axis = RandomInt(0, 2);

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
            left  = objects[start];
            right = objects[start + 1];
        }
        else
        {
            std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);

            const auto mid = start + object_span / 2;

            left  = std::make_shared<BVHNode>(objects, start, mid);
            right = std::make_shared<BVHNode>(objects, mid, end);
        }

        this->bbox = AABB(left->BBox(), right->BBox());
    }

    bool Hit(const Ray& ray, Interval ray_t, HitRecord& hit_record) const override
    {
        if (this->bbox.Hit(ray, ray_t))
        {
            const bool hit_left  = this->left->Hit(ray, ray_t, hit_record);
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
