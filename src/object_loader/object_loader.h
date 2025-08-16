#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <tuple>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <limits>
#include "math/own_math.h"

namespace objmini {



struct Vertex {
    Vector3 pos;
    Vector3 norm;
    float u=0, v=0;
};

struct Material {
    std::string name;
    Vector3 Ka{0,0,0}; // ambient
    Vector3 Kd{1,1,1}; // diffuse
    Vector3 Ks{0,0,0}; // specular
};

struct Submesh {
    int material = -1;           // index into materials
    uint32_t indexOffset = 0;    // starting index into indices
    uint32_t indexCount  = 0;    // number of indices
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;         // triangle list
    std::vector<Material> materials;       // by index
    std::vector<Submesh> submeshes;        // contiguous index ranges per material (in draw order)
};

// ----------------- Utility -----------------
inline static Vector3 cross(const Vector3& a, const Vector3& b){ return {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x}; }
inline static float dot(const Vector3& a, const Vector3& b){ return a.x*b.x + a.y*b.y + a.z*b.z; }
inline static float length(const Vector3& v){ return std::sqrt(objmini::dot(v,v)); }
inline static Vector3 normalize(const Vector3& v){ float L=length(v); return (L>1e-8f)?Vector3{v.x/L,v.y/L,v.z/L}:Vector3{0,1,0}; }

// Convert OBJ index (1-based, can be negative) to 0-based absolute
inline static int fixIndex(int idx, int count){
    if (idx > 0) return idx - 1;
    if (idx < 0) return count + idx; // -1 is last
    throw std::runtime_error("OBJ index of 0 is invalid");
}

// Parse an .mtl string (very small subset: newmtl, Ka, Kd, Ks)
inline static std::vector<Material> ParseMTL(const std::string& mtlText){
    std::vector<Material> mats; Material cur; bool has=false;
    std::istringstream ss(mtlText); std::string line;
    auto emit = [&](){ if(has){ mats.push_back(cur);} };
    while (std::getline(ss, line)){
        // strip leading spaces
        size_t i=0; while (i<line.size() && std::isspace((unsigned char)line[i])) ++i;
        if (i>=line.size() || line[i]=='#') continue;
        std::istringstream ls(line.substr(i));
        std::string tag; ls >> tag; if(tag.empty()) continue;
        if (tag == "newmtl"){
            emit(); cur = Material{}; has = true; ls >> cur.name; if(cur.name.empty()) cur.name = "(unnamed)";
        } else if (tag == "Ka"){
            ls >> cur.Ka.x >> cur.Ka.y >> cur.Ka.z;
        } else if (tag == "Kd"){
            ls >> cur.Kd.x >> cur.Kd.y >> cur.Kd.z;
        } else if (tag == "Ks"){
            ls >> cur.Ks.x >> cur.Ks.y >> cur.Ks.z;
        } else {
            // ignore others
        }
    }
    emit();
    return mats;
}

// ----------------- OBJ Loader -----------------
// Load from strings; if you only have one .mtl file referenced by obj, pass it in mtlText.
// Returns Mesh with welded vertices and per-material submeshes.
inline static Mesh LoadOBJFromStrings(const std::string& objText, const std::string& mtlText = std::string(), float scale=1.0f){
    // 1) Materials
    Mesh out{};
    std::unordered_map<std::string,int> matIndex; // name->index
    if(!mtlText.empty()){
        out.materials = ParseMTL(mtlText);
        for (int i=0;i<(int)out.materials.size();++i) matIndex[out.materials[i].name] = i;
    }

    // 2) Geometry sources
    std::vector<Vector3> srcPos; srcPos.reserve(1<<12);
    std::vector<Vector3> srcNrm; srcNrm.reserve(1<<12);
    std::vector<Vector2> srcUv;  srcUv.reserve(1<<12);

    // 3) Output welding map: key = (v,t,n)
    struct Key{ int v=-1,t=-1,n=-1; bool operator==(const Key& o) const { return v==o.v && t==o.t && n==o.n; } };
    struct KeyHash{ size_t operator()(const Key& k) const { return (size_t)k.v*73856093u ^ (size_t)k.t*19349663u ^ (size_t)k.n*83492791u; } };
    std::unordered_map<Key,uint32_t,KeyHash> cache;

    auto emitVertex = [&](const Key& k)->uint32_t{
        auto it = cache.find(k); if (it!=cache.end()) return it->second;
        Vertex v{};
        if (k.v>=0 && k.v<(int)srcPos.size()){ v.pos = srcPos[k.v]; v.pos.x *= scale; v.pos.y *= scale; v.pos.z *= scale; }
        if (k.t>=0 && k.t<(int)srcUv.size()){ v.u = srcUv[k.t].x; v.v = srcUv[k.t].y; }
        if (k.n>=0 && k.n<(int)srcNrm.size()){ v.norm = srcNrm[k.n]; }
        uint32_t idx = (uint32_t)out.vertices.size();
        out.vertices.push_back(v);
        cache.emplace(k, idx);
        return idx;
    };

    auto beginSubmesh = [&](int mat){
        if (!out.submeshes.empty() && out.submeshes.back().material == mat) return; // extend current
        Submesh sm; sm.material = mat; sm.indexOffset = (uint32_t)out.indices.size(); sm.indexCount = 0; out.submeshes.push_back(sm);
    };

    // 4) Parse OBJ
    std::istringstream ss(objText); std::string line; int currentMat = -1;
    while (std::getline(ss, line)){
        // trim leading spaces
        size_t i=0; while (i<line.size() && std::isspace((unsigned char)line[i])) ++i;
        if (i>=line.size() || line[i]=='#') continue;
        std::istringstream ls(line.substr(i));
        std::string tag; ls >> tag; if(tag.empty()) continue;

        if (tag == "v"){
            Vector3 p; ls >> p.x >> p.y >> p.z; srcPos.push_back(p);
        } else if (tag == "vt"){
            Vector2 t; ls >> t.x >> t.y; srcUv.push_back(t);
        } else if (tag == "vn"){
            Vector3 n; ls >> n.x >> n.y >> n.z; srcNrm.push_back(n);
        } else if (tag == "usemtl"){
            std::string name; ls >> name; auto it = matIndex.find(name);
            currentMat = (it==matIndex.end()? -1 : it->second);
            beginSubmesh(currentMat);
        } else if (tag == "mtllib"){
            // Ignored here; pass your MTL text via the function argument.
        } else if (tag == "f"){
            // Collect polygon vertices
            std::vector<Key> poly; poly.reserve(8);
            std::string vert;
            while (ls >> vert){
                // parse a token like v, v/t, v//n, v/t/n (v,t,n can be negative)
                int vi=0, ti=0, ni=0; int field=0; bool hasV=false, hasT=false, hasN=false;
                const char* s = vert.c_str(); int sign=1, val=0; auto flush=[&](){ if(!hasV){ vi = sign*val; hasV=true;} else if(!hasT){ ti = sign*val; hasT=true;} else { ni = sign*val; hasN=true;} sign=1; val=0; };
                for (size_t k=0;k<vert.size();++k){ char c=s[k];
                    if (c=='-'){ sign = -1; }
                    else if (c=='/'){
                        if (k==0 || s[k-1]=='/'){ // empty part
                            if(!hasV){ hasV=true; }
                            else if(!hasT){ hasT=true; }
                        } else flush();
                        ++field;
                    } else if (std::isdigit((unsigned char)c)){
                        val = val*10 + (c - '0');
                    } else { /* ignore */ }
                }
                if (val!=0 || sign==-1){ flush(); }
                int vcount = (int)srcPos.size();
                int tcount = (int)srcUv.size();
                int ncount = (int)srcNrm.size();
                Key kkey;
                if(hasV) kkey.v = fixIndex(vi, vcount);
                if(hasT) kkey.t = fixIndex(ti, tcount);
                if(hasN) kkey.n = fixIndex(ni, ncount);
                poly.push_back(kkey);
            }
            if (poly.size() < 3) continue;
            beginSubmesh(currentMat);
            // Triangulate as a fan: (0,i,i+1)
            for (size_t i1=1;i1+1<poly.size();++i1){
                uint32_t a = emitVertex(poly[0]);
                uint32_t b = emitVertex(poly[i1]);
                uint32_t c = emitVertex(poly[i1+1]);
                out.indices.push_back(a);
                out.indices.push_back(b);
                out.indices.push_back(c);
                out.submeshes.back().indexCount += 3;
            }
        } else {
            // ignore: g, o, s, etc.
        }
    }

    // 5) If some vertices don't have normals, compute smooth normals
    bool needNormals=false; for (auto& v : out.vertices){ if (std::fabs(v.norm.x)+std::fabs(v.norm.y)+std::fabs(v.norm.z) < 1e-7f){ needNormals=true; break; } }
    if (needNormals){
        std::vector<Vector3> acc(out.vertices.size(), {0,0,0});
        for (size_t i3=0;i3<out.indices.size(); i3+=3){
            uint32_t i0=out.indices[i3+0], i1=out.indices[i3+1], i2=out.indices[i3+2];
            Vector3 p0=out.vertices[i0].pos, p1=out.vertices[i1].pos, p2=out.vertices[i2].pos;
            Vector3 n = objmini::cross(Vector3{p1.x-p0.x,p1.y-p0.y,p1.z-p0.z}, Vector3{p2.x-p0.x,p2.y-p0.y,p2.z-p0.z});
            acc[i0].x+=n.x; acc[i0].y+=n.y; acc[i0].z+=n.z;
            acc[i1].x+=n.x; acc[i1].y+=n.y; acc[i1].z+=n.z;
            acc[i2].x+=n.x; acc[i2].y+=n.y; acc[i2].z+=n.z;
        }
        for (size_t i=0;i<out.vertices.size();++i) out.vertices[i].norm = objmini::normalize(acc[i]);
    }

    return out;
}

// Convenience: return just vertices + indices like you asked
inline static std::pair<std::vector<Vertex>, std::vector<uint32_t>>
BuildVerticesAndIndices(const std::string& objText, const std::string& mtlText = std::string(), float scale=1.0f){
    Mesh m = LoadOBJFromStrings(objText, mtlText, scale);
    return {std::move(m.vertices), std::move(m.indices)};
}

} // namespace objmini
