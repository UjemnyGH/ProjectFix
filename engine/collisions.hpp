#pragma once
#ifndef __COLLISIONS_
#define __COLLISIONS_

#include "engine_math.hpp"
#include "renderer.hpp"
#include <cfloat>

struct Box {
    RVec mMax;
    RVec mMin;
    // Currently not used
    RVec mRotation;

    Box() {
        mMax = RVec(1.0f, 1.0f, 1.0f);
        mMin = RVec(-1.0f, -1.0f, -1.0f);
        mRotation = RVec().ToRad();
    }

    Box(RVec pos, RVec size) {
        mRotation = RVec().ToRad();
        mMin = pos - size;
        mMax = pos + size;
    }

    void SetPosition(RVec pos) {
        RVec size = (RVec(mMax.x - mMin.x, mMax.y - mMin.y, mMax.z - mMin.z, 1.0f) / 2.0f).Abs();
        mMin = pos - size;
        mMax = pos + size;
    }

    void SetSize(RVec size) {
        RVec pos = mMax - (RVec(mMax.x - mMin.x, mMax.y - mMin.y, mMax.z - mMin.z, 1.0f) / 2.0f).Abs();
        mMin = pos - size;
        mMax = pos + size;
    }

    RVec GetPosition() {
        return mMax - (RVec(mMax.x - mMin.x, mMax.y - mMin.y, mMax.z - mMin.z, 1.0f) / 2.0f).Abs();
    }

    RVec GetSize() {
        return (RVec(mMax.x - mMin.x, mMax.y - mMin.y, mMax.z - mMin.z, 1.0f) / 2.0f).Abs();
    }

    void _debug_Show() {
        _debug_DrawCube(GetPosition(), GetSize());
    }
};

struct Sphere {
    RVec mPosition;
    real mRadius;
};

struct Plane {
    RVec mNormal;
    RVec mPlanePoint;
    real mDistance;
    RVec mMax;
    RVec mMin;

    Plane() {
        mNormal = RVec(0.0f, 1.0f);
        mPlanePoint = RVec();
        mMax = RVec();
        mMin = RVec();
        mDistance = 0.0f;
    }

    Plane(RVec p1, RVec p2, RVec p3) {
        mPlanePoint = p1;
        mNormal = (p2 - p1).Cross(p3 - p1).Normalize();
        mDistance = (p1.Distance(p2) + p1.Distance(p3)) / 2.0f;

        mMax = RVec(__max(p1.x, __max(p2.x, p3.x)), __max(p1.y, __max(p2.y, p3.y)), __max(p1.z, __max(p2.z, p3.z))) + RVec(0.05f);
        mMin = RVec(__min(p1.x, __min(p2.x, p3.x)), __min(p1.y, __min(p2.y, p3.y)), __min(p1.z, __min(p2.z, p3.z))) - RVec(0.05f);
    }
};

/*struct CollisionMesh {
    std::vector<float> mVertices;

    static CollisionMesh LoadCollisionMesh(std::string obj_path) {
        std::ifstream f(obj_path, std::ios::binary);

        std::string line;

        CollisionMesh temp_data;

        temp_data.mVertices.clear();

        while(!f.eof()) {
            std::getline(f, line);

            if(line.find("v ") == 0) {
                float x, y, z;

                std::stringstream ss(line.c_str() + 2);

                ss >> x >> y >> z;

                temp_data.mVertices.push_back(x);
                temp_data.mVertices.push_back(y);
                temp_data.mVertices.push_back(z);
            }
        }

        return temp_data;
    }
};

inline bool MeshToMeshCollision(CollisionMesh m1, CollisionMesh m2) {
    std::vector<RVec> normals;
    std::vector<float> points = m1.mVertices;
    std::copy(m2.mVertices.begin(), m2.mVertices.end(), std::back_inserter(points));

    CollisionMesh* cm;

    if(m1.mVertices.size() < m2.mVertices.size()) {
        cm = &m1;
    }
    else {
        cm = &m2;
    }

    for(int i = 0; i < cm->mVertices.size() / 9; i++) {
        RVec normal = RVec::PlaneNormal(RVec(cm->mVertices[i * 9 + 0], cm->mVertices[i * 9 + 1], cm->mVertices[i * 9 + 2]), 
            RVec(cm->mVertices[i * 9 + 3], cm->mVertices[i * 9 + 4], cm->mVertices[i * 9 + 5]),
            RVec(cm->mVertices[i * 9 + 6], cm->mVertices[i * 9 + 7], cm->mVertices[i * 9 + 8]));

        normals.push_back(normal);
    }

    for(RVec normal : normals) {
        for(int i = 0; i < points.size() / 3; i++) {

        }
    }
}*/

inline bool BoxCanBeOnPlane(Box b, Plane p) {
    return (b.mMax.x > p.mMin.x || b.mMin.x > p.mMin.x) && (b.mMax.x < p.mMax.x || b.mMin.x < p.mMax.x) && 
        (b.mMax.y > p.mMin.y || b.mMin.y > p.mMin.y) && (b.mMax.y < p.mMax.y || b.mMin.y < p.mMax.y) && 
        (b.mMax.z > p.mMin.z || b.mMin.z > p.mMin.z) && (b.mMax.z < p.mMax.z || b.mMin.z < p.mMax.z);
}

bool AABBToPointCollision(Box b, RVec point) {
    bool collisionX = point.x >= b.mMin.x && point.x <= b.mMax.x;
    bool collisionY = point.y >= b.mMin.y && point.y <= b.mMax.y;
    bool collisionZ = point.z >= b.mMin.z && point.z <= b.mMax.z;

    return collisionX && collisionY && collisionZ;
}

bool AABBToAABBCollision(Box b1, Box b2) {
    bool collisionX = (b1.GetPosition() - b1.GetSize() + (b1.GetSize() * 2.0f)).x <= (b2.GetPosition() - b2.GetSize()).x && (b2.GetPosition() - b2.GetSize() + (b2.GetSize() * 2.0f)).x >= (b1.GetPosition() - b1.GetSize()).x;
    bool collisionY = (b1.GetPosition() - b1.GetSize() + (b1.GetSize() * 2.0f)).y <= (b2.GetPosition() - b2.GetSize()).y && (b2.GetPosition() - b2.GetSize() + (b2.GetSize() * 2.0f)).y >= (b1.GetPosition() - b1.GetSize()).y;
    bool collisionZ = (b1.GetPosition() - b1.GetSize() + (b1.GetSize() * 2.0f)).z <= (b2.GetPosition() - b2.GetSize()).z && (b2.GetPosition() - b2.GetSize() + (b2.GetSize() * 2.0f)).z >= (b1.GetPosition() - b1.GetSize()).z;

    return collisionX && collisionY && collisionZ;
}

bool SphereToPointCollision(Sphere s, RVec point) {
    return s.mPosition.Distance(point) <= s.mRadius;
}

bool SphereToSphereCollisions(Sphere s1, Sphere s2) {
    return abs(s1.mRadius) + abs(s2.mRadius) >= s1.mPosition.Distance(s2.mPosition);
}

bool SphereToAABBCollision(Sphere s, Box b) {
    RVec point(__max(b.mMin.x, __min(s.mPosition.x, b.mMax.x)), __max(b.mMin.y, __min(s.mPosition.y, b.mMax.y)), __max(b.mMin.z, __min(s.mPosition.z, b.mMax.z)));

    return point.Distance(s.mPosition) < s.mRadius;
}

RVec PointOnPlane(RVec pv1, RVec pv2, RVec pv3, RVec point) {
    RVec plane_normal = (pv2 - pv1).Cross(pv3 - pv1).Normalize();
    real point_length = (plane_normal.Dot(point - pv1));
    RVec point_on_plane = point - (plane_normal * point_length);

    return point_on_plane;
}

bool BoxToPlaneCollision(Box b, Plane p) {
    RVec plane_point = (b.GetPosition() - (p.mNormal * b.GetSize())) - (p.mNormal * (p.mNormal.Dot((b.GetPosition() - (p.mNormal * b.GetSize())) - p.mPlanePoint)));
    RVec plane_point2 = (b.GetPosition() + (p.mNormal * b.GetSize())) - (p.mNormal * (p.mNormal.Dot((b.GetPosition() + (p.mNormal * b.GetSize())) - p.mPlanePoint)));

    return AABBToPointCollision(b, plane_point) || AABBToPointCollision(b, plane_point2);
}

RVec PointOnPlaneXZ(RVec pv1, RVec pv2, RVec pv3, RVec point, real offset = 2.5f) {
    RVec max_p(__max(pv1.x, __max(pv2.x, pv3.x)), __max(pv1.y, __max(pv2.y, pv3.y)), __max(pv1.z, __max(pv2.z, pv3.z)));
    RVec min_p(__min(pv1.x, __min(pv2.x, pv3.x)), __min(pv1.y, __min(pv2.y, pv3.y)), __min(pv1.z, __min(pv2.z, pv3.z)));
    RVec plane_normal = (pv2 - pv1).Cross(pv3 - pv1).Normalize();
    real point_length = sqrt(plane_normal.Dot(point - pv1));
    RVec point_on_plane = point - (plane_normal * point_length);

    if(point.x <= max_p.x + offset && point.z <= max_p.z + offset && point.x >= min_p.x - offset && point.z >= min_p.z - offset) {
        return point_on_plane;
    }
    else {
        return RVec(FLT_MAX);
    }
}

real PlaneToPointDistance(RVec pv1, RVec pv2, RVec pv3, RVec point, real offset = 2.5f) {
    RVec max_p(__max(pv1.x, __max(pv2.x, pv3.x)), __max(pv1.y, __max(pv2.y, pv3.y)), __max(pv1.z, __max(pv2.z, pv3.z)));
    RVec min_p(__min(pv1.x, __min(pv2.x, pv3.x)), __min(pv1.y, __min(pv2.y, pv3.y)), __min(pv1.z, __min(pv2.z, pv3.z)));
    RVec plane_normal = (pv2 - pv1).Cross(pv3 - pv1).Normalize();
    real point_length = sqrt(plane_normal.Dot(point - pv1));
    RVec point_on_plane = point - (plane_normal * point_length);

    if(point.x <= max_p.x + offset && point.y <= max_p.y + offset && point.z <= max_p.z + offset && point.x >= min_p.x - offset && point.y >= min_p.y - offset && point.z >= min_p.z - offset) {
        return point.Distance(point_on_plane);
    }
    else {
        return FLT_MAX;
    }
}

bool PlaneToPointCollision(RVec pv1, RVec pv2, RVec pv3, RVec point, real fixed_distance = 0.2f, real offset = 2.5f) {
    RVec max_p(__max(pv1.x, __max(pv2.x, pv3.x)), __max(pv1.y, __max(pv2.y, pv3.y)), __max(pv1.z, __max(pv2.z, pv3.z)));
    RVec min_p(__min(pv1.x, __min(pv2.x, pv3.x)), __min(pv1.y, __min(pv2.y, pv3.y)), __min(pv1.z, __min(pv2.z, pv3.z)));
    RVec plane_normal = (pv2 - pv1).Cross(pv3 - pv1).Normalize();
    real point_length = sqrt(plane_normal.Dot(point - pv1));
    RVec point_on_plane = point - (plane_normal * point_length);

    if(point.x <= max_p.x + offset && point.y <= max_p.y + offset && point.z <= max_p.z + offset && point.x >= min_p.x - offset && point.y >= min_p.y - offset && point.z >= min_p.z - offset) {
        return point.Distance(point_on_plane) < fixed_distance;
    }
    else {
        return false;
    }
}

#endif