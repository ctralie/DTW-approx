#include <algorithm>
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "curve.h"

typedef Kernel::Iso_rectangle_2 BBox; //bounding box for 2D point sets

class SequentialWSPD
{
	public:
		SequentialWSPD(Curve curve1, Curve curve2, double s);
		
    // separation constant
    double s;
		
		vector<IndexSegment> segs1()
		{
			return segs1_;
		}
		
		vector<IndexSegment> segs2()
		{
			return segs2_;
		}

  private:
		Curve curve1_;
		Curve curve2_;
	
		vector<IndexSegment> segs1_;
		vector<IndexSegment> segs2_;

		void pairing(IndexSegment seg1, IndexSegment seg2);
		bool is_well_separated(IndexSegment seg1, IndexSegment seg2);
		double calc_side(BBox bbox);
};
