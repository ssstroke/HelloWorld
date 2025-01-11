#pragma once

#include "Hittable.hpp"

#include <vector>

class HittableList : public Hittable
{
public:
    std::vector<std::shared_ptr<Hittable>> objects;

    HittableList() {}
    HittableList(std::shared_ptr<Hittable> object) { Add(object); }

    void Add(const std::shared_ptr<Hittable> object) { objects.push_back(object); }
    void Clear() { objects.clear(); }

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
                closest_so_far = temp_record.t;

                record = temp_record;
            }
        }

        return hit_anything;
    }
};



