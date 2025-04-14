#pragma once

#include "RTWeekend.hpp"

#include "AABB.hpp"
#include "Interval.hpp"
#include "Ray.hpp"
#include "Vec3.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <memory>
#include <utility>
#include <vector>

using std::make_shared;
using std::shared_ptr;

class Material;

class HitRecord
{
public:
    Point3 point;

    Vec3 normal;
    bool front_face = false;

    double t = 0;

    shared_ptr<Material> material;

    double u = 0;
    double v = 0;

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
    std::vector<shared_ptr<Hittable>> objects;

    Hit_List() {}

    Hit_List(shared_ptr<Hittable> object)
    {
        Add(object);
    }

    void Add(const shared_ptr<Hittable> object)
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
        double closest_so_far = ray_t.max;

        for (const shared_ptr<Hittable>& object : objects)
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

    Hit_BVHNode(std::vector<shared_ptr<Hittable>>& objects, size_t start, size_t end)
    {
        this->bbox = AABB::Empty;
        for (size_t i = start; i < end; ++i)
        {
            this->bbox = AABB(this->bbox, objects[i]->BBox());
        }

        const int axis = this->bbox.LongestAxis();

        // This is a function pointer.
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

            const size_t mid = start + object_span / 2;

            left = make_shared<Hit_BVHNode>(objects, start, mid);
            right = make_shared<Hit_BVHNode>(objects, mid, end);
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
    shared_ptr<Hittable> left;
    shared_ptr<Hittable> right;
    AABB bbox;

    static bool BoxCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, const int axis_index)
    {
        return a->BBox().AxisInterval(axis_index).min < b->BBox().AxisInterval(axis_index).min;
    }

    static bool BoxCompareX(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
    {
        return BoxCompare(a, b, 0);
    }

    static bool BoxCompareY(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
    {
        return BoxCompare(a, b, 1);
    }

    static bool BoxCompareZ(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
    {
        return BoxCompare(a, b, 2);
    }
};

class Hit_Translate : public Hittable
{
public:
    Hit_Translate(const shared_ptr<Hittable> object, const Vec3& offset) :
        object(object), offset(offset)
    {
        this->bbox = this->object->BBox() + offset;
    }

    bool Hit(const Ray& ray, const Interval ray_t, HitRecord& hit_record) const override
    {
        const Ray ray_offset = Ray(ray.Origin() - offset, ray.Direction(), ray.Time());

        if (this->object->Hit(ray_offset, ray_t, hit_record) == false)
        {
            return false;
        }

        hit_record.point += offset;

        return true;
    }

    AABB BBox() const override
    {
        return this->bbox;
    }

private:
    shared_ptr<Hittable> object;
    Vec3 offset;
    AABB bbox;
};

class Hit_RotateY : public Hittable
{
public:
    Hit_RotateY(shared_ptr<Hittable> object, const double angle) :
        object(object)
    {
        const double radians = DegreesToRadians(angle);
        this->sin_theta = std::sin(radians);
        this->cos_theta = std::cos(radians);
        this->bbox = object->BBox();

        Point3 min( infinity,  infinity,  infinity);
        Point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int k = 0; k < 2; ++k)
                {
                    const double x = i * bbox.x.max + (1 - i) * bbox.x.min;
                    const double y = j * bbox.y.max + (1 - j) * bbox.y.min;
                    const double z = k * bbox.z.max + (1 - k) * bbox.z.min;

                    const double x_new =  this->cos_theta * x + this->sin_theta * z;
                    const double z_new = -this->sin_theta * x + this->cos_theta * z;
                    
                    const Vec3 tester(x_new, y, z_new);

                    for (int c = 0; c < 3; ++c)
                    {
                        min[c] = std::min(min[c], tester[c]);
                        max[c] = std::max(max[c], tester[c]);
                    }
                }
            }
        }

        this->bbox = AABB(min, max);
    }

    bool Hit(const Ray& ray, const Interval ray_t, HitRecord& hit_record) const override
    {
        // Transform the ray from world space to object space.
        //

        const Point3 origin = Point3(
            (cos_theta * ray.Origin().x()) - (sin_theta * ray.Origin().z()),
            ray.Origin().y(),
            (sin_theta * ray.Origin().x()) + (cos_theta * ray.Origin().z())
        );
        const Vec3 direction = Vec3(
            (cos_theta * ray.Direction().x()) - (sin_theta * ray.Direction().z()),
            ray.Direction().y(),
            (sin_theta * ray.Direction().x()) + (cos_theta * ray.Direction().z())
        );
        const Ray ray_rotated(origin, direction, ray.Time());

        if (object->Hit(ray_rotated, ray_t, hit_record) == false)
        {
            return false;
        }

        // Transform the intersection from object space back to world space.
        //

        hit_record.point = Point3(
            (cos_theta * hit_record.point.x()) + (sin_theta * hit_record.point.z()),
            hit_record.point.y(),
            (-sin_theta * hit_record.point.x()) + (cos_theta * hit_record.point.z())
        );
        hit_record.normal = Vec3(
            (cos_theta * hit_record.normal.x()) + (sin_theta * hit_record.normal.z()),
            hit_record.normal.y(),
            (-sin_theta * hit_record.normal.x()) + (cos_theta * hit_record.normal.z())
        );

        return true;
    }

    AABB BBox() const override
    {
        return this->bbox;
    }

private:
    shared_ptr<Hittable> object;
    double sin_theta;
    double cos_theta;
    AABB bbox;
};

class Hit_Sphere : public Hittable
{
public:
    // Stationary sphere.
    Hit_Sphere(const Point3& static_center, const double radius, const shared_ptr<Material> material) :
        center(static_center, Vec3(0, 0, 0)), radius(std::max(0.0, radius)), material(material)
    {
        const Vec3 rvec = Vec3(radius, radius, radius);
        this->bbox = AABB(static_center - rvec, static_center + rvec);
    }

    // Moving sphere.
    Hit_Sphere(const Point3& center_0, const Point3& center_1, const double radius, const shared_ptr<Material> material) :
        center(center_0, center_1 - center_0), radius(std::max(0.0, radius)), material(material)
    {
        const Vec3 rvec = Vec3(radius, radius, radius);
        const AABB bbox_0(center.At(0) - rvec, center.At(0) + rvec);
        const AABB bbox_1(center.At(1) - rvec, center.At(1) + rvec);
        this->bbox = AABB(bbox_0, bbox_1);
    }

    bool Hit(const Ray& ray, const Interval ray_t, HitRecord& hit_record) const override
    {
        const Point3 current_center = center.At(ray.Time());

        const Vec3 OC = current_center - ray.Origin();

        const double a = ray.Direction().LengthSquared();
        const double h = Dot(ray.Direction(), OC);
        const double c = OC.LengthSquared() - radius * radius;

        const double discriminant = h * h - a * c;
        if (discriminant < 0)
        {
            return false;
        }

        const double sqrt_discriminant = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        double root = (h - sqrt_discriminant) / a;
        if (ray_t.Surrounds(root) == false)
        {
            root = (h + sqrt_discriminant) / a;
            if (ray_t.Surrounds(root) == false)
            {
                return false;
            }
        }

        hit_record.t = root;

        hit_record.point = ray.At(hit_record.t);

        const Vec3 outward_normal = (hit_record.point - current_center) / radius;
        hit_record.SetFaceNormal(ray, outward_normal);

        GetUV(outward_normal, hit_record.u, hit_record.v);

        hit_record.material = this->material;

        return true;
    }

    AABB BBox() const override
    {
        return this->bbox;
    }

private:
    shared_ptr<Material> material;
    AABB bbox;


    Ray center;
    double radius = 1;

    static void GetUV(const Point3& p, double& u, double& v)
    {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0, 1] of angle around the Y axis from X = -1.
        // v: returned value [0, 1] of angle from Y = -1 to Y = +1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        const double theta = std::acos(-p.y());
        const double phi = std::atan2(-p.z(), p.x()) + pi;

        u = phi / (2 * pi);
        v = theta / pi;
    }
};

class Hit_Quad : public Hittable
{
public:
    Hit_Quad(const Point3& _q, const Vec3& _u, const Vec3& _v, shared_ptr<Material> _material) :
        q(_q), u(_u), v(_v), material(_material)
    {
        const Vec3 n = Cross(u, v);
        this->normal = UnitVector(n);
        this->d = Dot(normal, q);
        this->w = n / Dot(n, n);

        SetBBox();
    }

    virtual void SetBBox()
    {
        this->bbox = AABB(AABB(q, q + u + v), AABB(q + u, q + v));
    }

    AABB BBox() const override
    {
        return this->bbox;
    }

    bool Hit(const Ray& ray, const Interval ray_t, HitRecord& hit_record) const override
    {
        const double denominator = Dot(this->normal, ray.Direction());
        if (std::abs(denominator) < 1e-8)
            // Ray is parallel to plane.
        {
            return false;
        }

        const double t = (this->d - Dot(this->normal, ray.Origin())) / denominator;
        if (ray_t.Contains(t) == false)
        {
            return false;
        }
        
        const Point3 intersection = ray.At(t);
        const Vec3 planar_intersection = intersection - this->q;
        const double alpha = Dot(this->w, Cross(planar_intersection, v));
        const double beta  = Dot(this->w, Cross(u, planar_intersection));

        if (_Hit(alpha, beta) == false)
        {
            return false;
        }
        hit_record.u = alpha;
        hit_record.v = beta;

        hit_record.t = t;
        hit_record.point = intersection;
        hit_record.material = material;
        hit_record.SetFaceNormal(ray, this->normal);

        return true;
    }

protected:
    shared_ptr<Material> material;
    AABB bbox;

    Point3 q;
    Vec3 u, v;
    Vec3 normal;
    double d;
    Vec3 w;

    virtual bool _Hit(const double alpha, const double beta) const
    {
        const Interval unit_interval = Interval(0, 1);
        if (unit_interval.Contains(alpha) == false || unit_interval.Contains(beta) == false)
        {
            return false;
        }
        return true;
    }
};

class Hit_Tri : public Hit_Quad
{
public:
    Hit_Tri(const Point3& q, const Vec3& u, const Vec3& v, shared_ptr<Material> material) :
        Hit_Quad(q, u, v, material) {}

protected:
    bool _Hit(const double alpha, const double beta) const override
    {
        if (alpha < 0 || beta < 0 || alpha + beta > 1)
        {
            return false;
        }
        return true;
    }
};

inline shared_ptr<Hit_List> Box(const Point3& a, const Point3& b, const shared_ptr<Material> material)
{
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

    auto sides = make_shared<Hit_List>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    const Point3 min = Point3(std::fmin(a.x(), b.x()), std::fmin(a.y(), b.y()), std::fmin(a.z(), b.z()));
    const Point3 max = Point3(std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()), std::fmax(a.z(), b.z()));

    const Vec3 dx = Vec3(max.x() - min.x(), 0, 0);
    const Vec3 dy = Vec3(0, max.y() - min.y(), 0);
    const Vec3 dz = Vec3(0, 0, max.z() - min.z());

    sides->Add(make_shared<Hit_Quad>(Point3(min.x(), min.y(), max.z()), dx, dy, material)); // front
    sides->Add(make_shared<Hit_Quad>(Point3(max.x(), min.y(), max.z()), -dz, dy, material)); // right
    sides->Add(make_shared<Hit_Quad>(Point3(max.x(), min.y(), min.z()), -dx, dy, material)); // back
    sides->Add(make_shared<Hit_Quad>(Point3(min.x(), min.y(), min.z()), dz, dy, material)); // left
    sides->Add(make_shared<Hit_Quad>(Point3(min.x(), max.y(), max.z()), dx, -dz, material)); // top
    sides->Add(make_shared<Hit_Quad>(Point3(min.x(), min.y(), min.z()), dx, dz, material)); // bottom

    return sides;
}
