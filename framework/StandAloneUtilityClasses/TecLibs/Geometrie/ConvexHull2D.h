#pragma once
#include <algorithm>

template<typename ... addTypes>
class ConvexHull2D
{
protected:

	std::vector<std::tuple<v2f, addTypes...>>	mVCloud;

	float										mMiniDistPercent=0.0f;

	std::pair< std::vector<size_t>, std::vector<size_t> > separateList(const std::pair<size_t, size_t>& line, const std::vector<size_t>& input, std::pair<size_t, size_t>& farther)
	{

		std::vector<size_t> left, right;
		farther.first = farther.second = -1;

		v2f vline(std::get<0>(mVCloud[line.second]) - std::get<0>(mVCloud[line.first]));

		v2f normal(vline.y, -vline.x);
		float norm = Norm(normal);
		if (norm > 0.0f)
		{
			normal *= 1.0f / norm;
			norm *= mMiniDistPercent;

			v2f maxdotdist(0.0f,0.0f);
			v2f maxfulldist(0.0f, 0.0f);

			for (auto i : input)
			{
				if ((i != line.first) && (i != line.second))
				{
					v2f vtest(std::get<0>(mVCloud[i]) - std::get<0>(mVCloud[line.first]));
					float dot = Dot(vtest, normal);
					if (dot < -norm)
					{
						left.push_back(i);
						if (maxdotdist.x >= dot)
						{
							float fullDist = NormSquare(vtest);
							if ((maxdotdist.x > dot) || ((maxdotdist.x == dot) && (fullDist> maxfulldist.x)))
							{
								maxdotdist.x = dot;
								maxfulldist.x = fullDist;
								farther.first = i;
							}
						}
					}
					else if (dot > norm)
					{
						right.push_back(i);
						if (maxdotdist.y <= dot)
						{
							float fullDist = NormSquare(vtest);
							if ((maxdotdist.y < dot) || ((maxdotdist.y == dot) && (fullDist > maxfulldist.y)))
							{
								maxdotdist.y = dot;
								maxfulldist.y = fullDist;
								farther.second = i;
							}
						}
					}
				}
			}
		}

		return { left,right };
	}
	std::pair<size_t, size_t> getSeparatingLine()
	{
		size_t i = 0;
		v2f minp, maxp;
		v2i mini(0, 0), maxi(0, 0);
		
		for (const auto& v : mVCloud)
		{
			const v2f& current = std::get<0>(v);
			if (i)
			{
				for (size_t coord = 0; coord < 2; coord++)
				{
					if (current[coord] == minp[coord])
					{
						if (current[1 - coord] <= minp[1 - coord])
						{
							minp[coord] = current[coord];
							mini[coord] = i;
						}
					}
					else if (current[coord] < minp[coord])
					{
						minp[coord] = current[coord];
						mini[coord] = i;
					}
					if (current[coord] > maxp[coord])
					{
						maxp[coord] = current[coord];
						maxi[coord] = i;
					}
					else if (current[coord] == maxp[coord])
					{
						if (current[1-coord] >= maxp[1-coord])
						{
							maxp[coord] = current[coord];
							maxi[coord] = i;
						}
					}
				}
			}
			else
			{
				minp = current;
				maxp = current;
			}
			++i;
		}

		if (mini.x == maxi.x)
		{
			return { mini.y , maxi.y };
		}
		return { mini.x , maxi.x };
	}


	void	getHalfHull(int level, const std::pair<size_t, size_t>& line, const std::vector<size_t>& input, std::vector<size_t>& result)
	{
		std::pair<size_t, size_t> farther;
		std::pair< std::vector<size_t>, std::vector<size_t> > sep = separateList(line, input, farther);

		if (farther.second != -1)
		{
			getHalfHull(level + 1, { line.first,farther.second }, sep.second, result);
			getHalfHull(level + 1, { farther.second,line.second }, sep.second, result);
		}
		else
		{
			result.push_back(line.second);
		}

		if (level == 0) // do other side
		{
			if (farther.first != -1)
			{
				getHalfHull(level + 1, { line.second,farther.first }, sep.first, result);
				getHalfHull(level + 1, { farther.first,line.first }, sep.first, result);
			}
			else
			{
				result.push_back(line.second);
			}
		}
	}

	std::vector<size_t>	getHull(const std::vector<size_t>& input)
	{
		std::vector<size_t> result;

		std::pair<size_t, size_t> line=getSeparatingLine();

		if (line.first == line.second)
		{
			result.push_back(line.first);
		}
		else
		{
			getHalfHull(0, line, input, result);
		}
		return result;
	}

public:
	ConvexHull2D(const std::vector<std::tuple<v2f, addTypes...>>& vlist,float precision=0.0f) : mVCloud(vlist), mMiniDistPercent(precision)
	{
		

	}

	std::vector<size_t>	getHull()
	{
		std::vector<size_t>	plist;
		plist.resize(mVCloud.size());
		size_t i = 0;
		for (auto& p : plist)
		{
			p = i;
			i++;
		}
		return getHull(plist);
	}
};
