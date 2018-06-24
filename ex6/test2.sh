echo "Warp four of the half-resolution input image"
bin/Panorama sphrWarp data/test2/pano1_0008.tga data/test2/warp08.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0009.tga data/test2/warp09.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0010.tga data/test2/warp10.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0011.tga data/test2/warp11.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0012.tga data/test2/warp12.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0013.tga data/test2/warp13.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0014.tga data/test2/warp14.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0015.tga data/test2/warp15.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0016.tga data/test2/warp16.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0017.tga data/test2/warp17.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0018.tga data/test2/warp18.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0019.tga data/test2/warp19.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0020.tga data/test2/warp20.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0021.tga data/test2/warp21.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0022.tga data/test2/warp22.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0023.tga data/test2/warp23.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0024.tga data/test2/warp24.tga 595 -0.15
bin/Panorama sphrWarp data/test2/pano1_0025.tga data/test2/warp25.tga 595 -0.15

echo "Generate features"
bin/Features computeFeatures data/test2/warp08.tga data/test2/warp08.f
bin/Features computeFeatures data/test2/warp09.tga data/test2/warp09.f
bin/Features computeFeatures data/test2/warp10.tga data/test2/warp10.f
bin/Features computeFeatures data/test2/warp11.tga data/test2/warp11.f
bin/Features computeFeatures data/test2/warp12.tga data/test2/warp12.f
bin/Features computeFeatures data/test2/warp13.tga data/test2/warp13.f
bin/Features computeFeatures data/test2/warp14.tga data/test2/warp14.f
bin/Features computeFeatures data/test2/warp15.tga data/test2/warp15.f
bin/Features computeFeatures data/test2/warp16.tga data/test2/warp16.f
bin/Features computeFeatures data/test2/warp17.tga data/test2/warp17.f
bin/Features computeFeatures data/test2/warp18.tga data/test2/warp18.f
bin/Features computeFeatures data/test2/warp19.tga data/test2/warp19.f
bin/Features computeFeatures data/test2/warp20.tga data/test2/warp20.f
bin/Features computeFeatures data/test2/warp21.tga data/test2/warp21.f
bin/Features computeFeatures data/test2/warp22.tga data/test2/warp22.f
bin/Features computeFeatures data/test2/warp23.tga data/test2/warp23.f
bin/Features computeFeatures data/test2/warp24.tga data/test2/warp24.f
bin/Features computeFeatures data/test2/warp25.tga data/test2/warp25.f

echo "Compute matches"
bin/Features matchFeatures data/test2/warp08.f data/test2/warp09.f 0.8 data/test2/match-08-09.txt 2
bin/Features matchFeatures data/test2/warp09.f data/test2/warp10.f 0.8 data/test2/match-09-10.txt 2
bin/Features matchFeatures data/test2/warp10.f data/test2/warp11.f 0.8 data/test2/match-10-11.txt 2
bin/Features matchFeatures data/test2/warp11.f data/test2/warp12.f 0.8 data/test2/match-11-12.txt 2
bin/Features matchFeatures data/test2/warp12.f data/test2/warp13.f 0.8 data/test2/match-12-13.txt 2
bin/Features matchFeatures data/test2/warp13.f data/test2/warp14.f 0.8 data/test2/match-13-14.txt 2
bin/Features matchFeatures data/test2/warp14.f data/test2/warp15.f 0.8 data/test2/match-14-15.txt 2
bin/Features matchFeatures data/test2/warp15.f data/test2/warp16.f 0.8 data/test2/match-15-16.txt 2
bin/Features matchFeatures data/test2/warp16.f data/test2/warp17.f 0.8 data/test2/match-16-17.txt 2
bin/Features matchFeatures data/test2/warp17.f data/test2/warp18.f 0.8 data/test2/match-17-18.txt 2
bin/Features matchFeatures data/test2/warp18.f data/test2/warp19.f 0.8 data/test2/match-18-19.txt 2
bin/Features matchFeatures data/test2/warp19.f data/test2/warp20.f 0.8 data/test2/match-19-20.txt 2
bin/Features matchFeatures data/test2/warp20.f data/test2/warp21.f 0.8 data/test2/match-20-21.txt 2
bin/Features matchFeatures data/test2/warp21.f data/test2/warp22.f 0.8 data/test2/match-21-22.txt 2
bin/Features matchFeatures data/test2/warp22.f data/test2/warp23.f 0.8 data/test2/match-22-23.txt 2
bin/Features matchFeatures data/test2/warp23.f data/test2/warp24.f 0.8 data/test2/match-23-24.txt 2
bin/Features matchFeatures data/test2/warp24.f data/test2/warp25.f 0.8 data/test2/match-24-25.txt 2

echo "Align the pairs using feature matching:"
bin/Panorama alignPair data/test2/warp08.f data/test2/warp09.f data/test2/match-08-09.txt 200 1
bin/Panorama alignPair data/test2/warp09.f data/test2/warp10.f data/test2/match-09-10.txt 200 1
bin/Panorama alignPair data/test2/warp10.f data/test2/warp11.f data/test2/match-10-11.txt 200 1
bin/Panorama alignPair data/test2/warp11.f data/test2/warp12.f data/test2/match-11-12.txt 200 1
bin/Panorama alignPair data/test2/warp12.f data/test2/warp13.f data/test2/match-12-13.txt 200 1
bin/Panorama alignPair data/test2/warp13.f data/test2/warp14.f data/test2/match-13-14.txt 200 1
bin/Panorama alignPair data/test2/warp14.f data/test2/warp15.f data/test2/match-14-15.txt 200 1
bin/Panorama alignPair data/test2/warp15.f data/test2/warp16.f data/test2/match-15-16.txt 200 1
bin/Panorama alignPair data/test2/warp16.f data/test2/warp17.f data/test2/match-16-17.txt 200 1
bin/Panorama alignPair data/test2/warp17.f data/test2/warp18.f data/test2/match-17-18.txt 200 1
bin/Panorama alignPair data/test2/warp18.f data/test2/warp19.f data/test2/match-18-19.txt 200 1
bin/Panorama alignPair data/test2/warp19.f data/test2/warp20.f data/test2/match-19-20.txt 200 1
bin/Panorama alignPair data/test2/warp20.f data/test2/warp21.f data/test2/match-20-21.txt 200 1
bin/Panorama alignPair data/test2/warp21.f data/test2/warp22.f data/test2/match-21-22.txt 200 1
bin/Panorama alignPair data/test2/warp22.f data/test2/warp23.f data/test2/match-22-23.txt 200 1
bin/Panorama alignPair data/test2/warp23.f data/test2/warp24.f data/test2/match-23-24.txt 200 1
bin/Panorama alignPair data/test2/warp24.f data/test2/warp25.f data/test2/match-24-25.txt 200 1

echo "Finally, blend these four images together"
bin/Panorama blendPairs data/test2/pairlist.txt data/test2/stitch2.tga 200
