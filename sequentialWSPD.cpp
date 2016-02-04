#include <CGAL/number_utils.h>
#include <CGAL/bounding_box.h>
#include <CGAL/squared_distance_2.h>

#include "easylogging++.h"
#include "sequentialWSPD.h"

SequentialWSPD::SequentialWSPD(Curve curve1, Curve curve2, double s):
		s(s), curve1_(curve1), curve2_(curve2)
{
	pairing(make_pair(0, curve1_.size()), make_pair(0, curve2_.size());
}

void SequentialWSPD::pairing(IndexSegment seg1, IndexSegment seg2)
{
	//base case
	if (seg1.second - seg1.first == 1 && seg2.second - seg2.first == 1)
	{
		segs1.push_back(seg1);
		segs2.push_back(seg2);
		LOG(INFO) << "base case";
	}
	
	if (is_well_separated(seg1, seg2)
	{
		segs1.push_back(seg1);
		segs2.push_back(seg2);
	}
	else
	{
		if (seg1.second - seg1.first > seg2.second - seg2.first)
		{
			int mid = (seg1.second - seg1.first) / 2 + 1;
			pairing(make_pair(seg1.first, mid), seg2);
			pairing(make_pair(mid, seg1.second), seg2);
		}
		else
		{
			int mid = (seg2.second - seg2.first) / 2 + 1;
			pairing(seg1, make_pair(seg2.first, mid));
			pairing(seg1, make_pair(mid, seg2.second));
		}
}
	
bool SequentialWSPD::is_well_separated(IndexSegment seg1, IndexSegment seg2)
{
	BBox bbox1 = CGAL::bounding_box(curve1_.begin() + seg1.first, curve1_.begin() + seg1.second);
	BBox bbox2 = CGAL::bounding_box(curve2_.begin() + seg2.first, curve2_.begin() + seg2.second);
	
	Point_2 center1 = Point_2((bbox1.xmin() + bbox1.xmax()) / 2,
	                          (bbox1.ymin() + bbox1.ymax()) / 2);
	Point_2 center2 = Point_2((bbox1.xmin() + bbox1.xmax()) / 2,
	                          (bbox1.ymin() + bbox1.ymax()) / 2);
	
	if (max(calc_side(bbox1), calc_side(bbox2)) * s <= 
			CGAL::sqrt(CGAL::squared_distance(center1, center2))
	{
		return true;
	}
	else
	{
		return false;
	}
}

double SequentialWSPD::calc_side(BBox bbox)
{
	BBox bbox = CGAL::bounding_box(start, end);
	//center_ = Point_2((bbox.xmin() + bbox.xmax()) / 2,
	//                  (bbox.ymin() + bbox.ymax()) / 2);

	//Vector_2 v(bbox.max() - center_);
	//radius_ = CGAL::sqrt(v.squared_length());
	double width = bbox.xmax() - bbox.xmin();
	double height = bbox.ymax() - bbox.ymin();
	return width > height ? width : height;
}

