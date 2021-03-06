/*
* Copyright 2016 Nu-book Inc.
* Copyright 2016 ZXing authors
* Copyright 2020 Axel Waggershauser
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "GridSampler.h"

#ifndef NDEBUG
#include "LogMatrix.h"
#endif

namespace ZXing {

#ifndef NDEBUG
std::vector<PointF> theGrid;
#endif

DetectorResult SampleGrid(const BitMatrix& image, int width, int height, const PerspectiveTransform& transform)
{
#ifndef NDEBUG
	theGrid.clear();
	theGrid.reserve(width * height);
	LogMatrix log;
	log.init(&image, 5);
#endif
	auto project = [&](PointI p) { return PointI(transform(p + PointF(0.5, 0.5))); };
	auto isInside = [&](PointI p) {
		p = project(p);
		return 0 <= p.x && p.x < image.width() && 0 <= p.y && p.y < image.height();
	};

	if (width <= 0 || height <= 0 || !transform.isValid() || !isInside({0, 0}) || !isInside({width - 1, 0}) ||
		!isInside({width - 1, height - 1}) || !isInside({0, height - 1}))
		return {};

	BitMatrix res(width, height);
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x) {
			auto p = project({x, y});
#ifndef NDEBUG
			theGrid.emplace_back(p);
			log(p, 3);
#endif
			if (image.get(p))
				res.set(x, y);
		}
#ifndef NDEBUG
	log.write("grid.pnm");
#endif

	auto projectCorner = [&](PointI p) { return PointI(transform(PointF(p)) + PointF(0.5, 0.5)); };
	return {
		std::move(res),
		{projectCorner({0, 0}), projectCorner({width, 0}), projectCorner({width, height}), projectCorner({0, height})}};
}

} // ZXing
