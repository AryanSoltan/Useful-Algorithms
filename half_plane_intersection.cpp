
const long double eps = 1e-9, inf = 1e9; 
struct Point { 
    long double x, y;
    explicit Point(long double x = 0, long double y = 0) : x(x), y(y) {}
    friend Point operator + (const Point& p, const Point& q) {
        return Point(p.x + q.x, p.y + q.y); 
    }
    friend Point operator - (const Point& p, const Point& q) { 
        return Point(p.x - q.x, p.y - q.y); 
    }
    friend Point operator * (const Point& p, const long double& k) { 
        return Point(p.x * k, p.y * k); 
    } 
    friend long double cross(const Point& p, const Point& q) { 
        return p.x * q.y - p.y * q.x; 
    }
};
struct Halfplane { 
    // 'p' is a passing point of the line and 'pq' is the direction vector of the line.
    Point p, pq; 
    long double angle;
    Halfplane() {}
    Halfplane(const Point& a, const Point& b) : p(a), pq(b - a) {
        angle = atan2l(pq.y, pq.x);    
    }
    // Every half-plane allows the region to the LEFT of its line.
    bool out(const Point& r) { 
        return cross(pq, r - p) < -eps; 
    }
    bool operator < (const Halfplane& e) const { 
        if (fabsl(angle - e.angle) < eps) return cross(pq, e.p - p) < 0;
        return angle < e.angle;
    } 
    bool operator == (const Halfplane& e) const { 
        return fabsl(angle - e.angle) < eps; 
    }
    friend Point inter(const Halfplane& s, const Halfplane& t) {
        long double alpha = cross((t.p - s.p), t.pq) / cross(s.pq, t.pq);
        return s.p + (s.pq * alpha);
    }
};
vector<Point> hp_intersect(vector<Halfplane>& H) {

    Point box[4] = {  // Bounding box in CCW order
        Point(inf, inf),
        Point(-inf, inf),
        Point(-inf, -inf),
        Point(inf, -inf)
    };
    for(int i = 0; i<4; i++) { // Add bounding box half-planes.
        Halfplane aux(box[i], box[(i+1) % 4]);
        H.push_back(aux);
    }
    sort(H.begin(), H.end());
    H.erase(unique(H.begin(), H.end()), H.end());
    deque<Halfplane> dq;
    int len = 0;
    for(int i = 0; i < int(H.size()); i++) {
        while (len > 1 && H[i].out(inter(dq[len-1], dq[len-2]))) {
            dq.pop_back();
            --len;
        }
        while (len > 1 && H[i].out(inter(dq[0], dq[1]))) {
            dq.pop_front();
            --len;
        }
        dq.push_back(H[i]);
        ++len;
    }
    while (len > 2 && dq[0].out(inter(dq[len-1], dq[len-2]))) {
        dq.pop_back();
        --len;
    }
    while (len > 2 && dq[len-1].out(inter(dq[0], dq[1]))) {
        dq.pop_front();
        --len;
    }
    if (len < 3) return vector<Point>();
    vector<Point> ret(len);
    for(int i = 0; i+1 < len; i++) {
        ret[i] = inter(dq[i], dq[i+1]);
    }
    ret.back() = inter(dq[len-1], dq[0]);
    return ret;
}