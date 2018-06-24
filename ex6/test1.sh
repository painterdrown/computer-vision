echo "Warp four of the half-resolution input image"
bin/Panorama sphrWarp data/test1/pano1_0008.tga data/test1/warp08.tga 595 -0.15
bin/Panorama sphrWarp data/test1/pano1_0009.tga data/test1/warp09.tga 595 -0.15
bin/Panorama sphrWarp data/test1/pano1_0010.tga data/test1/warp10.tga 595 -0.15
bin/Panorama sphrWarp data/test1/pano1_0011.tga data/test1/warp11.tga 595 -0.15

echo "Generate features"
bin/Features computeFeatures data/test1/warp08.tga data/test1/warp08.f
bin/Features computeFeatures data/test1/warp09.tga data/test1/warp09.f
bin/Features computeFeatures data/test1/warp10.tga data/test1/warp10.f
bin/Features computeFeatures data/test1/warp11.tga data/test1/warp11.f

echo "Compute matches"
bin/Features matchFeatures data/test1/warp08.f data/test1/warp09.f 0.8 data/test1/match-08-09.txt 2
bin/Features matchFeatures data/test1/warp09.f data/test1/warp10.f 0.8 data/test1/match-09-10.txt 2
bin/Features matchFeatures data/test1/warp10.f data/test1/warp11.f 0.8 data/test1/match-10-11.txt 2

echo "Align the pairs using feature matching:"
bin/Panorama alignPair data/test1/warp08.f data/test1/warp09.f data/test1/match-08-09.txt 200 1
bin/Panorama alignPair data/test1/warp09.f data/test1/warp10.f data/test1/match-09-10.txt 200 1
bin/Panorama alignPair data/test1/warp10.f data/test1/warp11.f data/test1/match-10-11.txt 200 1

echo "Finally, blend these four images together"
bin/Panorama blendPairs data/test1/pairlist.txt data/test1/stitch4.tga 200
