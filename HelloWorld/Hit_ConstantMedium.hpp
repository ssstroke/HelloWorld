#pragma once

// This is kept in a separate file instead of "Hittable.hpp" because of the
// circular dependency around "Material.hpp" and "Hittable.hpp".

#include "RTWeekend.hpp"

#include "AABB.hpp"
#include "Color.hpp"
#include "Hittable.hpp"
#include "Interval.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include "Texture.hpp"
#include "Vec3.hpp"

#include <cmath>
#include <memory>

using std::make_shared;
using std::shared_ptr;

class Hit_ConstantMedium : public Hittable
{
public:
    Hit_ConstantMedium(
        const shared_ptr<Hittable> boundary,
        const double density,
        const shared_ptr<Texture> tex
    ) : boundary(boundary), neg_inv_density(-1 / density),
        phase_function(make_shared<Mat_Isotropic>(tex))
    {}

    Hit_ConstantMedium(
        const shared_ptr<Hittable> boundary,
        const double density,
        const Color& albedo
    ) : boundary(boundary), neg_inv_density(-1 / density),
        phase_function(make_shared<Mat_Isotropic>(albedo))
    {}

    bool Hit(const Ray& ray, const Interval ray_t, HitRecord& hit_record) const override
    {
        HitRecord rec_1, rec_2;

        if (this->boundary->Hit(ray, Interval::Universe, rec_1) == false)
        {
            return false;
        }
        if (this->boundary->Hit(ray, Interval(rec_1.t + 0.0001, infinity), rec_2) == false)
        {
            return false;
        }

        if (rec_1.t < ray_t.min)
        {
            rec_1.t = ray_t.min;
        }
        if (rec_2.t > ray_t.max)
        {
            rec_2.t = ray_t.max;
        }

        if (rec_1.t >= rec_2.t)
        {
            return false;
        }

        if (rec_1.t < 0)
        {
            rec_1.t = 0;
        }

        const double ray_length =
            ray.Direction().Length();
        const double distance_inside_boundary =
            (rec_2.t - rec_1.t) * ray_length;
        const double hit_distance =
            neg_inv_density * std::log(RandomDouble());

        if (hit_distance > distance_inside_boundary)
        {
            return false;
        }

        hit_record.t = rec_1.t + hit_distance / ray_length;
        hit_record.point = ray.At(hit_record.t);

        hit_record.normal = Vec3(1, 0, 0); // arbitrary
        hit_record.front_face = true;      // arbitrary

        hit_record.material = phase_function;

        return true;
    }

    AABB BBox() const override
    {
        return this->boundary->BBox();
    }

private:
    shared_ptr<Hittable> boundary;
    double neg_inv_density;
    shared_ptr<Material> phase_function;
};
