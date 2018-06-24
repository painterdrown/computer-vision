bin/Panorama sphrWarp data/test1/pano1_0008.tga data/test1/warp08.tga 595 -0.15 0.0
bin/Panorama sphrWarp data/test1/pano1_0009.tga data/test1/warp09.tga 595 -0.15 0.0

bin/Features computeFeatures data/test1/warp08.tga data/test1/warp08.f
bin/Features computeFeatures data/test1/warp09.tga data/test1/warp09.f

bin/Features matchSIFTFeatures data/test1/warp08.f data/test1/warp09.f data/test1/match-08-09.txt 2

bin/Panorama alignPair data/test1/warp08.f data/test1/warp09.f data/test1/match-08-09.txt 200 1 sift

bin/Panorama blendPairs data/test1/pairlist2.txt data/test1/stitch2.tga 200
